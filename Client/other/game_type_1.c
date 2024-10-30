/**
 * Project escape room game core part
 *
 * After initial game setup on both client and server,
 *
 * 1. the user need to input a valid command
 * 2. the client sends the command to server and get the response
 * 3. the game state updates according to the response
 *
 */

#include "games_list.h"
//
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "connection.h"
#include "message.h"
#include "utility.h"

struct Game1Session {
    char* game_id;
    int status;
    time_t end_time;
    unsigned int bag;
    unsigned int bag_size;
    unsigned int token_list;
    unsigned int token_number;
};

enum Game1Command {
    GAME1_COMMAND_LOOK,
    GAME1_COMMAND_TAKE,
    GAME1_COMMAND_DROP,
    GAME1_COMMAND_USE,
    GAME1_COMMAND_OBJS,
    GAME1_COMMAND_HELP,
    GAME1_COMMAND_END,
};

static enum Game1Command get_command(char* arg0, char* arg1);
static int do_init_game(struct Game1Session* session);
static int do_look(struct Game1Session* session, char* arg0);
static int do_take(struct Game1Session* session, char* arg0);
static int do_drop(struct Game1Session* session, char* arg0);
static int do_use(struct Game1Session* session, char* arg0, char* arg1);
static int do_objs(struct Game1Session* session);

/**
 * The main game method
 */
void play_game_type_1(char* game_id) {
    int ret;
    int can_exit = 0;

    static char msg_command_list[] =
        "Command list:\n"
        ">> look [location | object]    - get a description of room or selected location/object  \n"
        ">> take <object>               - collect an object\n"
        ">> drop <object>               - drop an collected object\n"
        ">> use <object1> [object2]     - use an object or use it on another object\n"
        ">> objs                        - show the objects you collected\n"
        ">> end                         - exit from this game\n"
        ">> help                        - show this message again\n";

    struct Game1Session game_session = {
        .game_id = game_id,
        .status = 0,
    };

    // server init game, set up time, game data
    ret = do_init_game(&game_session);
    if (ret == -1) {
        printf("Could not start the game, returning to game list page...\n");
        return;
    }

    printf("%s\n", msg_command_list);

    do {
        int print_state = 1;

        char arg0[32];
        char arg1[32];
        enum Game1Command command = get_command(arg0, arg1);

        switch (command) {
            case GAME1_COMMAND_LOOK:
                ret = do_look(&game_session, arg0);
                // if(ret == -1){}
                break;

            case GAME1_COMMAND_TAKE:
                if (strcmp(arg0, "") == 0) {
                    printf("Missing <object>, select which object to take, e.g. 'take libro'\n");
                    continue;
                }
                break;
            case GAME1_COMMAND_DROP:
                if (strcmp(arg0, "") == 0) {
                    printf("Missing <object>, select which object to drop, e.g. 'drop libro'\n");
                    continue;
                }
                break;
            case GAME1_COMMAND_USE:
                if (strcmp(arg0, "") == 0) {
                    printf("Missing <object>, select which object to drop, e.g. 'use libro'\n");
                    continue;
                }
                break;
            case GAME1_COMMAND_OBJS:
                break;

            case GAME1_COMMAND_END:
                can_exit = 1;
                print_state = 0;
                break;

            case GAME1_COMMAND_HELP:
                print_state = 0;
                printf("\n%s\n", msg_command_list);
                break;

            default:;
        }

        // check status win/lose

        if (print_state) {
            printf(
                "Token Collected: 1\t"
                "Remainant: 5\t"
                "Time left: 59min 10s\n"
                //
            );
        }

    } while (!can_exit);

    return;
}

/**
 * Ask user input until getting a valid command
 * command parameters stored in arg0 arg1
 * no checks perfomed on parameters
 *
 * TODO: could generalize with other get_command functions
 */
static enum Game1Command get_command(char* arg0, char* arg1) {
    enum Game1Command command_id;
    int valid = 0;

    struct CommandDict {
        char* input_str;
        enum Game1Command command_id;
    };

    static struct CommandDict command_list[] = {
        {.input_str = "look", .command_id = GAME1_COMMAND_LOOK},
        {.input_str = "take", .command_id = GAME1_COMMAND_TAKE},
        {.input_str = "drop", .command_id = GAME1_COMMAND_DROP},
        {.input_str = "use", .command_id = GAME1_COMMAND_USE},
        {.input_str = "objs", .command_id = GAME1_COMMAND_OBJS},
        {.input_str = "end", .command_id = GAME1_COMMAND_END},
        {.input_str = "help", .command_id = GAME1_COMMAND_HELP},
    };

    do {
        char command[16] = "";
        char argument0[32] = "";
        char argument1[32] = "";
        char line[128] = "";
        printf(">> ");
        get_input_line(line, sizeof(line));
        sscanf(line, "%15s %31s %31s", command, argument0, argument1);

        if (strcmp(command, "") == 0)
            continue;

        {
            // look for the command
            int len = sizeof(command_list) / sizeof(command_list[0]);
            for (int i = 0; i < len; i++) {
                if (strcmp(command, command_list[i].input_str) == 0) {
                    command_id = command_list[i].command_id;
                    valid = 1;
                    // could do strcpy arg01 here and return directly
                    break;
                }
            }
        }

        if (!valid)
            printf("Invalid command, type 'help' to show command list \n");
        else {
            strcpy(arg0, argument0);
            strcpy(arg1, argument1);
        }
    } while (!valid);

    return command_id;
}

static int do_init_game(struct Game1Session* session) {
    return -1;
}

static int do_look(struct Game1Session* session, char* arg0) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN ISTR %s look %s", session->game_id, arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    printf("%s\n", response);

    // update session

    return 0;
}

static int do_take(struct Game1Session* session, char* arg0) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN ISTR %s take %s", session->game_id, arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    printf("%s\n", response);

    // update session

    return 0;
}
static int do_drop(struct Game1Session* session, char* arg0) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN ISTR %s take %s", session->game_id, arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    printf("%s\n", response);

    // update session

    return 0;
}
static int do_use(struct Game1Session* session, char* arg0, char* arg1) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN ISTR %s take %s", session->game_id, arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    printf("%s\n", response);

    // update session

    return 0;
}
static int do_objs(struct Game1Session* session) {
    int ret;
    // char payload[128] = "";
    // char response[128] = "";
    // sprintf(payload, "GMN ISTR %s take %s", session->game_id, arg0);

    // ret = connection.request(payload, response, sizeof(response));
    // if (ret == -1) {
    //     printf("Error on reaching the server, please retry...\n");
    //     return -1;
    // }

    // printf("%s\n", response);

    session;

    // update session

    return 0;
}
