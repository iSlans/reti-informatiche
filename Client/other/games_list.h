#ifndef GAMES_LIST_H
#define GAMES_LIST_H

#define NUMBER_GAMES 1

struct RoomGame {
    char id[16];
    char name[16];
    void (*play)();
    // unsigned int code;
    // ...
};

extern struct RoomGame availableGames[NUMBER_GAMES];

void game1();
// void game2();
// void game3();
// void game4();

#endif
