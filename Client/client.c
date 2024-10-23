#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

const char* SERVER_IP = "127.0.0.1";
// const int SERVER_PORT = 4242;
const int SERVER_PORT = 1212;

int connect_server(const char* ip, int port);
void parse_input(char* buffer, unsigned int len);

int main() {
    int conn_socket;

    printf(
        "Client...\n"
        "\n");

    conn_socket = 1;
    // conn_socket = connect_server(SERVER_IP, SERVER_PORT);
    if (conn_socket == -1) {
        printf("Could not connect to server...\n");
        exit(-1);
    }

    /* ------------------------------ login signup ------------------------------ */

    printf(
        "Welcome to Escape Room Game!\n"
        "Please login to get started:\n"
        "1) login <username>        - to login with an already registered account\n"
        "2) signup                  - to register a new account\n"
        "3) end                    - to exit and close the game\n"
        /* */
    );

    do {
        printf(">> ");
        fflush(stdout);

        char buffer[64];  // = "logi nsssdddsss canoxnaaaannnnaaaann";
        char command[16];
        char arg[16];
        parse_input(buffer, sizeof(buffer));

        int len = 15;

        char fmt_str[64];
        snprintf(fmt_str, 64, "%%%ds %%%ds", len, len);
        sscanf(buffer, fmt_str, command, arg);
        command[15] = '\0';
        arg[15] = '\0';
        printf("%s %s \n", command, arg);

        if (strcmp(command, "") == 0) continue;
        if (strcmp(command, "login") == 0) {
            printf("%s", arg);
        } else if (strcmp(command, "signup") == 0) {
            printf("%s", arg);

            /* code */
        } else if (strcmp(command, "end") == 0) {
            exit(0);
        }

    } while (1);

    int ret;

    // ret = send(conn_socket, "login aa", 10, 0);
    // ret = recv(conn_socket, "", 1, 0);

    // login
    // signup
    // exit

    /* ---------------------------- game istructions ---------------------------- */
    printf("Game istructions ...\n\n");

    // logout
    //

    return 0;
}

int connect_server(const char* ip, int port) {
    int ret;
    int conn_socket;
    struct sockaddr_in server_addr;

    printf("Client...\n");

    conn_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    /*ret = bind(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)); */
    ret = connect(conn_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Connect error");
        return -1;
    }

    return conn_socket;
}

void parse_input(char* buffer, unsigned int len) {
    // char buffer[64];
    fgets(buffer, len, stdin);
    if (!strchr(buffer, '\n')) {
        scanf("%*[^\n]");
        scanf("%*c");
    }
    buffer[strcspn(buffer, "\n")] = '\0';

    return;

    // sscanf(buffer, "%*s %*s", c_len, command, a_len, arg);
    // command[c_len - 1] = '\0';
    // arg[a_len - 1] = '\0';
}
