#include "fdcontroller.h"

#include <stddef.h>
#include <sys/select.h>

struct FDControllerData {
    fd_set fd_list;
    fd_set ready_fds;
    int maxfd;
};

static struct FDControllerData fd_data;

static void fd_list_init() {
    fd_data.maxfd = 0;
    FD_ZERO(&fd_data.fd_list);
    FD_ZERO(&fd_data.ready_fds);
}

static fd_set fd_list_select() {
    // fd_set fds;
    // FD_ZERO(&fds);  // is this necessary?
    // fds = fd_data.fd_list;

    fd_data.ready_fds = fd_data.fd_list;

    int ret = select(fd_data.maxfd + 1, &fd_data.ready_fds, NULL, NULL, NULL);
    if (ret == -1) {
    }

    return fd_data.ready_fds;
}

static void fd_list_add(int fd) {
    FD_SET(fd, &fd_data.fd_list);
    if (fd > fd_data.maxfd) fd_data.maxfd = fd;
}

static void fd_list_remove(int fd) {
    FD_CLR(fd, &fd_data.fd_list);
}

static int fd_list_is_ready(int fd) {
    return FD_ISSET(fd, &fd_data.ready_fds);
}

static int fd_list_get_max_fd() {
    return fd_data.maxfd;
}

const struct FDController fd_controller = {
    .init = fd_list_init,
    .add = fd_list_add,
    .remove = fd_list_remove,
    .update_ready_list = fd_list_select,
    .is_ready = fd_list_is_ready,
    .get_max_fd = fd_list_get_max_fd,
};
