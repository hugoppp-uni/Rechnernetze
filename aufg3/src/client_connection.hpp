#pragma once

#include <unistd.h>
#include <netdb.h>
#include <string>
#include <vector>

class ClientConnection {
public:
    ClientConnection(sockaddr sockaddr, socklen_t socklen, int file_descriptor);

    virtual ~ClientConnection();

    std::string str();
    [[nodiscard]] std::vector<char> receive_bytes() const;

private:
    struct sockaddr socket_address{};
    socklen_t socket_address_length{};
    int file_descriptor{};

};



