#include <vector>
#include "bft_datagram.hpp"
#include "flags.hpp"
#include <chrono>
#include <thread>
#include <cstring>

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

BftDatagram::BftDatagram(Flags flags, char *data_begin, char *data_end, bool sqn)
    : flags(flags), payload_size(data_end - data_begin) {
    std::copy(data_begin, data_end, payload.begin());
    if (sqn)
        this->flags = (flags | Flags::SQN);
    checksum = calc_checksum();
}

BftDatagram::BftDatagram(Flags flags, bool sqn)
    : flags(flags), payload_size(0) {
    if (sqn)
        this->flags = (flags | Flags::SQN);
    checksum = calc_checksum();
}

BftDatagram::BftDatagram(Flags flags, const std::string &data, bool sqn)
    : flags(flags), payload_size(data.size()) {
    data.copy(&payload[0], data.size());
    if (sqn)
        this->flags = (flags | Flags::SQN);
    checksum = calc_checksum();
}

bool BftDatagram::check_integrity() {
    return calc_checksum() == checksum && size() <= MAX_DATAGRAM_SIZE;
}

int BftDatagram::receive(int fd, sockaddr_in &client_addr, BftDatagram &response) {

    socklen_t len = sizeof client_addr;

    int bytes_recvd = (int) recvfrom(
        fd,
        (void *) &response,
        MAX_DATAGRAM_SIZE,
        MSG_WAITALL,
        (struct sockaddr *) &client_addr,
        &len);

    if(bytes_recvd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        errno = EAGAIN;
        return -1;
    } else if (bytes_recvd < 0) {
        perror("on select");
        exit(EXIT_FAILURE);
    }

    Logger::debug("˅ " + response.to_string());
    Logger::data("Payload:\n" + response.get_payload_as_string());
    std::this_thread::sleep_for(std::chrono::milliseconds (1));

    return bytes_recvd;
}

int BftDatagram::send(int sockfd, const sockaddr_in &client_addr) const {
    Logger::debug("˄ " + to_string());
    int bytes_sent = (int) sendto(
            sockfd,
            (void *) this,
            size(),
            MSG_CONFIRM,
            (struct sockaddr *) &client_addr,
            sizeof client_addr);

    Logger::data("Payload:\n" + get_payload_as_string());
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return bytes_sent;
}

std::string BftDatagram::to_string() const {
    return
        "'" + checksum_as_string() + ": " +
        flags_to_str(flags) +
        ", payload size: " + std::to_string(payload_size)
        + "'";
}
