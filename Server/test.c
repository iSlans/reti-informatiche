#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/client_session.h"
#include "src/logging.h"

int main(int argc, char **argv) {
    char *command_str = malloc(128);
    char *type = malloc(128);
    char *args = malloc(128);
    sscanf("USR SIGNUP bb bb", "%15s %15s %256[^\\0]", command_str, type, args);

    logging_info("aaa");
    logging_info("aaa %s", "2");
    logging_info("aaa %s", "2");
    logging_error("aaa %s %i", "2", 2);
    logging_debug("aaa %s", "2");

    return 0;
}
