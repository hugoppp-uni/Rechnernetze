
#pragma once
#include <string>
#include <vector>


class FileWriter {
public:
    FileWriter(std::string fileName);
    void writeBytes(const std::vector<char>& data);
    const std::string file_name;


};



