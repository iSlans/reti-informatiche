#ifndef FDCONTROLLER_H
#define FDCONTROLLER_H

#include <sys/select.h>

/**
 * Controller to manage fd
 * by using FD macros
 */
struct FDController {
    void (*init)();                 // initialize necessary variables
    void (*add)(int fd);            // add fd into watchlist
    void (*remove)(int fd);         // remove fd from watchlist
    fd_set (*update_ready_list)();  // update ready fds list
    int (*is_ready)(int fd);        // check if fd is in ready list
    int (*get_max_fd)();            // get the max fd obtained until now
};

extern const struct FDController fd_controller;

// struct myfds {
// struct myfds* this;
// };

#endif
