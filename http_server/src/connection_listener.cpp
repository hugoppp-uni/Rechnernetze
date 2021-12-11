#include "connection_listener.hpp"
#include "logger.hpp"

#include <sys/socket.h>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>


ConnectionListener::ConnectionListener(int port, int backlog) : port(port) {

    // When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces
    auto server_addr = Address::get_any_address(port);

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > file_descriptor) {
        std::cout << "Could not create socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (0 != bind(file_descriptor, server_addr->get_sockaddr(), server_addr->get_socklen())) {
        std::cout << "Could not bind socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (0 != ::listen(file_descriptor, backlog)) {
        std::cout << "Could not listen on socket: " << strerror(errno) << std::endl;
        exit(1);
    }

    Logger::warn("Listening on port " + std::to_string(port));
}

ConnectionListener::~ConnectionListener() {
    if (0 != ::close(file_descriptor)) {
        Logger::error("Could not close socket " + std::to_string(file_descriptor) + " :"
                      + std::string(strerror(errno)));
    }
}

std::unique_ptr<Connection> ConnectionListener::accept_next_connection() const {
    auto *peer_sockaddr = new struct sockaddr();

    socklen_t peer_socklen{};
    int peer_file_descriptor = ::accept(file_descriptor, peer_sockaddr, &peer_socklen);

    if (0 > peer_file_descriptor) {
        delete peer_sockaddr;
        if (errno == EINVAL) // Socket is not listening for connections, or addrlen is invalid (e.g., is negative).
            throw ListenerClosedException{};

        Logger::error("Could not accept connection: " + std::string(strerror(errno)));
        return nullptr;
    }

    //address dtor will handle deleting sockaddr here from here on
    return std::make_unique<Connection>(std::make_shared<Address>(peer_sockaddr, peer_socklen), peer_file_descriptor);
}

void ConnectionListener::close() const {
    //this is needed to cause EBADF error and exit from accept
    ::shutdown(file_descriptor, SHUT_RD);
    Logger::warn("No longer listening on port " + std::to_string(port));
}
