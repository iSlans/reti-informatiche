#include "connection.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

#define MAX_REQUEST_LEN 128
#define MAX_RESPONSE_LEN 128

#define BACKLOG_SIZE 10
#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"

// struct ConnectionData;
static int g_listener;
static int g_port;
static char g_ip[16];

static int activate_listener_socket(const char* ip, int port) {
    struct sockaddr_in server_addr;
    int listener;
    int ret;

    listener = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listener == -1) {
        perror("Socket error");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    ret = bind(listener, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Bind error");
        return -1;
    }

    ret = listen(listener, BACKLOG_SIZE);
    if (ret == -1) {
        // if (errno != 0) {
        perror("Listen error");
        return -1;
    }

    logging_info("Server listening on %s:%d", ip, port);

    strcpy(g_ip, ip);
    g_port = port;
    g_listener = listener;
    return listener;
}

static int listener_accept(/*client_addr*/) {
    struct sockaddr_in client;
    socklen_t len;
    int fd;

    len = sizeof(client);

    fd = accept(g_listener, (struct sockaddr*)&client, &len);
    if (fd == -1) {
        perror("Error accept: ");
        return -1;
    }

    logging_info("Accepted new connection %d from %s:%u", fd, inet_ntoa(client.sin_addr), client.sin_port);

    return fd;
}

static int close_listener() {
    return close(g_listener);
}

static int get_listener_fd() {
    return g_listener;
}

/**
 * Receive a message from client
 *
 * return 0 if connection is closed
 * return -1 if any error
 */
static int receive_from_fd(int fd, char* buffer) {
    int ret;
    int len;
    // int error = 0;
    char buf[MAX_REQUEST_LEN];

    uint16_t msg_len;

    ret = recv(fd, &msg_len, sizeof(uint16_t), 0);
    if (ret == 0) {
        logging_info("recv returned 0 on fd %d", fd);
        return 0;
    }
    if (ret < sizeof(uint16_t)) {
        logging_error("Error recv size");

        // flush the request
        // recv(fd, buf, MAX_REQUEST_LEN, 0);
        return -1;
    }

    len = ntohs(msg_len);
    if (len > MAX_REQUEST_LEN) {
        logging_error("Got request length %d over the MAX LENGTH %d", len, MAX_REQUEST_LEN);
        return -1;
    }

    ret = recv(fd, buf, len, 0);
    if (ret < 0) {
        perror("Error recv: ");
        return -1;
    }
    if (ret == 0) {
        logging_info("recv returned 0 on fd %d", fd);
        return 0;
    }

    // if (error) return -1;

    buf[ret] = '\0';
    strcpy(buffer, buf);

    return ret;
}

static int send_to_fd(int fd, char* payload) {
    int ret;
    int len = strlen(payload) + 1;

    if (len > MAX_RESPONSE_LEN) {
        logging_error("payload length over MAX LENGTH");
        return -1;
    }

    uint16_t msg_len = htons(len);

    ret = send(fd, &msg_len, sizeof(uint16_t), 0);
    if (ret < 0) {
        perror("Error send msg len: ");
        return -1;
    }

    ret = send(fd, payload, len, 0);
    if (ret < 0) {
        perror("Error send payload: ");
        return -1;
    }

    return 0;
}

static int close_fd(int fd) {
    return close(fd);
}

const struct Connection connection = {
    .listen = activate_listener_socket,
    .accept = listener_accept,
    .close_listener = close_listener,
    .get_listener = get_listener_fd,
    .receive = receive_from_fd,
    .send = send_to_fd,
    .close = close_fd,
    .default_ip = DEFAULT_IP,
    .default_port = DEFAULT_PORT,
};
