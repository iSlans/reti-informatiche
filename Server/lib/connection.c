#include "connection.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

#define BACKLOG_SIZE 10
// #define DEFAULT_PORT 4242
// #define DEFAULT_IP "127.0.0.1"

// struct ConnectionData;
static int g_listener;

static int activate_listener_socket(char* ip, int port) {
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

    logging_info("Accepted new connection: %s %u", inet_ntoa(client.sin_addr), client.sin_port);

    return fd;
}

static int close_listener() {
    return close(g_listener);
}

static int get_listener_fd() {
    return g_listener;
}

const struct Connection connection = {
    .listen = activate_listener_socket,
    .accept = listener_accept,
    .close_listener = close_listener,
    .get_listener = get_listener_fd,
};
