
struct Connection {
    int (*get_conn_socket)();
    int (*connect)(const char* ip, int port);
    void (*close)();
};

extern const struct Connection connection;
