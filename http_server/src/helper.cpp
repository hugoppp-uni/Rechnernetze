
#include <vector>
#include <fstream>
#include "helper.hpp"

namespace fs = std::filesystem;

std::vector<char> helper::read_file(const fs::path &file_path) {
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    std::vector<char> contents((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    in.close();

    return contents;
}

std::string helper::file_size_to_str(uint64_t size) {
    std::array<std::string, 5> units = {"B", "kB", "MB", "GB", "TB"};

    int order = 0;
    auto size_double = (double)size;
    while (size_double >= 1024 && order < units.size() - 1) {
        order++;
        size_double /= 1024;
    }


    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(1);
    ss << size_double << units[order];
    return ss.str();
}
