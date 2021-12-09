#include <iostream>
#include "options.hpp"

#include <thread>

#include "connection_listener.hpp"

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

    std::unique_ptr<Connection> cnn = listener.accept_next_connection(20);

    if (cnn) {
        std::cout << "Client connected from '" << cnn->get_address()->str() << "', receiving data" << std::endl;
        std::cout << "Data: '"<< cnn->receive_string() << "'" << std::endl;
    } else {
        std::cout << "Error while accepting client connection" << std::endl;
    }

    return 0;

}
