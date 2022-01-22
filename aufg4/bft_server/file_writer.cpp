#include "file_writer.hpp"

#include <utility>
#include <iterator>
#include <filesystem>
#include "logger.hpp"

using std::chrono::system_clock;

FileWriter::FileWriter(std::string file_path) : file_path(std::move(file_path)),
                                                file_stream(this->file_path,
                                                            std::ios::out | std::ios::binary | std::ios::app),
                                                last_log(system_clock::now()) {
    if (!file_stream.is_open()) {
        //todo proper error handling
        Logger::error("file not opened");
        exit(1);
    }
}

FileWriter::~FileWriter() {
    if (file_stream.is_open())
        file_stream.close();
}

void FileWriter::writeBytes(const std::vector<char> &data) {

    if ((system_clock::now() - last_log) > std::chrono::seconds(1)) {
        log_bytes_written();
    }

    if (!file_stream.is_open()) {
        //todo proper error handling
        Logger::error("file not opened");
        exit(1);
    }

    for (const auto &data_byte: data) {
        file_stream << data_byte;
    }
    file_stream.flush();
    bytes_written += data.size();
}

void FileWriter::log_bytes_written() {

    auto ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>((system_clock::now() - last_log)).count();

    Logger::info(
        "Wrote total of " + std::to_string(bytes_written) +
        " bytes to '" + file_path + "' (" + std::to_string( ((bytes_written - last_bytes_written) / ms_elapsed)) + "kB/s)");

    last_bytes_written = bytes_written;
    last_log = system_clock::now();
}

void FileWriter::abort() {
    file_stream.close();
    if (bytes_written > 0) {
        std::filesystem::remove(file_path);
    }
}

unsigned long FileWriter::get_bytes_written() const { return bytes_written; }
