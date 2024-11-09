#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
// #include <bits/types/FILE.h>

void get_input_line(char* buffer, unsigned int max_len);
void get_line_from(FILE* stream, char* buffer, unsigned int buffer_len);

int parse_port(char* port_str);

#endif
