#pragma once
#include <string>
#include <vector>
#include <fstream>


class FileWriter {
public:
    explicit FileWriter(std::string file_path);

    virtual ~FileWriter();

    void writeBytes(const std::vector<char>& data);
    void abort();
    const std::string file_path;
    unsigned long get_bytes_written() const;
private:
    std::ofstream file_stream;
    unsigned long bytes_written{0};


};



