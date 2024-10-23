#include <stdio.h>

#include "lib/logging.h"

int main(int argc, char** argv) {
    logging_info("aaa");
    logging_info("aaa %s", "2");
    logging_info("aaa %s", "2");
    logging_error("aaa %s %i", "2", 2);
    logging_debug("aaa %s", "2");

    return 0;
}
