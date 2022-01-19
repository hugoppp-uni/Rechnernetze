#include "file_writer.hpp"

#include <utility>
#include "logger.hpp"

FileWriter::FileWriter(std::string fileName) : file_name(std::move(fileName)) {}

void FileWriter::writeBytes(const std::vector<char>& data) {
    //todo write to file
    Logger::debug("Writing " + std::to_string(data.size()) + " bytes to '" + file_name + "'");
}
