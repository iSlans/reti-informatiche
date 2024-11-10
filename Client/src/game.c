/**
 * Escape room Game core part
 *
 * After initial game setup on both client and server:
 * 1. get a valid command from user
 * 2. sends the command to server and get the response
 * 3. the game state updates according to the response
 * 4. check win and exit or loop back
 */

#include "games_list.h"
//
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "connection.h"
#include "message.h"
#include "utility.h"

struct GameSession {
    char* game_id;
    int status;
    int phase;

    time_t end_time;
    // unsigned int bag_quantity;
    // unsigned int bag_size;

    // char token_list[256];
    unsigned int token_earned;
    unsigned int token_left;
};

enum GameCommand {
    GAME_COMMAND_LOOK,
    GAME_COMMAND_TAKE,
    GAME_COMMAND_DROP,
    GAME_COMMAND_USE,
    GAME_COMMAND_OBJS,
    GAME_COMMAND_HELP,
    GAME_COMMAND_END,
};

enum GameStatus {
    GAME_STATUS_NOT_STARTED = 0,
    GAME_STATUS_PLAYING = 100,
    GAME_STATUS_WIN = 200,
    GAME_STATUS_LOSE = 300,
    GAME_STATUS_TIMEOUT = 301,
};

static enum GameCommand get_command(char* arg0, char* arg1);
static int do_init_game(struct GameSession* session);
static int do_get_game_status(struct GameSession* session);

/**
 * Here all the functions to manage every game command
 */
static int do_look(struct GameSession* session, char* arg0);
static int do_take(struct GameSession* session, char* arg0);
static int do_drop(struct GameSession* session, char* arg0);
static int do_use(struct GameSession* session, char* arg0, char* arg1);
static int do_objs(struct GameSession* session);

/**
 * The main game entry point
 */
void play_game(char* game_id) {
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

    struct GameSession game_session = {
        .game_id = game_id,
        .status = 0,
    };

    /* -------------------------------------------------------------------------- */
    /*                           INITIAL GAME DATA SETUP                          */
    /* -------------------------------------------------------------------------- */

    // server init game, set up time, game data
    ret = do_init_game(&game_session);
    if (ret == -1) {
        printf("Could not start the game, returning to game list page...\n");
        return;
    }

    if (game_session.status != GAME_STATUS_PLAYING) {
        printf("Game status not correctly set, returning to game list page...\n");
        return;
    }

    printf("\n%s\n", msg_command_list);

    do {
        /* -------------------------------------------------------------------------- */
        /*                       GET USER INPUT AND SEND REQUEST                      */
        /* -------------------------------------------------------------------------- */
        int print_state = 1;

        char arg0[32];
        char arg1[32];
        enum GameCommand command = get_command(arg0, arg1);

        switch (command) {
            case GAME_COMMAND_LOOK:
                ret = do_look(&game_session, arg0);
                if (ret == -1) goto unhandled_error;
                break;

            case GAME_COMMAND_TAKE:
                ret = do_take(&game_session, arg0);
                if (ret == -1) goto unhandled_error;
                break;

            case GAME_COMMAND_DROP:
                ret = do_drop(&game_session, arg0);
                if (ret == -1) goto unhandled_error;
                break;

            case GAME_COMMAND_USE:
                ret = do_use(&game_session, arg0, arg1);
                if (ret == -1) goto unhandled_error;
                break;

            case GAME_COMMAND_OBJS:
                ret = do_objs(&game_session);
                if (ret == -1) goto unhandled_error;
                break;

            case GAME_COMMAND_END:
                can_exit = 1;
                // print_state = 0;
                break;

            case GAME_COMMAND_HELP:
                // print_state = 0;
                printf("\n%s\n", msg_command_list);
                break;

            default:;
        }

        /* -------------------------------------------------------------------------- */
        /*                              UPDATE GAME STATE                             */
        /* -------------------------------------------------------------------------- */

        ret = do_get_game_status(&game_session);
        if (ret == -1) {
            printf("Could not fetch game status, returning to game list page...\n");
            return;
        }

        /* -------------------------------------------------------------------------- */
        /*                                  CHECK WIN                                 */
        /* -------------------------------------------------------------------------- */

        switch (game_session.status) {
            case GAME_STATUS_WIN:
                printf("You Won!\n");
                can_exit = 1;
                break;

            case GAME_STATUS_LOSE:
                printf("You Lost :c\n");
                can_exit = 1;
                break;

            case GAME_STATUS_TIMEOUT:
                printf("Game Timeout, you lost :c\n");
                can_exit = 1;
                break;
            default:
                break;
        }

        // print some data as required
        if (print_state) {
            time_t diff = game_session.end_time - time(NULL);
            if (diff < 0) diff = 0;
            struct tm* time_left;
            time_left = gmtime(&diff);
            // printf("%lu\n%s", game_session.end_time, asctime(time_left));

            printf(
                "\n"
                "Token Collected: %u\t"
                "Remaining: %u\t"
                "Time left: %02d:%02d:%02d\n\n",
                game_session.token_earned,
                game_session.token_left,
                time_left->tm_hour,
                time_left->tm_min,
                time_left->tm_sec);
        }

        // unhandled_error:
    } while (!can_exit);
    return;

unhandled_error:
    // If any unexpected error, just exit from the game
    printf("Error unhandled error, ending game... \n");
}

/**
 * Ask user input until getting a valid command
 * command parameters stored in arg0 arg1
 * no checks performed on parameters
 *
 * TODO: could generalize with other get_command functions
 */
static enum GameCommand get_command(char* arg0, char* arg1) {
    enum GameCommand command_id;
    int valid = 0;

    struct CommandDict {
        char* input_str;
        enum GameCommand command_id;
    };

