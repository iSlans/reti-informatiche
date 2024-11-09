#include "utility.h"

#include <stdio.h>
#include <string.h>

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
