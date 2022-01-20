#include <cstddef>
#include <array>
#include <netinet/in.h>
#include "lib/include/CRC.h"
#include "logger.hpp"

#pragma once

enum class Flags : unsigned short {
    None = 0,
    SQN = 1 << 0,
    ACK = 1 << 1,
    FIN = 1 << 2,
    ABR = 1 << 3,
    SYN = 1 << 4,
    AGN = 1 << 5
};

Flags operator|(Flags lhs, Flags rhs);
Flags operator&(Flags lhs, Flags rhs);
Flags operator^(Flags lhs, Flags rhs);
Flags operator~(Flags lhs);
std::string flags_to_str(Flags flags);
void incrementSQN(Flags &flags);

