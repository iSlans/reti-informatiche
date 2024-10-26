#include "games_list.h"
//
#include "connection.h"
#include "message.h"
#include "utility.h"

static enum Game1Command get_command(char* arg0, char* arg1);

static enum Game1Command {
    GAME1_COMMAND_LOOK,
    GAME1_COMMAND_TAKE,
    GAME1_COMMAND_DROP,
    GAME1_COMMAND_USE,
    GAME1_COMMAND_OBJS,
    GAME1_COMMAND_HELP,
    GAME1_COMMAND_END,
};

void game1() {
    int ret;
    int can_exit = 0;
    char msg_commands_list =
        "Command list:\n"
        ">> look [location | object]    - get a description of room or selected location/object  \n"
        ">> take <object>               - collect an object\n"
        ">> drop <object>               - drop an colleted object\n"
        ">> use <object1> [object2]     - use an object or use it on another object\n"
        ">> objs                        - show the objects you collected\n"
        ">> end                         - exit from this game\n"
        ">> help                        - show this message again\n";

    printf("%s\n", msg_commands_list);

    do {
        char arg[16];
        enum Game1Command command = get_game_page_command(arg);

        switch (command) {
                // case GAME1_COMMAND_START:
                //     ret = do_start(arg);
                //     if (ret == -1) {
                //         continue;
                //     }
                //     can_exit = 1;
                //     break;

                // case GAME1_COMMAND_LOGOUT:
                //     ret = do_logout();
                //     if (ret == -1) {
                //         continue;
                //     }
                //     session->logged = 0;
                //     can_exit = 1;
                //     printf(
                //         "Logged out\n"
                //         "Returning to Login page...\n\n");
                //     // sleep(1);
                //     break;

            case GAME1_COMMAND_END:
                can_exit = 1;
                break;

            case GAME1_COMMAND_HELP:
                printf("\n%s\n", msg_commands_list);
                break;

            default:;
        }

    } while (!can_exit);

    return;
}

static enum Game1Command get_command(char* arg0, char* arg1) {
    enum Game1Command command_id;
    int valid = 0;
    char command[16] = "";
    char argument0[32] = "";
    char argument1[32] = "";
    char line[128] = "";
    do {
        printf(">> ");
        get_input_line(line, sizeof(line));
        sscanf(line, "%15s %31s %31s", command, argument0, argument1);

        if (strcmp(command, "") == 0) {
            continue;

        } else if (strcmp(command, "look") == 0) {
            strcpy(arg0, argument0);
            strcpy(arg1, argument1);
            command_id = GAME1_COMMAND_LOOK;
            valid = 1;

        } else if (strcmp(command, "take") == 0) {
            if (strcmp(argument0, "") == 0) {
                printf("Missing <object>, select which object to take, e.g. 'take libro'\n ");
                continue;
            }
            command_id = GAME1_COMMAND_TAKE;
            valid = 1;

        } else if (strcmp(command, "use") == 0) {
            if (strcmp(argument0, "") == 0) {
                printf("Missing <object1>, select which object to use, e.g. 'use libro' or 'use chiave lucchetto'\n ");
                continue;
            }
            command_id = GAME1_COMMAND_USE;
            valid = 1;

        } else if (strcmp(command, "objs") == 0) {
            command_id = GAME1_COMMAND_OBJS;
            valid = 1;

        } else if (strcmp(command, "end") == 0) {
            command_id = GAME1_COMMAND_END;
            valid = 1;

        } else if (strcmp(command, "help") == 0) {
            command_id = GAME1_COMMAND_HELP;
            valid = 1;
        }

        if (!valid) printf("Invalid command \n");
    } while (!valid);

    return command_id;
}
