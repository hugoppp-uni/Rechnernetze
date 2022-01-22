#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <chrono>


class FileWriter {
public:
    explicit FileWriter(std::string file_path);

    virtual ~FileWriter();

    void writeBytes(const std::vector<char>& data);
    void abort();
    const std::string file_path;
    unsigned long get_bytes_written() const;
    void log_bytes_written();

private:
    std::ofstream file_stream;
    unsigned long bytes_written{0};
    unsigned long last_bytes_written{0};
    std::chrono::time_point<std::chrono::system_clock> last_log;
};



