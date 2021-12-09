#include <iostream>
#include "options.hpp"

#include <thread>

#include "connection_listener.hpp"

void handle_incoming_requests(const ConnectionListener &listener);

/*
 * For now, you can test this with netcat:
 * 1. http_server /home -p 8080
 * 2. echo -n "my request string" | netcat -N localhost 8080
 */
int main(int argc, char **argv) {

    Options opt{argc, argv};

    if (!opt.port.has_value())
        opt.port = 8080;

    ConnectionListener listener = ConnectionListener{opt.port.value()};

    while (true) {
        handle_incoming_requests(listener);
    }

    return 0;
}

void handle_incoming_requests(const ConnectionListener &listener) {
    std::unique_ptr<Connection> cnn = listener.accept_next_connection(20);
    if (cnn) {
        //todo start a thread that handles the connection
        std::cout << "Client connected from '" << cnn->get_address()->str() << "', receiving data" << std::endl;
        std::cout << "Data: '" << cnn->receive_string() << "'" << std::endl;
    } else {
        std::cout << "Error while accepting client connection" << std::endl;
    }
}
