#ifndef LOGGING_H
#define LOGGING_H
// #include <stdarg.h>

// void logging(const char* level, const char* msg, va_list args);
void logging_info(const char* msg, ...);
void logging_warn(const char* msg, ...);
void logging_error(const char* msg, ...);
void logging_debug(const char* msg, ...);

#endif
