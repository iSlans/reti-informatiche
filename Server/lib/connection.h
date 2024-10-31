#ifndef CONNECTION_H
#define CONNECTION_H

struct Connection {
    /* data */
    int (*listen)(char* ip, int port);
    int (*accept)();
    int (*close_listener)();
    int (*get_listener)();
};

extern const struct Connection connection;

#endif
