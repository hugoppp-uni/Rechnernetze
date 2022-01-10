
#include <vector>
#include "bft_datagram.hpp"

BftDatagram::BftDatagram() {
    flags = Flags::None;
}

unsigned short BftDatagram::size() const {
    return payload_size + HEADER_SIZE;
}

unsigned int BftDatagram::calc_checksum() {
        checksum = CRC::Calculate(this, (size_t) sizeof(*this), CRC::CRC_32());
        return checksum;
}

std::string BftDatagram::Payload() {
    return std::string{payload.begin(), payload.begin() + payload_size};
}

BftDatagram::BftDatagram(Flags flags) {
    this->flags = flags;
}



