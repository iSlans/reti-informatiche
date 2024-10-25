#include "login_page.h"

#include <stdio.h>
#include <string.h>

#include "connection.h"
#include "utility.h"

#define USER_LEN 32
#define PASS_LEN 32

int get_login_page_command() {
    int command_id;
    int valid = 0;
    do {
        printf(">> ");

        char command[16] = "";
        char line[16] = "";
        get_input_line(line, 16);
        sscanf(line, "%5s", command);
        command[5] = '\0';

        if (strcmp(command, "") == 0)
            continue;
        else if (strcmp(command, "login") == 0) {
            command_id = 0;
            valid = 1;
        } else if (strcmp(command, "signup") == 0) {
            command_id = 1;
            valid = 1;
        } else if (strcmp(command, "end") == 0) {
            command_id = 2;
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
        printf("Enter <username> <password> \n");
        get_input_line(line, sizeof(line));
        /*
        %s adds '\0' automatically,
        password here can't contain spaces
        */
        sscanf(line, "%31s %31s", user, pass);

        if (strcmp(user, "") == 0 || strcmp(pass, "") == 0) {
            printf("Invalid input\n");
        } else {
            valid = 1;
        }
    } while (!valid);

    /* hash(pass); */

    return 0;

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
    return -1;
}
