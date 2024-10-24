#include "connection.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int conn_socket;

int get_conn_socket() {
    return conn_socket;
}

int connect_server(const char* ip, int port) {
    int ret;
    struct sockaddr_in server_addr;

    conn_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    /*ret = bind(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)); */
    ret = connect(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Connect error");
        return -1;
    }

    return conn_socket;
}

void close_connection() {
    printf("closing connection\n");
    close(conn_socket);
}

const struct Connection connection = {
    .get_conn_socket = get_conn_socket,
    .connect = connect_server,
    .close = close_connection,
};
