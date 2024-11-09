#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "time.h"
struct GameData {
    int status;
    int phase;
    time_t end_time;

    int bag_size;
    int token_earned;
    int token_total;

    int* flags;
    int flags_len;
};

char* serialize_game_data(struct GameData*);

struct ClientState {
    int is_logged;
    int selected_game;
    struct GameData game_data;
};

struct ClientList {
    void (*init)();
    void (*remove)(int fd);
    struct ClientState* (*add)(int fd);
    struct ClientState* (*find)(int fd);
};

extern struct ClientList client_list;

#endif
