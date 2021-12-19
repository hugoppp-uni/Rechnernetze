#pragma once

#include <filesystem>

namespace helper {

std::vector<char> read_file(const std::filesystem::path &file_path);

std::string file_size_to_str(uint64_t size);
}

