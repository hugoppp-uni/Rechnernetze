#include <iomanip>
#include <chrono>
#include <map>
#include <filesystem>
#include <HttpRequest.h>
#include <vector>
#include <fstream>
#include "helper.hpp"
#include "fmt/format.h"

namespace fs = std::filesystem;

namespace helper {

std::vector<char> read_file(const fs::path &file_path) {
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    std::vector<char> contents((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    in.close();

    return contents;
}

std::string file_size_to_str(uint64_t size) {
    std::array<std::string, 5> units = {"B", "kB", "MB", "GB", "TB"};

    int order = 0;
    auto size_double = (double) size;
    while (size_double >= 1024 && order < units.size() - 1) {
        order++;
        size_double /= 1024;
    }

    return fmt::format("{:.1f}{}", size_double, units[order]);
}

std::string time_t_to_string(time_t &tt, const char *format) {
    tm *gmt = gmtime(&tt);
    std::stringstream buffer;
    buffer << std::put_time(gmt, format);
    std::string formattedFileTime = buffer.str();
    return formattedFileTime;
}


}