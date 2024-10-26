#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    const char* welcome;
    const char* login_page;
    const char* game_list_page;
};

extern struct Message message;

#endif
