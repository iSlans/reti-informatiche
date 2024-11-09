#include "game_page.h"

#include <stdio.h>
#include <string.h>
// #include <unistd.h>  //sleep

#include "connection.h"
#include "games_list.h"
#include "message.h"
#include "utility.h"

enum UserCommand get_game_page_command(char* arg0);
int get_game_idx_from_id(char* gameid);
int do_start(char* arg0);
int do_logout();

/**
 * Main Game Page function
 * show games list and commands
 * and execute the commands
 */
void game_page(struct Session* session) {
    int ret;
    int can_exit = 0;

    if (session->close_client) return;
    if (!session->logged) {
        printf("User not logged in");
        return;
    }

    printf("%s\n", message.game_list_page);

    do {
        char arg[16];
        enum UserCommand command = get_game_page_command(arg);

        // TODO dismantle this long switch
        switch (command) {
            case COMMAND_START:
                ret = do_start(arg);
                if (ret == -1) {
                    continue;
                }
                can_exit = 1;
                break;

            case COMMAND_LOGOUT:
                ret = do_logout();
                if (ret == -1) {
                    continue;
                }
                session->logged = 0;
                can_exit = 1;
                printf(
                    "Logged out\n"
                    "Returning to Login page...\n\n");
                // sleep(1);
                break;

            case COMMAND_END:
                session->close_client = 1;
                can_exit = 1;
                break;

            case COMMAND_HELP:
                printf("\n%s\n", message.game_list_page);
                break;

            default:;
        }

    } while (!can_exit);

    return;
}

/**
 *
 * return -1 if error
 */
int do_start(char* arg0) {
    struct RoomGame* game_list = &available_games[0];
    int index = get_game_idx_from_id(arg0);
    if (index == -1) {
        printf("Selected game room not found!\n");
        return -1;
    }

    // game_list[index].play(game_list[index].id);
    play_game(game_list[index].id);

    // printf("Game start!\n");
    // getchar();
    printf("Game end, press any key to return to game list page\n");
    getchar();

    return 0;
}

/**
 * Send Logout request to server
 */
int do_logout() {
    int ret;
    char payload[16] = "USR LOGOUT";
    char response[16] = "";

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    if (strcmp(response, "OK") == 0) {
        printf("Logged out!\n\n");
        return 0;
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
/*                             auxiliary functions                            */
/* -------------------------------------------------------------------------- */

/**
 * Get a valid game page command
 * need an arg0 argument for start command, with size >= 16...
 */
enum UserCommand get_game_page_command(char* arg0) {
    enum UserCommand command_id;
    int valid = 0;
    do {
        char command[16] = "";
        char arg[16] = "";
        char line[32] = "";

        printf(">> ");
        get_input_line(line, sizeof(line));
        sscanf(line, "%15s %15s", command, arg);

        if (strcmp(command, "") == 0) {
            continue;
        } else if (strcmp(command, "start") == 0) {
            if (strcmp(arg, "") == 0) {
                printf(
                    "No room selected\n"
                    "Please selected one by adding the room id in 'start <room>' command\n");
                continue;
            }
            strcpy(arg0, arg);
            command_id = COMMAND_START;
            valid = 1;
        } else if (strcmp(command, "logout") == 0) {
            command_id = COMMAND_LOGOUT;
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

// TODO remove this func
int get_game_idx_from_id(char* gameid) {
    int len = len_available_games;
    for (int i = 0; i < len; i++) {
        if (strcmp(available_games[i].id, gameid) == 0) {
            return i;
        }
    }
    return -1;
}
