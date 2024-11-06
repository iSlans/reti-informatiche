#include "server_handler.h"

#include <stdio.h>
#include <string.h>

#include "client_session.h"
#include "connection.h"
#include "fdcontroller.h"
#include "logging.h"
#include "utility.h"
// #include "other.h"

/* ---------------------------- utility functions --------------------------- */
static int get_command(char* arg0);
static void parse_request(char* request, int* command, char* type, char* args);

/* ------------------------------------ - ----------------------------------- */
int handle_user(struct ClientState* client, char* type, char* args);

void handle_input(struct ServerState* server) {
    char arg0[16];
    int command_id = get_command(arg0);

    if (command_id == COMMAND_EMPTY) {
        return;
    }

    if (command_id == COMMAND_START) {
        if (server->is_listening) {
            logging_warn("Already listening on %s:%d", server->ip, server->port);
            return;
        }

        const char* ip = connection.default_ip;
        int port = parse_port(arg0);

        if (port == -1) {
            port = connection.default_port;
            logging_warn("Invalid or no port input found, using default port...");
        }

        int listener = connection.listen(ip, port);
        if (listener == -1) {
            logging_error("Could not start connection listener");
            return;
        }

        fd_controller.add(listener);
        strcpy(server->ip, ip);
        server->port = port;
        server->is_listening = 1;
        server->listener_fd = listener;

        client_list.init();

        return;
    }

    if (command_id == COMMAND_STOP) {
        if (server->n_clients > 0) {
            logging_warn("Can't close server, still have %d active connection", server->n_clients);
            return;
        }
        if (server->is_listening) {
            connection.close_listener();
            server->is_listening = 0;
        }
        server->can_close_server = 1;

        return;
    }

    logging_warn("Unknown command");
}

void handle_listener(struct ServerState* server) {
    int newfd = connection.accept();
    if (newfd == -1) {
        logging_error("");
        return;
    }

    fd_controller.add(newfd);
    server->n_clients++;

    struct ClientState* client = client_list.add(newfd);

    client->is_logged = 0;

    logging_info(
        "New fd: %d"
        "\t"
        "Total connections: %d",
        newfd, server->n_clients);
}

void handle_client_fd(int fd, struct ServerState* server) {
    int ret;
    char request[128] = "buffer";
    char response[128] = "OKLA";

    ret = connection.receive(fd, request);
    if (ret == -1 || ret == 0) goto close_client;
    logging_debug("Got client message: %s", request);

    int command;
    char type[16] = "";
    char args[256] = "";
    parse_request(request, &command, type, args);

    if (command == CLIENT_INVALID) {
        logging_warn("Client sent an invalid request: %s", request);
        ret = connection.send(fd, "400");
        if (ret == -1) goto close_client;
        return;
    }

    struct ClientState* client = client_list.find(fd);
    if (client == NULL) goto close_client;

    if (command == CLIENT_USR) {
        ret = handle_user(client, type, args);
        if (ret == -1) goto close_client;

        ret = connection.send(fd, ret ? "OK" : "NK");
        if (ret == -1) goto close_client;

        return;
    }

    if (command == CLIENT_GMN) {
    }

    ret = connection.send(fd, "OK");
    if (ret == -1) goto close_client;

    return;
close_client:
    connection.close(fd);
    fd_controller.remove(fd);
    client_list.remove(fd);
    server->n_clients--;
}

/**
 *
 * returns 1 if operation success
 * returns 0 if login failed
 * returns -1 if errors or any unexpected situation
 * (no distinction between user flow error or server error)
 */
int handle_user(struct ClientState* client, char* type, char* args) {
    const char user_file[] = "user.txt";

    if (strcmp(type, "LOGOUT") == 0) {
        client->is_logged = 0;
        return 1;
    }

    if (strcmp(type, "SIGNUP") == 0) {
        if (client->is_logged) return -1;

        FILE* file;
        if ((file = fopen(user_file, "r")) == NULL) {
            logging_error("Error opening file user.txt");
            return -1;
        }

        char line[256] = "";
        char substring[256];
        char username[32];
        char password[32];
        sscanf(args, "%s %s", username, password);

        // Check if username already exists
        while (!feof(file)) {
            // get_line_from(file, line, sizeof line);
            fgets(line, sizeof line, file);

            sprintf(substring, "user:%s ", username);
            if (strstr(line, substring)) {
                logging_info("User already registered");
                return -1;
            }
        }
        fclose(file);

        // Add username, password to db
        if ((file = fopen(user_file, "a")) == NULL) {
            logging_error("Error opening file user.txt");
            return -1;
        }
        // sprintf(substring);
        fprintf(file, "user:%s pass:%s\n", username, password);
        fclose(file);
        client->is_logged = 1;

        return 1;
    }

    if (strcmp(type, "LOGIN") == 0) {
        if (client->is_logged) {
            return -1;
        }

        FILE* file;
        if ((file = fopen(user_file, "r")) == NULL) {
            logging_error("Error opening file user.txt");
            return -1;
        }

        char line[256] = "";
        char substring[256];
        char username[32];
        char password[32];
        sscanf(args, "%s %s", username, password);

        // Check credentials
        while (!feof(file)) {
            // get_line_from(file, line, sizeof line);
            fgets(line, sizeof line, file);

            sprintf(substring, "user:%s pass:%s", username, password);
            if (strstr(line, substring)) {
                logging_info("Login successful");
                client->is_logged = 1;
                fclose(file);
                return 1;
            }
        }
        fclose(file);

        logging_info("Credentials not found");
        return 0;
    }

    logging_error("Unknown user request type: %s", type);
    return -1;
}

/* -------------------------------------------------------------------------- */
/*                              utility functions                             */
/* -------------------------------------------------------------------------- */

/**
 * Try parse a command from stdin
 * Return -1 if invalid command found
 * No checks performed on arguments
 */
static int get_command(char* arg0) {
    char command[16] = "";
    char arg[16] = "";
    char line[32] = "";

    get_input_line(line, sizeof(line));
    sscanf(line, "%15s %15s", command, arg);

    if (strcmp(command, "") == 0) {
        return COMMAND_EMPTY;
    } else if (strcmp(command, "start") == 0) {
        strcpy(arg0, arg);
        return COMMAND_START;
    } else if (strcmp(command, "stop") == 0) {
        return COMMAND_STOP;
    }
    return -1;
}

static void parse_request(char* request, int* command, char* type, char* args) {
    char command_str[16];
    sscanf(request, "%15s %15s %255[^\\0]", command_str, type, args);

    *command = CLIENT_INVALID;
    if (strcmp(command_str, "USR") == 0) {
        *command = CLIENT_USR;
    } else if (strcmp(command_str, "GMN") == 0) {
        *command = CLIENT_GMN;
    }
}
