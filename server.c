#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 4242

int main(int argc, char **argv) {
    unsigned int port = DEFAULT_PORT;

    if (argc > 1) {
        port = strtoul(argv[1], NULL, 10);
    } else {
        printf("No port provided, using default port %i \n", port);
    }

    printf("Server... %u\n", port);

    return 0;
}
