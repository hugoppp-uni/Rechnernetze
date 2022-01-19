#include "file_writer.hpp"
#include "logger.hpp"

FileWriter::FileWriter(const std::string &fileName) : file_name(fileName) {}

void FileWriter::writeBytes(const std::vector<char>& data) {
    //todo write to file
    Logger::debug("Writing " + std::to_string(data.size()) + " bytes to '" + file_name + "'");
}
