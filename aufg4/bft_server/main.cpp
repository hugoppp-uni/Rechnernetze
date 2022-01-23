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

int sock_fd;
std::unique_ptr<FileWriter> fileWriter;
std::chrono::time_point<std::chrono::system_clock> fileTransferStartTime;
unsigned int nDuplicates;
sockaddr_in client_addr{0};
bool currentSQN = SQN_START_VAL;

void handle_valid_datagram(BftDatagram &datagram);

bool handle_SYN(BftDatagram datagram, const std::string &dir);

void signalHandler(int signum) {
    Logger::info("Server shutting down");
    if (fileWriter) {
        BftDatagram::ABR.send(sock_fd, client_addr);
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

    struct sockaddr_in server_addr{
        .sin_family = AF_INET,
        .sin_port = htons(options.port),
        .sin_addr =  {htonl(INADDR_ANY)}
    };

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sock_fd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    nDuplicates = 0;
    while (true) {
        BftDatagram received_datagram;
        BftDatagram::receive(sock_fd, client_addr, received_datagram);

        if (!received_datagram.check_integrity()) {
            //request repeat of current packet
            BftDatagram(Flags::AGN, currentSQN).send(sock_fd, client_addr);
            continue;
        }

        if (received_datagram.is_SYN()) {
            if (handle_SYN(received_datagram, options.directory)) {
                received_datagram.create_ACK().send(sock_fd, client_addr);
                currentSQN = SQN_START_VAL ^ true;
            } else {
                BftDatagram::ABR.send(sock_fd, client_addr);
                //do not update SQN
            }
        } else if (received_datagram.is_ABR()) {
            if (!fileWriter)
                continue;
            Logger::warn("Got ABR, deleting '" + fileWriter->file_path + "'");
            received_datagram.create_ACK().send(sock_fd, client_addr);
            fileWriter->abort();
            fileWriter = nullptr;
        } else if (received_datagram.get_SQN() != currentSQN) {
            nDuplicates++;
            //todo ignore it or resend old ACK?
//            BftDatagram(Flags::ACK, currentSQN).send(sock_fd, client_addr);
            //do not update SQN
        } else {
            handle_valid_datagram(received_datagram);
            received_datagram.create_ACK().send(sock_fd, client_addr);
            currentSQN ^= true;
        }
    }
}

/// \return true, if connection established, else false
bool handle_SYN(BftDatagram datagram, const std::string &dir) {

    if (fileWriter) {
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

    fileWriter = std::make_unique<FileWriter>(filepath.string());
    fileTransferStartTime = std::chrono::system_clock::now();
    return true;
}

void handle_valid_datagram(BftDatagram &datagram) {
    if ((datagram.get_flags() & Flags::FIN) == Flags::FIN) {
        std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
        int elapsed = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - fileTransferStartTime).count();
        fileWriter->log_bytes_written();
        Logger::info(
            "Upload of '" + fileWriter->file_path + "' completed in " + std::to_string(elapsed) + " milliseconds.");
        Logger::debug("Number of duplicates: " + std::to_string(nDuplicates));
        fileWriter = nullptr;
    } else if (clear_flag(datagram.get_flags(), Flags::SQN) == Flags::None) {
        const std::vector<char> &payload = datagram.get_payload();
        fileWriter->writeBytes(payload);
        Logger::debug(
            "Wrote " + std::to_string(payload.size()) + "/" + std::to_string(fileWriter->get_bytes_written()) +
            " bytes to '" + fileWriter->file_path + "'");
    }

}
