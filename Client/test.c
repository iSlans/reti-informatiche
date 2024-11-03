#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "other/connection.h"
#include "other/game_page.h"
#include "other/login_page.h"
#include "other/message.h"
#include "other/utility.h"

int logged = 0;
int close_client = 0;
int main() {
    connection.connect("127.0.0.1", 4242);

    int sd = connection.get_socket();

    char payload[128] = "OH YEAH";
    char response[128] = "";

    connection.request(payload, response, sizeof(response));

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
