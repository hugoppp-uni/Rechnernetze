#include "file_writer.hpp"

#include <utility>
#include <iterator>
#include <filesystem>
#include "logger.hpp"

FileWriter::FileWriter(std::string file_path) : file_path(std::move(file_path)),
                                                file_stream(this->file_path, std::ios::out | std::ios::binary | std::ios::app) {
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

void FileWriter::abort() {
    file_stream.close();
    if (bytes_written > 0) {
        std::filesystem::remove(file_path);
    }
}

unsigned long FileWriter::get_bytes_written() const { return bytes_written; }
