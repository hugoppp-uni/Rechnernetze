
#include <vector>
#include <fstream>
#include "helper.hpp"

namespace fs = std::filesystem;

std::vector<char> helper::read_file(fs::path &file_path) {
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    std::vector<char> contents((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    in.close();

    return contents;
}
