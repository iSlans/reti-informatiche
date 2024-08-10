#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Custom logger
 */

/* 'static' to limit function scope */
void logging(const char* level, const char* msg, va_list args) {
    char buffer[30];

    time_t now;
    time(&now);
    strftime(buffer, sizeof buffer, "%H:%M:%S", localtime(&now));

    printf("%s [%s]: ", buffer, level);
    vprintf(msg, args);
    printf("\n");
}

void logging_info(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging("INFO", msg, args);
    va_end(args);
}

void logging_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging("ERROR", msg, args);
    va_end(args);
}

void logging_debug(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging("DEBUG", msg, args);
    va_end(args);
}
