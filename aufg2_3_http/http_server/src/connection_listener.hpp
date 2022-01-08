#pragma once

#include "connection.hpp"

#include <netdb.h>
#include <optional>
#include <memory>

class ConnectionListener {
public:
    explicit ConnectionListener(int port, int backlog = 20);

    virtual ~ConnectionListener();

    void shutdown() const;

    [[nodiscard]] std::unique_ptr<Connection> accept_next_connection() const;

    ConnectionListener(const ConnectionListener &) = delete;
    ConnectionListener &operator=(const ConnectionListener &) = delete;

private:

    int file_descriptor{};
    int port;
};


class ListenerClosedException: public std::exception
{
    [[nodiscard]] const char* what() const noexcept override
    {
        return "ConnectionListener has been closed";
    }
};
