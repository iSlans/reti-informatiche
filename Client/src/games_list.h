#ifndef GAMES_LIST_H
#define GAMES_LIST_H

// #define NUMBER_GAMES 1

struct RoomGame {
    char id[16];    // client side identifier
    char name[16];  //
    // void (*play)(char* arg);  // 'executable' to run the game TODO remove this
    // unsigned int code;     // server side identifier
    // ...
};

extern struct RoomGame available_games[];
extern unsigned int len_available_games;

void play_game(char* game_id);

#endif
