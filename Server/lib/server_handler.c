#include "server_handler.h"

#include <stdio.h>
#include <string.h>

#include "connection.h"
#include "fdcontroller.h"
#include "logging.h"
#include "other.h"
#include "utility.h"

enum Command {
    COMMAND_START,
    COMMAND_STOP,
    // COMMAND_CLOSE,
};

int get_command(char* arg0);

/**
 * Try parse a command from stdin
 * Return -1 if no command found
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
                port = connection.default_port;
                logging_warn("Invalid or no port input found, using default port...");
            }

            int listener = connection.listen(connection.default_ip, port);
            if (listener == -1) {
                logging_error("Could not start connection listener");
            }

            fd_controller.add(listener);
            // session->ip = connection.get_listener_ip();
            session->ip = connection.default_ip;
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

void handle_listener(struct ServerState* session) {
    int newfd = connection.accept();
    if (newfd == -1) {
        logging_error("");
        return;
    }

    fd_controller.add(newfd);
    session->n_clients++;

    logging_info(
        "New fd: %d"
        "\t"
        "Total connections: %d",
        newfd, session->n_clients);
}

void handle_client_fd(int fd, struct ServerState* session) {
    // connection.receive(fd);
    int ret;
    char buffer[128] = "original";
    char payload[128] = "OKLA";

    ret = connection.receive(fd, buffer);

    if (ret == 0) {
        // connection.close(fd);
        // fd_controller.remove(fd);
        return;
    }

    logging_debug("%s", buffer);

    connection.send(fd, payload);
}
