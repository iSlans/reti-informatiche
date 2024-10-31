#include <stdio.h>
#include <unistd.h>

#include "lib/connection.h"
#include "lib/fdcontroller.h"
#include "lib/logging.h"
#include "lib/other.h"
#include "lib/utility.h"

#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"

struct ServerState {
    char* ip;
    int port;
    int listening;
    int listener_fd;
    int n_clients;
    int can_close_server;
};

void handle_client_fd(int fd, struct ServerState*);
void handle_input(struct ServerState*);

int main() {
    struct ServerState session = {
        .ip = "127.0.0.1",
        .port = 4242,
        .can_close_server = 0,
        .listening = 0,
        .listener_fd = -1,
        .n_clients = 0,
    };

    fd_controller.init();
    fd_controller.add(STDIN_FILENO);

    printf(
        "***************************** SERVER STARTED *********************************\n"
        "Digita un comando:\n"
        "1) start [port]    --> avvia il server di gioco sulla porta data o di default \n"
        "2) stop            --> termina il server\n"
        "\n"

    );

    do {
        fd_controller.update_ready_list();

        int maxfd = fd_controller.get_max_fd();
        for (int fd = 0; fd <= maxfd; fd++) {
            if (!fd_controller.is_ready(fd)) continue;

            if (fd == STDIN_FILENO) {
                handle_input(&session);

            } else if (fd == session.listener_fd) {
                // handle_listener(&session);

                int newfd = connection.accept();
                if (newfd == -1) {
                }

                fd_controller.add(newfd);
                session.n_clients++;
            } else {
                handle_client_fd(fd, &session);
            }
        }
    } while (!session.can_close_server);

    return 0;
}

enum Command {
    COMMAND_START,
    COMMAND_STOP,
    // COMMAND_CLOSE,
};

int get_command(char* arg0);

void handle_input(struct ServerState* session) {
    char arg0[16];
    int command_id = get_command(arg0);

    switch (command_id) {
        case COMMAND_START: {
            if (session->listening) {
                logging_warn("Already listening on %s:%d", session->ip, session->port);
                break;
            }
            int port = parse_port(arg0);
            if (port == -1) {
                port = DEFAULT_PORT;
                logging_warn("Invalid port input, using default port...");
            }

            int listener = connection.listen(DEFAULT_IP, port);
            if (listener == -1) {
                logging_error("Could not start connection listener");
            }

            fd_controller.add(listener);
            session->ip = DEFAULT_IP;
            session->port = port;
            session->listening = 1;
            session->listener_fd = listener;
            break;
        }
        case COMMAND_STOP: {
            if (session->n_clients > 0) {
                logging_warn("Can't close server, there are still %d active connections", session->n_clients);
                break;
            }
            if (session->listening) {
                connection.close_listener();
                session->listening = 0;
            }
            session->can_close_server = 1;
            break;
        }
        default:
            logging_warn("Unknown command");
            break;
    }
}

void handle_client_fd(int fd, struct ServerState* session) {
    // connection.receive(fd);
}

#include <stdio.h>
#include <string.h>

/**
 * Try parse a command from stdin
 * Return -1 if is not a command
 * No checks performed on arguments
 */
int get_command(char* arg0) {
    // struct CommandDict {
    //     char input_str[32];
    //     enum Command command_id;
    // };
    // struct CommandDict command_list[] = {
    //     {.input_str = 'start', .command_id = COMMAND_START},
    //     {.input_str = 'end', .command_id = COMMAND_STOP},
    // }

    char command[16] = "";
    char arg[16] = "";
    char line[32] = "";

    get_input_line(line, sizeof(line));
    sscanf(line, "%15s %15s", command, arg);

    if (strcmp(command, "start") == 0) {
        strcpy(arg0, arg);
        return COMMAND_START;
    } else if (strcmp(command, "stop") == 0) {
        return COMMAND_STOP;
    }
    // else if (strcmp(command, "close") == 0) {
    //     return COMMAND_CLOSE;
    // }

    return -1;
}
