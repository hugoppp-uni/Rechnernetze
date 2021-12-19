#pragma once

#include <filesystem>

namespace helper {

std::vector<char> read_file(std::filesystem::path &file_path);

}

