#include "utility.h"

#include <stdio.h>
#include <string.h>

/**
 * read and return entire line until buffer_len or CR
 */
void get_input_line(char* buffer, unsigned int buffer_len) {
    fgets(buffer, buffer_len, stdin);  // fgets reads n-1
    if (!strchr(buffer, '\n')) {       // flush remainings in stdin
        scanf("%*[^\n]");
        scanf("%*c");
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}
