#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                                CUSTOM LOGGER                               */
/* -------------------------------------------------------------------------- */

// #define TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIME_FORMAT "%H:%M:%S"

#define RED(str) "\x1b[31m" str "\x1b[0m"
#define GREEN(str) "\x1b[32m" str "\x1b[0m"
#define YELLOW(str) "\x1b[33m" str "\x1b[0m"
#define BLUE(str) "\x1b[34m" str "\x1b[0m"
#define MAGENTA(str) "\x1b[35m" str "\x1b[0m"
#define CYAN(str) "\x1b[36m" str "\x1b[0m"
#define GRAY(str) "\x1b[90m" str "\x1b[0m"

/**
 * Custom logger
 * using ANSI codes to get colored prints
 */

void logging(const char* level, const char* msg, va_list args) {
    char timestamp[30];
    time_t now;
    time(&now);
    strftime(timestamp, sizeof timestamp, TIME_FORMAT, localtime(&now));

    printf(GRAY("%s") " [%s]: ", timestamp, level);
    vprintf(msg, args);
    printf("\n");
}

void logging_info(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging(CYAN("INFO"), msg, args);
    va_end(args);
}

void logging_warn(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging(YELLOW("WARNING"), msg, args);
    va_end(args);
}

void logging_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging(RED("ERROR"), msg, args);
    va_end(args);
}

void logging_debug(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    logging(GREEN("DEBUG"), msg, args);
    va_end(args);
}
