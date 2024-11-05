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

static int get_command(char* arg0);

/**
 * Try parse a command from stdin
 * Return -1 if no command found
 * No checks performed on arguments
 */
static int get_command(char* arg0) {
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
    return -1;
}

void handle_input(struct ServerState* server) {
    char arg0[16];
    int command_id = get_command(arg0);

    switch (command_id) {
        case COMMAND_START: {
            if (server->is_listening) {
                logging_warn("Already listening on %s:%d", server->ip, server->port);
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
            // session->ip = connection.default_ip;
            strcpy(server->ip, connection.default_ip);
            server->port = port;
            server->is_listening = 1;
            server->listener_fd = listener;
            break;
        }
        case COMMAND_STOP: {
            if (server->n_clients > 0) {
                logging_warn("Can't close server, there are still %d active connections", server->n_clients);
                break;
            }
            if (server->is_listening) {
                connection.close_listener();
                server->is_listening = 0;
            }
            server->can_close_server = 1;
            break;
        }
        default:
            logging_warn("Unknown command");
            break;
    }
}

void handle_listener(struct ServerState* server) {
    int newfd = connection.accept();
    if (newfd == -1) {
        logging_error("");
        return;
    }

    fd_controller.add(newfd);
    server->n_clients++;

    logging_info(
        "New fd: %d"
        "\t"
        "Total connections: %d",
        newfd, server->n_clients);
}

struct ClientState {
    int is_logged;
};

enum COMMAND {
    USR,
    GMN,
    USR,
    INVALID
};
int parse_request(char* request, int* command, char* type, char* args);

void handle_client_fd(int fd, struct ServerState* server) {
    int ret;
    char request[128] = "buffer";
    char response[128] = "OKLA";

    ret = connection.receive(fd, request);

    if (ret == -1) {
        return;
    }
    if (ret == 0) {
        connection.close(fd);
        fd_controller.remove(fd);
        server->n_clients--;
        return;
    }

    logging_debug("%s", request);

    // request format: "CMD TYPE  args[]"
    //            e.g. "USR LOGIN mario password123"
    //            e.g. "GMN 1     use   object1     object2"
    int command;
    char type[128] = "";
    char args[256] = "";
    ret = parse_request(request, &command, type, args);

    if (ret == -1) {
        logging_warn("Client sent an invalid request: %s", request);
        // could send a invalid request error to client
        // connection.send(fd, "400");
        return;
    }

    struct ClientState* client = get_client_from_fd(fd);

    switch (command) {
        case USR: {
            // handle_user(client);
            // if(client->is_logged)

        } break;

        default:
            break;
    }

    // connection.send(fd, response);
}

int parse_request(char* request, int* command, char* type, char* args) {
    int valid = 0;
    char command_str[128];

    sscanf(request, "%15s %15s %256s", command_str, type, args);

    if (strcmp(command_str, "USR") == 0) {
        *command = USR;
        valid = 1;
    } else if (strcmp(command_str, "GMN") == 0) {
        // TODO
    }

    if (!valid) {
        *command = INVALID;
        return 1;
    }

    return -1;
}
