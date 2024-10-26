
#include <stdio.h>
#include <stdlib.h>

#include "other/connection.h"
#include "other/game_page.h"
#include "other/login_page.h"
#include "other/message.h"
#include "other/session.h"
#include "other/utility.h"

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 4242;

int main() {
    int ret;
    struct Session session = {
        .logged = 1,
        .close_client = 0,
    };

    ret = connection.connect(SERVER_IP, SERVER_PORT);
    if (ret == -1) {
        printf("Could not connect to server...\n");
        exit(-1);
    }

    do {
        if (!session.logged) {
            login_page(&session);
        }
        game_page(&session);
    } while (!session.close_client);

    connection.close();

    return 0;
}
