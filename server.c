#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "logging.h"

// #define DEFAULT_PORT 4242
// #define DEFAULT_IP "127.0.0.1"
const char* DEFAULT_IP = "127.0.0.1";
const int DEFAULT_PORT = 4242;
const int DEFAULT_BACKLOG_SIZE = 10;

const char* COMMAND_START = "start";
const char* COMMAND_STOP = "stop";

int parse_port(unsigned int* port, char* str);
void start_server(int port);
void stop_server();

unsigned int parse_uint(char* str);
int activate_listener_socket(const char* ip, unsigned int port, int backlog);

/* -------------------------------------------------------------------------- */
/*                                    MAIN                                    */
/* -------------------------------------------------------------------------- */
int main(int argc, char* argv[]) {
    char ip[32];
    unsigned int def_port = DEFAULT_PORT;
    int backlog = DEFAULT_BACKLOG_SIZE;

    int is_server_running = 0;

    int maxfd;
    fd_set master_fds;
    fd_set ready_fds;
    FD_ZERO(&master_fds);
    FD_ZERO(&ready_fds);

    int listener_socket;

    strcpy(ip, DEFAULT_IP);

    /* ------------------------------- Parse Args ------------------------------- */
    if (argc > 1) {
        int ret = parse_port(&def_port, argv[1]);
        if (ret == -1) exit(-1);
        logging_info("Default port provided: %u", def_port);
    } else {
        logging_info("No port provided, may use default port %u", def_port);
    }

    // printf("Server... %u\n", def_port);
    /* ---------------------------------- Start --------------------------------- */

    printf(
        "***************************** SERVER STARTED *********************************\n"
        "Digita un comando:\n"
        "1) start [port]    --> avvia il server di gioco sulla porta data o di default \n"
        "2) stop            --> termina il server\n"
        "\n"
        // "******************************************************************************\n"
        /**/
    );

    printf(">> ");
    fflush(stdout);

    FD_SET(STDIN_FILENO, &master_fds);

    while (1) {
        int fdidx;
        maxfd = 3;
        ready_fds = master_fds;
        select(maxfd + 1, &ready_fds, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &ready_fds)) {
            char command[6] = "";
            char arg[6] = "";
            char buffer[64];
            // void get_command(&buffer, &command, &arg);
            {
                fgets(buffer, sizeof(buffer), stdin);
                if (!strchr(buffer, '\n')) {
                    scanf("%*[^\n]");
                    scanf("%*c");
                }
                buffer[strcspn(buffer, "\n")] = '\0';

                sscanf(buffer, "%5s %5s", command, arg);
                command[5] = '\0';
                arg[5] = '\0';
            }

            if (strcmp(command, "") == 0) goto next_input;

            if (strcmp(command, COMMAND_START) == 0) {
                unsigned int port = def_port;
                int sd;
                if (is_server_running) {
                    logging_info("Server is already running, stop it to restart again");
                    // goto next_input;
                }

                if (strcmp(arg, "") != 0) {
                    int ret = parse_port(&port, arg);
                    if (ret == -1) goto next_input;
                }

                logging_info("Starting server on %s:%u", ip, port);
                sd = activate_listener_socket(ip, port, backlog);
                if (sd == -1) {
                    logging_error("Could not start listener socket");
                    goto next_input;
                }
                FD_SET(sd, &master_fds);
                is_server_running = 1;
                listener_socket = sd;
                logging_info("Server started");

            } else if (strcmp(command, COMMAND_STOP) == 0) {
                if (!is_server_running) {
                    logging_info("Server is not running");
                    goto next_input;
                }
                //
                // if(active_user > 0) goto next_input;

                close(listener_socket);

                logging_info("Server stopped");
            } else {
                logging_error("Invalid command: %s", buffer);
            }

        next_input:
            printf(">> ");
            fflush(stdout);
        }
    }

    return 0;
}

int parse_port(unsigned int* port, char* str) {
    unsigned int p = parse_uint(str);
    // parse error or over max port number
    if (p <= 0 || p > 65535) {
        logging_error("Invalid port string %s", str);
        // exit(-1);
        return -1;
    }
    *port = p;
}

/* parse unsigned number from string, returns -1 if error */
unsigned int parse_uint(char* str) {
    unsigned int number;

    int ret = sscanf(str, "%u", &number);
    if (ret == 0) {
        return -1;
    }

    return number;

    // char* pos;
    // errno = 0;
    // number = strtoul(str, pos, 10);
    // number = strtoul("\n", NULL, 10);
    // number = strtoul("-111\n", NULL, 10);
    // number = strtoul("40000000000000000000000000000000000000000000000", NULL, 10);
    // if (errno != 0) {
    //     perror("Parse error");
    //     return -1;
    // }
}

int activate_listener_socket(const char* ip, unsigned int port, int backlog) {
    static struct sockaddr_in server_addr;
    // static struct sockaddr_in client_addr;
    static int listener_socket;
    int ret;

    listener_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    ret = bind(listener_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Bind error");
        return -1;
    }

    ret = listen(listener_socket, backlog);
    if (ret == -1) {
        // if (errno != 0) {
        perror("Listen error");
        return -1;
    }

    return listener_socket;
}
