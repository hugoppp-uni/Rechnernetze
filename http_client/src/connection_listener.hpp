#pragma once

#include "../../snatch/src/connection.hpp"

#include <netdb.h>
#include <optional>
#include <memory>

class ConnectionListener {
public:
    explicit ConnectionListener(int port);

    virtual ~ConnectionListener();

    [[nodiscard]] std::unique_ptr<Connection> accept_next_connection(int backlog) const;

private:

    int file_descriptor{};
    int port;

    static sockaddr_in get_server_address(int port);
};



