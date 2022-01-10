#pragma once

#include <cstddef>
#include <array>
#include "lib/include/CRC.h"

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


#define HEADER_SIZE (3 * sizeof(short))
#define MAX_DATAGRAM_SIZE 512

class BftDatagram {

public:
    explicit BftDatagram();
    explicit BftDatagram(Flags flags);
    Flags flags;
    unsigned int checksum{};
    unsigned short payload_size{};
    std::array<char, MAX_DATAGRAM_SIZE - HEADER_SIZE> payload{};

    unsigned int calc_checksum();
    [[nodiscard]] unsigned short size() const;
    std::string Payload();

};

