#ifndef CONNECTION_H
#define CONNECTION_H

struct Connection {
    int (*get_socket)();
    int (*connect)(const char* ip, int port);
    int (*close)();
    int (*request)(char* payload, char* response, unsigned int resp_len);
};

extern const struct Connection connection;

#endif
