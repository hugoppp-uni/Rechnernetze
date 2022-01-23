#include "options.hpp"
#include "bft_datagram.hpp"
#include "file_writer.hpp"

//c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */
#include "logger.hpp"
#include "flags.hpp"
#include <csignal>

#include <iomanip>  // Includes ::std::hex
#include <filesystem>
#include <utility>

struct ClientData {
    FileWriter file_writer;
    const std::chrono::time_point<std::chrono::system_clock> file_transfer_start_time;
    const sockaddr_in address{};
    unsigned int n_duplicates = 0;
    bool current_SQN = SQN_START_VAL;

    explicit ClientData(std::string file_path, sockaddr_in client_addr)
        : file_writer(std::move(file_path)),
          address(client_addr),
          file_transfer_start_time(std::chrono::system_clock::now()) {}

    void increment_SQN() {
        current_SQN ^= true;
    }
};

int sock_fd;
std::unique_ptr<ClientData> current_client;

void handle_valid_datagram(BftDatagram &datagram);

bool handle_SYN(BftDatagram datagram, const std::string &dir, sockaddr_in client_addr);

void init_socket(unsigned short port);

void signalHandler(int signum) {
    Logger::info("Server shutting down");
    if (current_client) {
        BftDatagram::ABR.send(sock_fd, current_client->address);
    }
    close(sock_fd);
    exit(EXIT_SUCCESS);
}


int main(int argc, char **args) {
    signal(SIGINT, signalHandler); // Cleanup when pressing CTRL+C

    Options options{argc, args};
    if (options.debug)
        Logger::set_level(Logger::level::DEBUG);
    else
        Logger::set_level(Logger::level::INFO);

    if (options.debug) {
        Logger::info("BFT Server started in Debug mode");
    } else {
        Logger::info("BFT Server started in Production mode");
    }
    Logger::info("Listening on port " + std::to_string(options.port));
    Logger::info("Incoming files will be saved into folder: " + options.directory);

    init_socket(options.port);

    while (true) {
        BftDatagram received_datagram;
        sockaddr_in sender_addr{};
        BftDatagram::receive(sock_fd, sender_addr, received_datagram);

        if (!received_datagram.check_integrity()) {
            //request repeat of current packet
            BftDatagram(Flags::AGN, current_client->current_SQN).send(sock_fd, sender_addr);
            continue;
        }

        if (received_datagram.is_SYN()) {
            if (handle_SYN(received_datagram, options.directory, sender_addr)) {
                received_datagram.create_ACK().send(sock_fd, sender_addr);
                current_client->increment_SQN();
            } else {
                BftDatagram::ABR.send(sock_fd, sender_addr);
                //do not update SQN
            }
            continue;
        }

        if (received_datagram.is_ABR()) {
            received_datagram.create_ACK().send(sock_fd, sender_addr);
            if (current_client != nullptr && sender_addr.sin_addr.s_addr == current_client->address.sin_addr.s_addr) {
                Logger::warn("Client aborted upload, deleting '" + current_client->file_writer.file_path + "'");
                current_client->file_writer.abort();
                current_client = nullptr;
            }
            continue;
        }

        if (current_client == nullptr || sender_addr.sin_addr.s_addr != current_client->address.sin_addr.s_addr) {
            //got a datagram from a different address
            BftDatagram::ABR.send(sock_fd, sender_addr);
            continue;
        }

        // a regular datagram
        if (received_datagram.get_SQN() != current_client->current_SQN) {
            current_client->n_duplicates++; //todo ignore it or resend old ACK?
        } else {
            received_datagram.create_ACK().send(sock_fd, current_client->address);
            current_client->increment_SQN();
            handle_valid_datagram(received_datagram);
        }
    }
}


void init_socket(unsigned short port) {
    struct sockaddr_in server_addr{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr =  {htonl(INADDR_ANY)}
    };

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sock_fd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

/// \return true, if connection established, else false
bool handle_SYN(BftDatagram datagram, const std::string &dir, sockaddr_in client_addr) {

    if (current_client) {
        Logger::warn("Busy, incoming connection denied");
        return false;
    }

    std::string filename = datagram.get_payload_as_string();

    const std::filesystem::path &filepath = std::filesystem::path(dir) / filename;
    if (exists(filepath)) {
        std::filesystem::remove(filepath);
        Logger::info("Incoming connection, replacing file '" + filename + "'");
    } else {
        Logger::info("Incoming connection, creating file '" + filename + "'");
    }

    current_client = std::make_unique<ClientData>(filepath.string(), client_addr);
    return true;
}

void handle_valid_datagram(BftDatagram &datagram) {
    if ((datagram.get_flags() & Flags::FIN) == Flags::FIN) {
        std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
        int elapsed = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - current_client->file_transfer_start_time).count();
        current_client->file_writer.log_bytes_written();
        Logger::info(
            "Upload of '" + current_client->file_writer.file_path + "' completed in " + std::to_string(elapsed) +
            " milliseconds.");
        Logger::debug("Number of duplicates: " + std::to_string(current_client->n_duplicates));
        current_client = nullptr;
    } else if (clear_flag(datagram.get_flags(), Flags::SQN) == Flags::None) {
        const std::vector<char> &payload = datagram.get_payload();
        current_client->file_writer.writeBytes(payload);
        Logger::debug(
            "Wrote " + std::to_string(payload.size()) + "/" +
            std::to_string(current_client->file_writer.get_bytes_written()) +
            " bytes to '" + current_client->file_writer.file_path + "'");
    }

}
