#pragma once

#include <cstddef>
#include <array>
#include <netinet/in.h>
#include "lib/include/CRC.h"
#include "logger.hpp"

enum class Flags : unsigned short {
    None = 0,
    SQN = 1 << 0,
    ACK = 1 << 1,
    FIN = 1 << 2,
    ABR = 1 << 3,
    SYN = 1 << 4,
    ERR = 1 << 5
};

Flags operator|(Flags lhs, Flags rhs) {
    return static_cast<Flags>(
        static_cast<std::underlying_type<Flags>::type>(lhs) |
        static_cast<std::underlying_type<Flags>::type>(rhs)
    );
}

Flags operator&(Flags lhs, Flags rhs) {
    return static_cast<Flags>(
        static_cast<std::underlying_type<Flags>::type>(lhs) &
        static_cast<std::underlying_type<Flags>::type>(rhs)
    );
}

std::string flags_to_str(Flags flags){
    std::stringstream ss;
    if ((flags & Flags::ACK) == Flags::ACK)
        ss << "ACK ";
    if ((flags & Flags::SYN) == Flags::SYN)
        ss << "SYN ";
    if ((flags & Flags::ERR) == Flags::ERR)
        ss << "ERR ";
    if ((flags & Flags::ABR) == Flags::ERR)
        ss << "ERR ";
    if ((flags & Flags::FIN) == Flags::FIN)
        ss << "FIN ";
    if ((flags & Flags::SQN) == Flags::SQN)
        ss << "SQN ";

    std::string str = ss.str();
    str.pop_back();
    return str;
}


#define HEADER_SIZE (sizeof(int) + sizeof(short) + sizeof(Flags))
#define MAX_DATAGRAM_SIZE 512
using PayloadArray = std::array<char, MAX_DATAGRAM_SIZE - HEADER_SIZE>;

#pragma pack(1)

class BftDatagram {
private:
    unsigned int checksum;
    unsigned short payload_size;
    Flags flags;
    PayloadArray payload = {0};

public:

    [[nodiscard]] unsigned int calc_checksum() {
        //we don't want to include the checksum field itself
        constexpr size_t crc_size = sizeof(BftDatagram) - offsetof(BftDatagram, payload_size);
        static_assert(crc_size < MAX_DATAGRAM_SIZE);
        return CRC::Calculate(&payload_size, crc_size, CRC::CRC_32());
    }

    [[nodiscard]] std::string checksum_as_string() const {
        std::stringstream stream;
        stream << std::hex << checksum;
        return stream.str();
    }

    [[nodiscard]] int size() const {
        return payload_size + (int) HEADER_SIZE;
    }

    BftDatagram(Flags flags, std::vector<char> data)
        : flags(flags), payload_size(data.size()) {
        checksum = calc_checksum();
        std::copy(data.begin(), data.end(), payload.begin());
    }

    explicit BftDatagram(Flags flags)
        : flags(flags), payload_size(0) {
        checksum = calc_checksum();
    }

    BftDatagram(Flags flags, const std::string &data)
        : flags(flags), payload_size(data.size()) {
        data.copy(&payload[0], data.size());
        checksum = calc_checksum();
    }

    BftDatagram() : checksum(0), payload_size(0), flags(Flags::None) {};

    bool check_integrity() {
        return calc_checksum() == checksum && size() <= MAX_DATAGRAM_SIZE;
    }

    static BftDatagram receive(int fd, sockaddr_in &client_addr) {

        BftDatagram datagram;
        socklen_t len = sizeof client_addr;

        int bytes_recvd = (int) recvfrom(
            fd,
            (void *) &datagram,
            MAX_DATAGRAM_SIZE,
            MSG_WAITALL,
            (struct sockaddr *) &client_addr,
            &len);

        if (bytes_recvd < 0) {
            perror("Error during receive");
            exit(EXIT_FAILURE);
        }


        Logger::debug("Received datagram " + datagram.to_string());
        return datagram;
    }

    int send(int sockfd, const sockaddr_in &client_addr) const {

        Logger::debug("Sending datagram " + to_string());
        int bytes_send = (int) sendto(
            sockfd,
            (void *) this,
            size(),
            MSG_CONFIRM,
            (struct sockaddr *) &client_addr,
            sizeof client_addr);

        return bytes_send;
    }

    [[nodiscard]] std::vector<char> get_payload() const { return {payload.begin(), payload.begin() + payload_size}; }
    [[nodiscard]] std::string get_payload_as_string() const { return {payload.begin(), payload.begin() + payload_size}; }
    [[nodiscard]] Flags get_flags() const { return flags; }
    [[nodiscard]] int get_payload_size() const { return payload_size; }

    [[nodiscard]] std::string to_string() const {
        return
        "'" + checksum_as_string() + ": " +
        flags_to_str(flags) +
        ", payload size: " + std::to_string(payload_size)
        + "'";
    }

};

static_assert(sizeof(BftDatagram) == MAX_DATAGRAM_SIZE);
