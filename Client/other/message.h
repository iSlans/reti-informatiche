struct Message {
    const char* welcome;
    const char* login_page;
    const char* game_list_page;
};

struct Message message = {

    .login_page =
        "Welcome to Escape Room Game!\n"
        "Please login to get started:\n"
        ">> login               - to login with an already registered account\n"
        ">> signup              - to register a new account\n"
        ">> end                 - to exit and close the game\n"

    ,
    .game_list_page =
        "Following games are available:\n"
        "1. Elementals\n"
        "\n"
        "commands:\n"
        ">> start <room>       - to start a game with id <room> \n"
        ">> logout             - to logout and return to login page\n"
        ">> end                - to exit and close the game\n"

    ,
};
