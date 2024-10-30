#ifndef GAMES_LIST_H
#define GAMES_LIST_H

// #define NUMBER_GAMES 1

struct RoomGame {
    char id[16];              // client side identifier
    char name[16];            //
    void (*play)(char* arg);  // 'executable' to run the game
    // unsigned int code;     // server side identifier
    // ...
};

extern struct RoomGame available_games[];
extern unsigned int len_available_games;

void play_game_type_1(char* game_id);
// void play_game_type_2();
// void play_game_type_3();
// void play_game_type_4();

#endif
