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


#define HEADER_SIZE (sizeof(int) + sizeof(short) + sizeof(Flags))
#define MAX_DATAGRAM_SIZE 512

#pragma pack(1)
struct BftDatagram {
    unsigned int checksum;
    unsigned short payload_size;
    Flags flags;
    std::array<char, MAX_DATAGRAM_SIZE - HEADER_SIZE> payload;

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

    [[nodiscard]] int size() const{
        return payload_size + (int) HEADER_SIZE;
    }

    void build() {
        checksum = calc_checksum();
    }

    bool check_integrity() {
        return calc_checksum() == checksum && size() <= MAX_DATAGRAM_SIZE;
    }
};

static_assert(sizeof(BftDatagram) == MAX_DATAGRAM_SIZE);
