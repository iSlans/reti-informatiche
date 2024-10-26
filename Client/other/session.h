#ifndef SESSION_H
#define SESSION_H

struct Session {
    int logged;
    int close_client;
};

enum UserCommand {
    COMMAND_LOGIN,
    COMMAND_SIGNUP,
    COMMAND_START,
    COMMAND_LOGOUT,
    COMMAND_END,
    COMMAND_HELP
};

#endif
