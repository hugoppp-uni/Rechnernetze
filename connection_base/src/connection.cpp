#include "connection.hpp"
#include "logger.hpp"
#include "HttpRequest.h"

#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

Connection::Connection(const std::shared_ptr<Address> &address, int file_desciptor) : address(address),
                                                                                      file_descriptor(file_desciptor) {

}

Connection::Connection(const std::string &host) : Connection(std::make_shared<Address>(host)) {}

Connection::Connection(const std::shared_ptr<Address> &address) : address(address) {

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (0 != connect(file_descriptor, address->get_sockaddr(), address->get_socklen())) {
        throw std::runtime_error(
            "Could not connect to " + address->str());
    }

//    std::cout << "available ip addresses:" << std::endl;
//    address->print_all_ip_addresses();

    Logger::info("using ip address: " + address->str());
}


Connection::~Connection() {
    if (0 != ::close(file_descriptor)) {
        Logger::error("Could not shutdown socket " + std::to_string(file_descriptor) + " :"
                      + strerror(errno));
    } else {
        Logger::info("Disconnected from '" + address->str() + "'");
    }
}

std::vector<char> Connection::receive_bytes() const {
    std::vector<char> result_buffer;
    result_buffer.reserve(16 * 1024);

    size_t size;
    std::array<char, 2 * 1024> recv_buffer{0};
    do {
        size = recv(file_descriptor, &recv_buffer[0], recv_buffer.size(), 0);
        result_buffer.insert(result_buffer.end(), recv_buffer.begin(), recv_buffer.begin() + size);
    } while (size > 0 && std::string{result_buffer.end() - 3, result_buffer.end()} == "\r\n\r\n");

    if (size < 0) {
        std::cerr << "error occurred during receive_bytes: " << strerror(errno) << std::endl;
    }

    return result_buffer;
}

void Connection::send(const std::string &message) const {
    const char *string = message.c_str();
    ::send(file_descriptor, string, message.length(), 0);
}

void Connection::send_slow(const std::string &message, int n_bytes, int timeout_ms) const {
    Logger::info("Sending message in slow-motion mode (BYTES=" + std::to_string(n_bytes) +
                 ", TIMEOUT=" + std::to_string(timeout_ms) + "ms)");
    // If the whole message should be sent at once, use normal send()-method
    if (n_bytes >= message.length()) {
        send(message);
    }

    int bytes_sent = 0;
    while (bytes_sent < message.length()) {
        if (bytes_sent + n_bytes >= message.length()) {
            n_bytes = (int) message.length() - bytes_sent;
        }
        ::send(file_descriptor, message.substr(bytes_sent, n_bytes).c_str(), n_bytes, 0);
        Logger::info("   Byte " + std::to_string(bytes_sent) + " to " +
                     std::to_string(bytes_sent + n_bytes - 1) + " were sent");
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        bytes_sent += n_bytes;
    }
}


HttpResponse Connection::receive_http_response() const {
    return HttpResponse{receive_bytes()};
}

std::string Connection::receive_string() const {
    const std::vector<char> vector = receive_bytes();
    return {vector.begin(), vector.end()};
}

std::shared_ptr<Address> Connection::get_address() {
    return address;
}

void Connection::send(HttpResponse &response) const {

    auto data = response.get_payload_as_binary();
    response.add_header("Content-Length", std::to_string(data.size()));

    const std::string &header = response.build_header();
    send(header);
    ::send(file_descriptor, data.data(), data.size(), 0);
}

void Connection::send(HttpResponse &response, Range range) const {
    auto data = response.get_payload_as_binary();

    //when the range start is not inside data, just send everything
    if (range.start > data.size()) {
        send(response);
        return;
    }

    //when the range end is not inside data, just send until the end
    if (range.end.has_value() && range.end.value() > data.size())
        range.end = std::nullopt;

    auto begin = data.begin() + range.start;
    auto size = range.end.has_value() ?
                (range.end.value() - range.start) + 1 :
                data.size() - range.start;

    response.add_header("Content-Length", std::to_string(size));

    const std::string &header = response.build_header();
    send(header);
    ::send(file_descriptor, begin.base(), size, 0);
}
