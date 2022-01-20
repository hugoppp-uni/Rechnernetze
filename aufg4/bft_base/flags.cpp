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

Flags operator~(Flags lhs) {
    return static_cast<Flags>(
        ~static_cast<std::underlying_type<Flags>::type>(lhs)
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
        if ((flags & Flags::ABR) == Flags::AGN)
            ss << "ABR ";
        if ((flags & Flags::FIN) == Flags::FIN)
            ss << "FIN ";
        if ((flags & Flags::SQN) == Flags::SQN)
            ss << "SQN ";
    }

    std::string str = ss.str();
    str.pop_back();
    return str;
}
