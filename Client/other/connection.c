#include "connection.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MOCK 1
#define MOCK_RESPONSE "OK"

#define MAX_PAYLOAD_SIZE 128
#define MAX_RESPONSE_SIZE 128

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

static int request(char* payload, char* response, unsigned int resp_len) {
    int ret;
    size_t payload_len;

    if (!conn_is_open) {
        printf("No connection opened yet\n");
        return -1;
    }

    payload_len = strlen(payload);
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

    ret = send(conn_socket, payload, payload_len, 0);
    if (ret == -1) {
        perror("Error sending payload: ");
        return -1;
    }

    ret = recv(conn_socket, response, resp_len, 0);
    if (ret == -1) {
        perror("Error receiving response: ");
        return -1;
    }

    return 0;
}

const struct Connection connection = {
    .get_socket = get_conn_socket,
    .connect = connect_server,
    .close = close_connection,
    .request = request,
};
