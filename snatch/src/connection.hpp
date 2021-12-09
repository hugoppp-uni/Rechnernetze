#pragma once
#include <netdb.h>
#include <string>
#include <vector>
#include <memory>
#include "http_response.h"
#include "address.hpp"

class Connection{
public:
    explicit Connection(const std::shared_ptr<Address>& address);
    Connection(const std::shared_ptr<Address>& address, int file_descriptor);

    [[nodiscard]] std::vector<char> receive_bytes() const;
    [[nodiscard]] std::string receive_string() const;
    [[nodiscard]] HttpResponse receive_http_response() const;
    void send(const std::string& message) const;
    void send_slow(const std::string& message, int n_bytes, int timeout_ms) const;

    std::shared_ptr<Address> get_address();
    virtual ~Connection();

private:
    int file_descriptor{};
    std::shared_ptr<Address> address;
};