#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "other/connection.h"
#include "other/game_page.h"
#include "other/login_page.h"
#include "other/message.h"
#include "other/utility.h"
#include "time.h"

int logged = 0;
int close_client = 0;
int main() {
    time_t diff = -10;

    // if (diff < 0) diff = 0;

    struct tm* ptm;
    ptm = gmtime(&diff);
    printf("%dmin %dsec", ptm->tm_min, ptm->tm_sec);

    printf(asctime(ptm));

    return 0;
    connection.connect("127.0.0.1", 4242);

    int sd = connection.get_socket();

    char payload[1024] = "";
    char response[1024] = "";

    while (1) {
        get_input_line(payload, sizeof payload);
        connection.request(payload, response, sizeof(response));
        printf("%s\n", response);
    }

    return 0;
}
void aa() {
    {
        // test static scope
        // extern get_conn_socket();
        // int s = get_conn_socket();
        // extern int conn_is_open;
        // int s = connection.get_socket();
        // conn_is_open = 1;
        // s = connection.get_socket();
    }
    // ------------

    // get_game_page_command();
    do_login();
    return;
    {
        char sss2[10] = "abcde";
        sprintf(sss2, "%s %s", "987", "6");

        char sss[6] = "abcde";
        printf("string %s \n", sss);
        fflush(stdout);
    }
    return;

    do_login();
    return;

    get_login_page_command();
    return;
    /*---------------------------------------*/
    {
        int s;
        s = connection.get_socket();
        printf("11 %d \n", s);

        int s0 = connection.connect("", 1212);

        s = connection.get_socket();
        printf("22 %d %d\n", s, s0);

        printf("aaa");

        connection.close();
    }
    return;
}
