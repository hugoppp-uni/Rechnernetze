
#pragma once
#include <string>
#include <vector>


class FileWriter {
public:
    FileWriter(const std::string &fileName);
    void writeBytes(const std::vector<char>& data);

private:
    std::string file_name;

};



