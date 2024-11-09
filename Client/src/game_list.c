#include "games_list.h"

/**
 * List available games in client
 * <play> field redirect to the relative game execution
 * could make getting this info from an config file...
 */
struct RoomGame available_games[] = {
    {
        .id = "1",
        .name = "Elementals",
        .play = play_game,
        // .code = 1,
    },
    {
        .id = "2",
        .name = "test",
        .play = play_game,
        // .code = 1,
    },
};

unsigned int len_available_games = sizeof(available_games) / sizeof(available_games[0]);
