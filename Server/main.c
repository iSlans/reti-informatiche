#include <unistd.h>

#include "src/fdcontroller.h"
#include "src/logging.h"
#include "src/server_handler.h"

int main() {
    struct ServerState session = {
        // .ip = NULL,
        .port = 0,
        .can_close_server = 0,
        .is_listening = 0,
        .listener_fd = -1,
        .n_clients = 0,
    };

    fd_controller.init();
    fd_controller.add(STDIN_FILENO);

    logging_info(
        "\n"
        "***************************** SERVER STARTED *********************************\n"
        "Digita un comando:\n"
        "1) start [port]    --> avvia il server di gioco sulla porta [port] o di default \n"
        "2) stop            --> termina il server\n"
        "\n"

    );

    do {
        fd_controller.update_ready_list();

        int maxfd = fd_controller.get_max_fd();
        for (int fd = 0; fd <= maxfd; fd++) {
            if (!fd_controller.is_ready(fd)) continue;

            if (fd == STDIN_FILENO) {
                handle_input(&session);

            } else if (fd == session.listener_fd) {
                handle_listener(&session);

            } else {
                handle_client_fd(fd, &session);
            }
        }
    } while (!session.can_close_server);

    return 0;
}
