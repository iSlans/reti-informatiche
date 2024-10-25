#ifndef UTILITY_H
#define UTILITY_H

enum UserCommand {
    COMMAND_LOGIN,
    COMMAND_SIGNUP,
    COMMAND_START,
    COMMAND_LOGOUT,
    COMMAND_END,
    COMMAND_HELP,
};

void get_input_line(char* buffer, unsigned int max_len);

#endif
