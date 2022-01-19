#include <vector>
#include "bft_datagram.hpp"
#include "flags.hpp"

unsigned int BftDatagram::calc_checksum() {
    //we don't want to include the checksum field itself
    constexpr size_t crc_size = sizeof(BftDatagram) - offsetof(BftDatagram, payload_size);
    static_assert(crc_size < MAX_DATAGRAM_SIZE);
    return CRC::Calculate(&payload_size, crc_size, CRC::CRC_32());
}

std::string BftDatagram::checksum_as_string() const {
    std::stringstream stream;
    stream << std::hex << checksum;
    return stream.str();
}

BftDatagram::BftDatagram(Flags flags, std::vector<char> data)
    : flags(flags), payload_size(data.size()) {
    checksum = calc_checksum();
    std::copy(data.begin(), data.end(), payload.begin());
}

BftDatagram::BftDatagram(Flags flags)
    : flags(flags), payload_size(0) {
    checksum = calc_checksum();
}

BftDatagram::BftDatagram(Flags flags, const std::string &data)
    : flags(flags), payload_size(data.size()) {
    data.copy(&payload[0], data.size());
    checksum = calc_checksum();
}

bool BftDatagram::check_integrity() {
    return calc_checksum() == checksum && size() <= MAX_DATAGRAM_SIZE;
}

BftDatagram BftDatagram::receive(int fd, sockaddr_in &client_addr) {

    BftDatagram datagram;
    socklen_t len = sizeof client_addr;

    int bytes_recvd = (int) recvfrom(
        fd,
        (void *) &datagram,
        MAX_DATAGRAM_SIZE,
        MSG_WAITALL,
        (struct sockaddr *) &client_addr,
        &len);

    if (bytes_recvd < 0) {
        perror("Error during receive");
        exit(EXIT_FAILURE);
    }


    Logger::debug("Received datagram " + datagram.to_string());
    return datagram;
}

int BftDatagram::send(int sockfd, const sockaddr_in &client_addr) const {

    Logger::debug("Sending datagram " + to_string());
    int bytes_send = (int) sendto(
        sockfd,
        (void *) this,
        size(),
        MSG_CONFIRM,
        (struct sockaddr *) &client_addr,
        sizeof client_addr);

    return bytes_send;
}

std::string BftDatagram::to_string() const {
    return
        "'" + checksum_as_string() + ": " +
        flags_to_str(flags) +
        ", payload size: " + std::to_string(payload_size)
        + "'";
}

