
#include <stdio.h>
#include <stdlib.h>

#include "src/admin_page.h"
#include "src/connection.h"
#include "src/game_page.h"
#include "src/login_page.h"
#include "src/session.h"
// #include "src/message.h"
// #include "src/utility.h"

// could try make a config file
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 4242;

int main() {
    int ret;
    struct Session session = {
        .logged = 0,
        .admin_mode = 0,
        .close_client = 0,
    };

    printf("Press any key to start and connect to server...\n");
    getchar();

    ret = connection.connect(SERVER_IP, SERVER_PORT);
    if (ret == -1) {
        printf(
            "Could not connect to server...\n"
            "Closing app...\n");
        exit(-1);
    }

    // client will loop until user close it with a command
    // Manage login first
    // if the user is an admin, show then the admin panel
    // otherwise just the normal game page

    do {
        if (!session.logged) {
            login_page(&session);
        }
        if (session.admin_mode) {
            admin_page(&session);
        } else {
            game_page(&session);
        }
    } while (!session.close_client);

    connection.close();

    return 0;
}
