#include "file_writer.hpp"

#include <utility>
#include <iterator>
#include <filesystem>

FileWriter::FileWriter(std::string fileName) : file_path(std::move(fileName)),
                                               file_stream(fileName, std::ios_base::out) {
}

FileWriter::~FileWriter() {
    if (file_stream.is_open())
        file_stream.close();
}

void FileWriter::writeBytes(const std::vector<char> &data) {

    for (const auto &data_byte: data) {
        file_stream << data_byte;
    }
    bytes_written += data.size();
}

void FileWriter::abort() {
    file_stream.close();
    if (bytes_written > 0) {
        std::filesystem::remove(file_path);
    }
}
