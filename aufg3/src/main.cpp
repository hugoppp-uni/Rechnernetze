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

    std::unique_ptr<ClientConnection> cnn = listener.accept_next_connection(20);

    if (cnn) {
        std::cout << "Client connected from " << cnn->str() << ", receiving data" << std::endl;
        const std::vector<char> &vector = cnn->receive_bytes();
        std::cout << "Data: '"<< std::string(vector.begin(), vector.end()) << "'" << std::endl;
    } else {
        std::cout << "Error while accepting client connection" << std::endl;
    }

    return 0;

}
