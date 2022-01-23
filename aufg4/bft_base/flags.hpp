#include <cstddef>
#include <array>
#include <netinet/in.h>
#include "lib/include/CRC.h"
#include "logger.hpp"

#pragma once

#define SQN_START_VAL false

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

Flags clear_flag(Flags original, Flags flags_to_clear);

std::string flags_to_str(Flags flags);

