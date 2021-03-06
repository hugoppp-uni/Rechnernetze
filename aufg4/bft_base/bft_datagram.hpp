#pragma once

#include "lib/include/CRC.h"
#include "logger.hpp"
#include "flags.hpp"


#define HEADER_SIZE (sizeof(int) + sizeof(short) + sizeof(Flags))
#define MAX_DATAGRAM_SIZE 512
#define MAX_PAYLOAD_SIZE MAX_DATAGRAM_SIZE - HEADER_SIZE

using PayloadArray = std::array<char, MAX_PAYLOAD_SIZE>;

#pragma pack(push, 1)
class BftDatagram {
public:

    BftDatagram() : checksum(0), payload_size(0), flags(Flags::None) {};
    explicit BftDatagram(Flags flags, bool sqn);
    BftDatagram(Flags flags, char *data_begin, char *data_end, bool sqn);
    BftDatagram(Flags flags, const std::string &data, bool sqn);


    bool check_integrity();
    int send(int sockfd, const sockaddr_in &client_addr) const;

    static int receive(int fd, sockaddr_in &client_addr, BftDatagram &response);
    static const BftDatagram SYN;
    static const BftDatagram ABR;

    BftDatagram create_ACK() const;

    [[nodiscard]] std::string to_string() const;

    //getter
    [[nodiscard]] std::vector<char> get_payload() const { return {payload.begin(), payload.begin() + payload_size}; }
    [[nodiscard]] std::string get_payload_as_string() const { return {payload.begin(), payload.begin() + payload_size}; }
    [[nodiscard]] Flags get_flags() const { return flags; }
    [[nodiscard]] int get_payload_size() const { return payload_size; }
    [[nodiscard]] std::string checksum_as_string() const;
    [[nodiscard]] bool get_SQN() const{ return (flags & Flags::SQN) == Flags::SQN; }
    [[nodiscard]] int size() const { return payload_size + (int) HEADER_SIZE; }
    [[nodiscard]] bool is_SYN() const{ return get_flags() == SYN.flags && get_SQN() == SYN.get_SQN(); }
    [[nodiscard]] bool is_ABR() const{ return get_flags() == ABR.flags && get_SQN() == ABR.get_SQN(); }
    [[nodiscard]] bool is_ACK_for(const BftDatagram &original) const;

private:
    unsigned int checksum;
    unsigned short payload_size;
    Flags flags;
    PayloadArray payload = {0};

    [[nodiscard]] unsigned int calc_checksum();
};
#pragma pack(pop)

static_assert(sizeof(BftDatagram) == MAX_DATAGRAM_SIZE);
