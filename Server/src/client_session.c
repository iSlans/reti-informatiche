#include "client_session.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#define MAX_SERIALIZED_LEN 1024

char* serialize_game_data(struct GameData* game) {
    char* data_string = calloc(MAX_SERIALIZED_LEN, sizeof(char));

    snprintf(data_string, MAX_SERIALIZED_LEN,
             //  "OK "
             "status=%d"
             "phase=%d"
             "end=%lu"
             "token=%d"
             "token_total=%d",
             game->status,
             game->phase,
             game->end_time,
             game->token_earned,
             game->token_total  //
    );

    // NEED TO DO free FROM CALLER
    return data_string;
}

/* -------------------------------------------------------------------------- */
/*                                 CLIENT LIST                                */
/* -------------------------------------------------------------------------- */

/**
 * Struct to associate the fd to ClientState
 * Using gcc queue.h to implement a list of this struct, so a list of <fd, clientState>
 */
typedef struct FDClientDict {
    int fd;
    struct ClientState* client;
    LIST_ENTRY(FDClientDict)
    entries;
} FDClientDict;

/**
 * Below all the methods to add, get, modify, delete elements in the list
 */

LIST_HEAD(ListHead, FDClientDict);
struct ListHead head;

static void my_list_init() {
    LIST_INIT(&head);
}

static struct ClientState* my_list_insert(int fd) {
    FDClientDict* elem = malloc(sizeof(FDClientDict));
    elem->client = malloc(sizeof(struct ClientState));
    memset(elem, 0, sizeof elem);
    elem->fd = fd;
    LIST_INSERT_HEAD(&head, elem, entries);
    return elem->client;
}

static void my_list_remove(int fd) {
    FDClientDict* elem;
    LIST_FOREACH(elem, &head, entries) {
        if (elem->fd == fd) {
            LIST_REMOVE(elem, entries);
            free(elem->client);
            free(elem);
            break;
        }
    }
}

static struct ClientState* my_list_find(int fd) {
    FDClientDict* elem;
    LIST_FOREACH(elem, &head, entries) {
        if (elem->fd == fd) {
            return elem->client;
            break;
        }
    }

    return NULL;
}

struct ClientList client_list = {
    .init = my_list_init,
    .add = my_list_insert,
    .remove = my_list_remove,
    .find = my_list_find,
};
