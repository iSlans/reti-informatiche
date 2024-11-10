#ifndef SESSION_H
#define SESSION_H

struct Session {
    int logged;
    int admin_mode;
    int close_client;  // would be more appropriate as an app state instead of session state
    // char* token;    // could implement session token
};

enum UserCommand {
    COMMAND_LOGIN,
    COMMAND_SIGNUP,
    COMMAND_START,
    COMMAND_LOGOUT,
    COMMAND_END,
    COMMAND_HELP,
    COMMAND_ADMIN
};

#endif
