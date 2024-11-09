#include "utility.h"

#include <stdio.h>
#include <string.h>

// static unsigned int parse_uint(char* str);

/**
 * store in buffer an entire line from stdin until buffer_len or newline
 */
void get_input_line(char* buffer, unsigned int buffer_len) {
    fgets(buffer, buffer_len, stdin);  // doc says fgets reads n-1 and store \0 in the last
    if (!strchr(buffer, '\n')) {       // flush remainings in stdin
        scanf("%*[^\n]");
        scanf("%*c");
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}
/**
 * Parse a port number from str
 * Return -1 if any error or invalid port number
 */
int parse_port(char* str) {
    unsigned int p = 0;
    sscanf(str, "%u", &p);

    if (p <= 0 || p > 65535) {
        return -1;
    }
    return p;
}

/* parse unsigned number from string, returns -1 if error */
// static unsigned int parse_uint(char* str) {
//     unsigned int number;

//     int ret = sscanf(str, "%u", &number);
//     if (ret == 0) {
//         return -1;
//     }

//     return number;

//     // char* pos;
//     // errno = 0;
//     // number = strtoul(str, pos, 10);
//     // number = strtoul("\n", NULL, 10);
//     // number = strtoul("-111\n", NULL, 10);
//     // number = strtoul("40000000000000000000000000000000000000000000000", NULL, 10);
//     // if (errno != 0) {
//     //     perror("Parse error");
//     //     return -1;
//     // }
// }
