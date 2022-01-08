#pragma once
#include <cstddef>

enum class Flags : unsigned short {
    None = 0,
    SQN = 1 << 0,
    ACK = 1 << 1,
    FIN = 1 << 2,
    ABR = 1 << 3,
    SYN = 1 << 4,
};

class BftDatagram {
    unsigned short checksum;
    Flags flags;
    std::byte payload[512 - 32];
};

