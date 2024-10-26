#include "games_list.h"

/**
 * List available games in client
 * <play> field redirect to the relative game execution
 * could make getting this info from an config file...
 */
struct RoomGame availableGames[] = {
    {.id = "1", .name = "Elementals", .play = game1},
};
