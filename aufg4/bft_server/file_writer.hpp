
#pragma once
#include <string>
#include <vector>


class FileWriter {
public:
    FileWriter(const std::string &fileName);
    void writeBytes(std::vector<unsigned char> data);

private:
    std::string file_name;

};



