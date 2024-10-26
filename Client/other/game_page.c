#include "game_page.h"

#include <stdio.h>
#include <string.h>
// #include <unistd.h> sleep

#include "connection.h"
#include "message.h"
#include "utility.h"

struct RoomGame {
    char id[16];
    char name[16];
    unsigned int code;
    // ...
};

struct RoomGame availableGames[] = {
    {.id = "1", .name = "Elementals", .code = 1},
};

/* ------------------------- show games and commands ------------------------ */
void game_page(struct Session* session) {
    int ret;
    int can_exit = 0;

    if (!session->logged) return;
    if (session->close_client) return;

    printf("%s\n", message.game_list_page);

    do {
        char arg[16];
        enum UserCommand command = get_game_page_command(arg);

        switch (command) {
            case COMMAND_START:
                /*
                 start server game session, get game end time
                */
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
 * Get a valid game page command
 * arg0 size >= 16...
 */
enum UserCommand get_game_page_command(char* arg0) {
    enum UserCommand command_id;
    int valid = 0;
    do {
        char command[16] = "";
        char arg[16] = "";
        char line[32] = "";

        printf(">> ");
        get_input_line(line, 32);
        sscanf(line, "%15s %15s", command, arg);

        if (strcmp(command, "") == 0) {
            continue;
        } else if (strcmp(command, "start") == 0) {
            if (strcmp(arg, "") == 0) {
                printf("No room selected\n");
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

int get_game_code_from_id(char* gameid) {
    int len = sizeof(availableGames) / sizeof(struct RoomGame);
    for (int i = 0; i < len; i++) {
        if (strcmp(availableGames[i].id, gameid) == 0) {
            return availableGames[i].code;
        }
    }
    return -1;
}

int do_start() {
    get_game_code_from_id("");
    printf("Game start!\n");
    getchar();

    printf("Game end, press any key to return to game list page\n");
    getchar();

    return 0;
}
int do_logout() {
    printf("Logged out\n");
    return 0;
}
