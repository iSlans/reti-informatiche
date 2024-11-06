#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

struct ClientState {
    int is_logged;
};

struct ClientList {
    void (*init)();
    void (*remove)(int fd);
    struct ClientState* (*add)(int fd);
    struct ClientState* (*find)(int fd);
};

extern struct ClientList client_list;

#endif
