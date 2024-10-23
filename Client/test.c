#include <stdio.h>

#include "connection.h"

int main() {
    int s;
    s = Connection.get_conn_socket();
    printf("11 %d \n", s);

    int s0 = Connection.connect("", 1212);

    s = Connection.get_conn_socket();
    printf("22 %d %d\n", s, s0);

    printf("aaa");

    Connection.close();
    return 0;
}
