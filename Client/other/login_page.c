#include "login_page.h"

#include <stdio.h>
#include <string.h>

#include "connection.h"
#include "message.h"
#include "session.h"
#include "utility.h"

#define USER_LEN 32
#define PASS_LEN 32

/**
 * show initial screen
 * and manage login commands
 */
void login_page(struct Session* session) {
    int ret;

    if (session->logged) return;
    if (session->close_client) return;

    printf("%s\n", message.login_page);

    do {
        enum UserCommand command = get_login_page_command();

        switch (command) {
            case COMMAND_LOGIN:
                ret = do_login();
                if (ret == -1) continue;
                session->logged = 1;
                break;

            case COMMAND_SIGNUP:
                ret = do_signup();
                if (ret == -1) continue;
                session->logged = 1;
                break;

            case COMMAND_END:
                session->close_client = 1;
                return;
                break;

            case COMMAND_HELP:
                printf("\n%s\n", message.login_page);
                break;

            default:;
        }

    } while (!session->logged);

    return;
}

enum UserCommand get_login_page_command() {
    enum UserCommand command_id;
    int valid = 0;
    do {
        char command[16] = "";
        char line[16] = "";

        printf(">> ");
        get_input_line(line, sizeof(line));
        sscanf(line, "%15s", command);
        command[15] = '\0';

        if (strcmp(command, "") == 0) {
            continue;
        } else if (strcmp(command, "login") == 0) {
            command_id = COMMAND_LOGIN;
            valid = 1;
        } else if (strcmp(command, "signup") == 0) {
            command_id = COMMAND_SIGNUP;
            valid = 1;
        } else if (strcmp(command, "end") == 0) {
            command_id = COMMAND_END;
            valid = 1;
        } else if (strcmp(command, "help") == 0) {
            command_id = COMMAND_HELP;
            valid = 1;
        }

        if (!valid) printf("Invalid command \n");
    } while (!valid);

    return command_id;
}

/**
 *
 * return -1 if error or login unsuccessful
 */
int do_login() {
    int ret;
    int valid = 0;
    /*
    skip using USER_LEN PASS_LEN
    skip building format string...
    */
    char user[32] = "";
    char pass[32] = "";
    char line[128] = "";
    char payload[128];
    char response[128];

    do {
        /*
        - %s adds '\0' automatically,
        - password here can't contain spaces
        - bugged if line is a long string without space
        */
        printf("Enter <username> <password> \n");
        get_input_line(line, sizeof(line));
        sscanf(line, "%31s %31s", user, pass);

        if (strcmp(user, "") == 0 || strcmp(pass, "") == 0) {
            printf("Invalid input\n");
        } else {
            valid = 1;
        }
    } while (!valid);

    /* hash(pass); */

    sprintf(payload, "USR LOGIN %s %s", user, pass);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    if (strcmp(response, "OK") == 0) {
        return 0;
    }

    return -1;
}

int do_signup() {
    int ret;
    int valid = 0;

    char user[32] = "";
    char pass[32] = "";
    char line[128] = "";
    char payload[128];
    char response[128];

    do {
        printf("Enter <username> <password> \n");
        get_input_line(line, sizeof(line));
        sscanf(line, "%31s %31s", user, pass);

        if (strcmp(user, "") == 0 || strcmp(pass, "") == 0) {
            printf("Invalid input\n");
        } else {
            valid = 1;
        }
    } while (!valid);

    /* hash(pass); */

    return 0;

    sprintf(payload, "USR SIGNUP %s %s", user, pass);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    if (strcmp(response, "OK") == 0) {
        return 0;
    }

    return -1;
}
