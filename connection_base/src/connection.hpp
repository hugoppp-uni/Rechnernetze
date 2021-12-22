#pragma once
#include <netdb.h>
#include <string>
#include <vector>
#include <memory>
#include "HttpResponse.hpp"
#include "address.hpp"
#include "HttpRequest.h"

class Connection{
public:
    explicit Connection(const std::shared_ptr<Address>& address);
    explicit Connection(const std::string& host);

    Connection(const std::shared_ptr<Address>& address, int file_descriptor);

    virtual ~Connection();

    [[nodiscard]] std::vector<char> receive_bytes() const;
    [[nodiscard]] std::string receive_string() const;
    [[nodiscard]] HttpResponse receive_http_response() const;
    void send(const std::string& message) const;
    void send(HttpResponse &response) const;
    void send(HttpResponse &response, Range range) const;
    void send_slow(const std::string& message, int n_bytes, int timeout_ms) const;

    std::shared_ptr<Address> get_address();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

private:
    int file_descriptor{};
    std::shared_ptr<Address> address;
};