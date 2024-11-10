#include "admin_page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "message.h"
#include "session.h"
#include "utility.h"

enum AdminCommand {
    // ADMIN_LOGIN,
    ADMIN_LIST,
    ADMIN_UPDATE,
    ADMIN_EXIT,
};

int get_admin_page_command(char*);
int do_list(char*);
int do_update(char*);

/**
 * Admin panel where the admin user can view and modify clients data
 * Simple flow:
 * 1. get a command
 * 2. send the request and get the response
 * 3. loop back until exit command
 */
void admin_page(struct Session* session) {
    int ret;

    if (!session->admin_mode) return;
    if (session->close_client) return;

    static const char* message_commands =
        "\n"
        "Admin Page\n"
        "Command list:\n"
        ">> list game                           \n"
        ">> update game <id> addtime <seconds> \n"
        ">> update game <id> subtime <seconds> \n"
        ">> exit                                \n";

    printf("%s\n", message_commands);

    do {
        char args[64] = "";
        int command = get_admin_page_command(args);

        switch (command) {
            case ADMIN_LIST:
                ret = do_list(args);
                if (ret == -1) exit(-1);
                break;

            case ADMIN_UPDATE:
                ret = do_update(args);
                if (ret == -1) exit(-1);
                break;

            case ADMIN_EXIT:
                session->admin_mode = 0;
                session->logged = 0;
                session->close_client = 1;
                return;
                break;

            default:;
        }

    } while (!session->close_client);

    return;
}

/**
 * Get a command from stdin
 * will loop until getting a valid command
 */
int get_admin_page_command(char* params) {
    enum AdminCommand command_id;
    int valid = 0;
    do {
        char command[16] = "";
        char args[64] = "";
        char line[128] = "";

        printf(">> ");
        get_input_line(line, sizeof(line));
        sscanf(line, "%15s %63[^\n]", command, args);

        if (strcmp(command, "") == 0) {
            continue;
        } else if (strcmp(command, "list") == 0) {
            command_id = ADMIN_LIST;
            valid = 1;
        } else if (strcmp(command, "update") == 0) {
            command_id = ADMIN_UPDATE;
            valid = 1;
        } else if (strcmp(command, "exit") == 0) {
            command_id = ADMIN_EXIT;
            valid = 1;
        }

        if (!valid) printf("Invalid command \n");
        if (valid) {
            strcpy(params, args);
        }
    } while (!valid);

    return command_id;
}

/**
 * Manage the "list <smt>" command, send the relative request to server
 * return -1 if error
 */
int do_list(char* args) {
    int ret;

    if (strcmp(args, "game") != 0) {
        printf("%s currently not supported\n", args);
        return 0;
    }

    // char line[128] = "";
    char payload[128];
    char response[1024];

    // Should do pagination
    // in case a lot of games are active and the returned list would be too large
    sprintf(payload, "ADM LIST %s", args);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server...\n");
        return -1;
    }

    if (strncmp(response, "OK", 2) == 0) {
        char msg[1024] = "";
        sscanf(response, "OK %1023[^\\0]", msg);
        printf("%s\n", msg);
        return 1;
    }
    printf("Unexpected response from server %s\n", response);
    return -1;
}

/**
 * Manage the "update <smt>" command
 */
int do_update(char* args) {
    int ret;

    char id[16] = "";
    char type[16] = "";
    int value = 0;

    if (strncmp(args, "game", 4) != 0) {
        printf("%s currently not supported\n", args);
        return 0;
    }

    sscanf(args, "game %s %s %d", id, type, &value);

    if (strcmp(type, "addtime") == 0) {
        value = value > 0 ? value : -value;
    } else if (strcmp(type, "subtime") == 0) {
        value = value < 0 ? value : -value;
    } else if (strcmp(type, "") == 0) {
        printf("No parameter to update specified\n");
        return 0;

    } else {
        printf("%s currently not supported\n", type);
        return 0;
    }

    char payload[128];
    char response[128];

    // wish we could use json...
    sprintf(payload, "ADM UPDATE game %s TIME %d", id, value);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, try close and restart application...\n");
        return -1;
    }

    if (strncmp(response, "OK", 2) == 0) {
        char msg[1024] = "";
        sscanf(response, "OK %1023[^\\0]", msg);
        printf("%s\n", msg);
        return 1;
    }
    return -1;
}
