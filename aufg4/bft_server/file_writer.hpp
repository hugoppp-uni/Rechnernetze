#pragma once
#include <string>
#include <vector>
#include <fstream>


class FileWriter {
public:
    explicit FileWriter(std::string fileName);

    virtual ~FileWriter();

    void writeBytes(const std::vector<char>& data);
    void abort();
    const std::string file_path;
    unsigned long get_bytes_written() const { return bytes_written; }
private:
    std::ofstream file_stream;
    unsigned long bytes_written{0};


};



