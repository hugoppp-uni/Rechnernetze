#include "flags.hpp"

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

Flags operator^(Flags lhs, Flags rhs) {
    return static_cast<Flags>(
        static_cast<std::underlying_type<Flags>::type>(lhs) ^
        static_cast<std::underlying_type<Flags>::type>(rhs)
    );
}

std::string flags_to_str(Flags flags) {
    std::stringstream ss;
    if (flags == Flags::None)
        ss << "(no flags) ";
    else {
        if ((flags & Flags::ACK) == Flags::ACK)
            ss << "ACK ";
        if ((flags & Flags::SYN) == Flags::SYN)
            ss << "SYN ";
        if ((flags & Flags::AGN) == Flags::AGN)
            ss << "AGN ";
        if ((flags & Flags::ABR) == Flags::ABR)
            ss << "ABR ";
        if ((flags & Flags::FIN) == Flags::FIN)
            ss << "FIN ";
        if ((flags & Flags::SQN) == Flags::SQN)
            ss << "SQN ";
    }

    std::string str = ss.str();
    if (!str.empty())
        str.pop_back();
    return str;
}

Flags clear_flag(Flags original, Flags flags_to_clear) {
    return static_cast<Flags>(
        static_cast<std::underlying_type<Flags>::type>(original)
        & ~static_cast<std::underlying_type<Flags>::type>(flags_to_clear)
    );
}
