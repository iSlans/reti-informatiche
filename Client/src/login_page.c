#include "login_page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "message.h"
#include "session.h"
#include "utility.h"

// #define USER_LEN 32
// #define PASS_LEN 32

/**
 * Module about Accounting, user can login/signup or enter admin account.
 *
 * Display the commands, performs some simple input check and send the request to server
 * The flow is pretty simple and self-explicative:
 * 1. read a input
 * 2. do the relative function
 * 3. loop until user is logged
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
                if (ret == -1) exit(-1);
                if (ret == 1) {
                    session->logged = 1;
                    session->admin_mode = 0;
                }
                break;

            case COMMAND_SIGNUP:
                ret = do_signup();
                if (ret == -1) exit(-1);
                if (ret == 1) {
                    session->logged = 1;
                    session->admin_mode = 0;
                }
                // session->logged = 1;
                break;

            case COMMAND_END:
                session->close_client = 1;
                return;
                break;

            case COMMAND_HELP:
                printf("\n%s\n", message.login_page);
                break;

            case COMMAND_ADMIN:
                ret = do_admin();
                if (ret == -1) exit(-1);
                if (ret == 1) {
                    session->logged = 1;
                    session->admin_mode = 1;
                }
                // session->logged = 1;
                break;
            default:;
        }

    } while (!session->logged);

    return;
}

/**
 * Get the user input from stdin and return the relative command
 * Loops until getting a valid command
 */
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
        } else if (strcmp(command, "admin") == 0) {
            command_id = COMMAND_ADMIN;
            valid = 1;
        }

        if (!valid) printf("Invalid command \n");
    } while (!valid);

    return command_id;
}

/**
 * Ask username and password and send the login request to server
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

    /* -------------------------------------------------------------------------- */
    /*                                 READ INPUT                                 */
    /* -------------------------------------------------------------------------- */

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

    /* -------------------------------------------------------------------------- */
    /*                                SEND REQUEST                                */
    /* -------------------------------------------------------------------------- */

    sprintf(payload, "USR LOGIN %s %s", user, pass);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server...\n");
        return -1;
    }

    /* -------------------------------------------------------------------------- */
    /*                               PARSE RESPONSE                               */
    /* -------------------------------------------------------------------------- */

    if (strcmp(response, "OK") == 0) {
        printf("Logged in!\n\n");
        return 1;
    }
    if (strcmp(response, "NK") == 0) {
        printf("Wrong credentials!\n");
        return 0;
    }

    printf("Unexpected response from server %s\n", response);
    return -1;
}

/**
 * Similar to login, here ask username and password to register a new account
 */
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
    // return 0;

    sprintf(payload, "USR SIGNUP %s %s", user, pass);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, try close and restart application...\n");
        return -1;
    }

    if (strcmp(response, "OK") == 0) {
        return 1;
    }

    return -1;
}

/**
 * Same as login, but for admin account
 */
int do_admin() {
    int ret;
    int valid = 0;

    char user[32] = "";
    char pass[32] = "";
    char line[128] = "";
    char payload[128];
    char response[128];

    do {
        printf("Enter <username> <password> of your admin account\n");
        get_input_line(line, sizeof(line));
        sscanf(line, "%31s %31s", user, pass);

        if (strcmp(user, "") == 0 || strcmp(pass, "") == 0) {
            printf("Invalid input\n");
        } else {
            valid = 1;
        }
    } while (!valid);

    sprintf(payload, "ADM LOGIN %s %s", user, pass);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, try close and restart application...\n");
        return -1;
    }

    if (strcmp(response, "OK") == 0) {
        return 1;
    } else if (strcmp(response, "NK") == 0) {
        printf("Wrong credentials!\n");
        return 0;
    }

    return -1;
}
