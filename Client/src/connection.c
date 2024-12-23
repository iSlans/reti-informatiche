#include "connection.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MOCK 0
#define MOCK_RESPONSE "OK"

#define MAX_PAYLOAD_SIZE 1024
#define MAX_RESPONSE_SIZE 1024

/**
 * Module for connection to server and data transmission
 *
 * It wraps the socket, send, recv.. functions and exposes "methods" to call
 */

// Could try make a struct member to contains these info,
// so would not limited to only one instance
/*limit scope*/
static int conn_socket;
static int conn_is_open = 0;

static int get_conn_socket() {
    if (!conn_is_open) {
        printf("No connection opened yet\n");
        return -1;
    }
    return conn_socket;
}

static int connect_server(const char* ip, int port) {
    int ret;
    struct sockaddr_in server_addr;

    // if (conn_is_open) ...

    conn_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (MOCK) {
        conn_is_open = 1;
        return conn_socket;
    }

    /*ret = bind(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)); */
    ret = connect(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Connect error");
        return -1;
    }

    conn_is_open = 1;
    return conn_socket;
}

static int close_connection() {
    int ret;
    if (!conn_is_open) {
        printf("There is no connection to close\n");
        return 0;
    }
    ret = close(conn_socket);
    if (ret == -1) {
        perror("Error closing connection: ");
        return -1;
    }
    conn_is_open = 0;
    /* printf("closing connection\n"); */
    return 0;
}

/**
 * Send a message to server and get a response
 * returns -1 if any error, e.g. connection unexpectedly closed
 */
static int request(char* payload, char* response, unsigned int resp_len) {
    int ret;
    size_t payload_len;

    if (!conn_is_open) {
        printf("No connection opened yet\n");
        return -1;
    }

    payload_len = strlen(payload) + 1;
    if (payload_len > MAX_PAYLOAD_SIZE) {
        printf("Payload size too big, max supported: %d\n", MAX_PAYLOAD_SIZE);
        return -1;
    }

    if (resp_len > MAX_RESPONSE_SIZE) {
        printf("Response size too big, max supported: %d\n", MAX_RESPONSE_SIZE);
        return -1;
    }

    if (MOCK) {
        strcpy(response, MOCK_RESPONSE);
        return 0;
    }

    /* -------------------------------------------------------------------------- */
    /*                                SEND PAYLOAD                                */
    /* -------------------------------------------------------------------------- */
    /**
     * Sending payload takes two send():
     * 1. the first to send the size of message, using binary data protocol
     * 2. the second to send the message, using text data protocol
     */

    uint16_t msg_len;

    msg_len = htons(payload_len);

    ret = send(conn_socket, &msg_len, sizeof(uint16_t), MSG_NOSIGNAL);
    if (ret == -1) {
        perror("Error sending payload len");
        // if(errno == ECONNRESET){}
        // printf("Closed connection with server, please close and restart the client ");
        // conn_is_open = 0;
        return -1;
    }
    if (ret < sizeof(uint16_t)) {
        printf("Error sending payload len");
        return -1;
    }

    ret = send(conn_socket, payload, payload_len, MSG_NOSIGNAL);  // may give SIGPIPE if connection gets closed
    if (ret == -1) {
        perror("Error sending payload");
        return -1;
    }
    if (ret < payload_len) {
        return -1;
    }

    /* -------------------------------------------------------------------------- */
    /*                              RECEIVE RESPONSE                              */
    /* -------------------------------------------------------------------------- */
    /**
     * Here also takes two steps with two recv():
     * 1. the first to recv the size of message, using binary data protocol
     * 2. the second to recv the message, using text data protocol
     */

    char buffer[MAX_RESPONSE_SIZE];

    ret = recv(conn_socket, &msg_len, sizeof(uint16_t), 0);
    if (ret == -1) {
        perror("Error receiving response len");
        close(conn_socket);
        return -1;
    }
    if (ret == 0) {
        printf("Server closed connection...\n");
        conn_is_open = 0;
        return -1;
    }

    int len = ntohs(msg_len);
    if (len > MAX_RESPONSE_SIZE) {
        printf("Got server response size too big, max supported: %d\n", MAX_RESPONSE_SIZE);
        return -1;
    }
    ret = recv(conn_socket, buffer, len, 0);
    if (ret == -1) {
        perror("Error receiving response");
        return -1;
    }

    if (len > resp_len) {
        printf("Response buffer too small, will truncate message\n");
    }
    strncpy(response, buffer, resp_len);
    response[resp_len - 1] = '\0';

    return 0;
}

const struct Connection connection = {
    .get_socket = get_conn_socket,
    .connect = connect_server,
    .close = close_connection,
    .request = request,
};
