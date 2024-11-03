#ifndef CONNECTION_H
#define CONNECTION_H

struct Connection {
    /* data */
    int (*listen)(const char* ip, int port);  // bind and listen on given ip, port
    int (*accept)();                          // accept a new connection from listener
    int (*close_listener)();                  //
    int (*get_listener)();                    // get the listener fd
    int (*receive)(int fd, char* buffer);     // recv a message from fd
    int (*send)(int fd, char* payload);       // send a message to fd
    int (*close)(int fd);                     // close the fd connection
    const int default_port;
    const char* default_ip;
};

extern const struct Connection connection;

#endif
