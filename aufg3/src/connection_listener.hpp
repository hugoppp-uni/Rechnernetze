#pragma once

#include "client_connection.hpp"

#include <netdb.h>
#include <optional>
#include <memory>

class ConnectionListener {
public:
    explicit ConnectionListener(int port);

    virtual ~ConnectionListener();

    [[nodiscard]] std::unique_ptr<ClientConnection> accept_next_connection(int backlog) const;

private:

    explicit ConnectionListener(sockaddr_in server_address, int port);

    int file_descriptor{};
    int port;

    static sockaddr_in get_server_address(int port);
};