    static struct CommandDict command_list[] = {
        {.input_str = "look", .command_id = GAME_COMMAND_LOOK},
        {.input_str = "take", .command_id = GAME_COMMAND_TAKE},
        {.input_str = "drop", .command_id = GAME_COMMAND_DROP},
        {.input_str = "use", .command_id = GAME_COMMAND_USE},
        {.input_str = "objs", .command_id = GAME_COMMAND_OBJS},
        {.input_str = "end", .command_id = GAME_COMMAND_END},
        {.input_str = "help", .command_id = GAME_COMMAND_HELP},
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

/**
 * Manage the initial game setup
 */
static int do_init_game(struct GameSession* session) {
    int ret;
    char payload[128] = "";
    char response[128] = "";

    sprintf(payload, "GMN init %s", session->game_id);
    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    // printf("%s\n", response);
    if (strncmp(response, "OK", 2) != 0) {
        return -1;
    }

    // Parse the server response
    struct ServerResponse {
        int status;
        int phase;
        unsigned long end_time;
        unsigned int token_earned;
        unsigned int token_total;
    };
    struct ServerResponse resp;

    sscanf(response,
           "OK "
           "status=%d"
           "phase=%d"
           "end=%lu"
           "token=%d"
           "token_total=%d",
           &resp.status,
           &resp.phase,
           &resp.end_time,
           &resp.token_earned,
           &resp.token_total);

    session->status = resp.status;
    session->phase = resp.phase;
    session->end_time = resp.end_time;
    session->token_earned = resp.token_earned;
    session->token_left = resp.token_total - resp.token_earned;

    return 0;
}

static int do_look(struct GameSession* session, char* arg0) {
    int ret;
    char payload[128] = "";
    char response[512] = "";
    sprintf(payload, "GMN look %s", arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    if (strncmp(response, "OK", 2) != 0) {
        return -1;
    }

    // printf("%s\n", response);

    struct ServerResponse {
        char message[512];
    };
    struct ServerResponse resp;

    sscanf(response, "OK %511[^\\0]", resp.message);

    printf("%s\n", resp.message);

    return 0;
}

static int do_take(struct GameSession* session, char* arg0) {
    if (strcmp(arg0, "") == 0) {
        printf("Missing <object>, select which object to take, e.g. 'take libro'\n");
        return 0;
    }

    int ret;
    char payload[512] = "";
    char response[512] = "";
    sprintf(payload, "GMN take %s", arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    // printf("%s\n", response);

    if (strncmp(response, "OK", 2) != 0) {
        return -1;
    }

    struct ServerResponse {
        char message[1024];
        // char enigma[512];
        // char solution[512];
    };
    struct ServerResponse resp;

    // Manage enigma
    if (strncmp(response, "OK ENIGMA", 9) == 0) {
        sscanf(response, "OK ENIGMA %1023[^\\0]", resp.message);
        printf("%s\n", resp.message);

        // Get and send solution here
        char solution[32] = "";
        get_input_line(solution, sizeof solution);

        sprintf(payload, "GMN take %s SOLUTION %s", arg0, solution);
        ret = connection.request(payload, response, sizeof(response));

        if (strncmp(response, "OK", 2) != 0) {
            return -1;
        }
    }
    sscanf(response, "OK %511[^\\0]", resp.message);
    printf("%s\n", resp.message);

    return 0;
}

static int do_drop(struct GameSession* session, char* arg0) {
    if (strcmp(arg0, "") == 0) {
        printf("Missing <object>, select which object to drop, e.g. 'drop libro'\n");
        return 0;
    }
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN drop %s", arg0);

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    // printf("%s\n", response);

    struct ServerResponse {
        char message[512];
    };
    struct ServerResponse resp;

    sscanf(response, "OK %511[^\\0]", resp.message);
    printf("%s\n", resp.message);

    return 0;
}

static int do_use(struct GameSession* session, char* arg0, char* arg1) {
    if (strcmp(arg0, "") == 0) {
        printf("Missing <object>, select which object to use, e.g. 'use libro'\n");
        return 0;
    }
    int ret;
    char payload[512] = "";
    char response[512] = "";
    sprintf(payload, "GMN use %s %s", arg0, arg1);

    if (strcmp(arg1, "") == 0) {
        sprintf(payload, "GMN use %s", arg0);
    }

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    // printf("%s\n", response);

    struct ServerResponse {
        char message[512];
    };
    struct ServerResponse resp;

    sscanf(response, "OK %511[^\\0]", resp.message);
    printf("%s\n", resp.message);

    // update session

    return 0;
}

static int do_objs(struct GameSession* session) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN objs");

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }
    printf("%s\n", response);
    return 0;
}

static int do_get_game_status(struct GameSession* session) {
    int ret;
    char payload[128] = "";
    char response[128] = "";
    sprintf(payload, "GMN status");

    ret = connection.request(payload, response, sizeof(response));
    if (ret == -1) {
        printf("Error on reaching the server, please retry...\n");
        return -1;
    }

    struct ServerResponse {
        int status;
        int phase;
        unsigned long end_time;
        unsigned int token_earned;
        unsigned int token_total;
    };
    struct ServerResponse resp;

    sscanf(response,
           "OK "
           "status=%d"
           "phase=%d"
           "end=%lu"
           "token=%d"
           "token_total=%d",
           &resp.status,
           &resp.phase,
           &resp.end_time,
           &resp.token_earned,
           &resp.token_total);

    session->status = resp.status;
    session->phase = resp.phase;
    session->end_time = resp.end_time;
    session->token_earned = resp.token_earned;
    session->token_left = resp.token_total - resp.token_earned;

    return 0;
}
