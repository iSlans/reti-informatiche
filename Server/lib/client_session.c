#include "client_session.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

typedef struct FDClientDict {
    int fd;
    struct ClientState* client;
    LIST_ENTRY(FDClientDict)
    entries;
} FDClientDict;

LIST_HEAD(ListHead, FDClientDict);
struct ListHead head;

static void my_list_init() {
    LIST_INIT(&head);
}

static struct ClientState* my_list_insert(int fd) {
    FDClientDict* elem = malloc(sizeof(FDClientDict));
    elem->client = malloc(sizeof(struct ClientState));
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
