#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

struct ServerState {
    char ip[16];
    int port;
    int is_listening;
    int listener_fd;
    int n_clients;
    int can_close_server;
};

void handle_input(struct ServerState*);
void handle_listener(struct ServerState*);
void handle_client_fd(int fd, struct ServerState*);

enum Command {
    COMMAND_START,
    COMMAND_STOP,
    COMMAND_EMPTY,
    // COMMAND_CLOSE,
};

enum ClientCommand {
    CLIENT_USR,
    CLIENT_GMN,
    CLIENT_ADM,
    CLIENT_INVALID
};

#endif
