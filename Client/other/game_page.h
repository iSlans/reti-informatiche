#ifndef GAME_PAGE_H
#define GAME_PAGE_H

#include "session.h"
#include "utility.h"

void game_page(struct Session* session);

enum UserCommand get_game_page_command(char* arg0);
int get_game_code_from_id(char* gameid);
int do_start();
int do_logout();
#endif
