#pragma once

#include <filesystem>

namespace helper {

std::vector<char> read_file(const std::filesystem::path &file_path);

std::string file_size_to_str(uint64_t size);

std::string time_t_to_string(time_t &tt, const char* format);

template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                        + system_clock::now());
    return
        system_clock::to_time_t(sctp);
}

}

