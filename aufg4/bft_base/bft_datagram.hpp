#pragma once

#include <cstddef>
#include <array>

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


#define header_size (3 * sizeof(short))
#define max_digeram_size 512

struct BftDatagram {
    unsigned short checksum;
    unsigned short payload_size;
    Flags flags;
    std::array<char, max_digeram_size - header_size> payload;


    [[nodiscard]] short calc_checksum() const {
        return static_cast<short>(flags);
    }

    [[nodiscard]] int size() const{
        return payload_size + header_size;
    }

};

