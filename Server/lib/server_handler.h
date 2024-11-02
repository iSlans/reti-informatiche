#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

struct ServerState {
    char* ip;
    int port;
    int listening;
    int listener_fd;
    int n_clients;
    int can_close_server;
};

void handle_input(struct ServerState*);
void handle_listener(struct ServerState*);
void handle_client_fd(int fd, struct ServerState*);

#endif
