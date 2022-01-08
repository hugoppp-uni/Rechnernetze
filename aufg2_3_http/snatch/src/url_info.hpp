#pragma once
#include <string>

struct UrlInfo{
    const std::string host;
    /*
     * The absolute path of the file, including the extension
     */
    const std::string file_path_name;
    /*
     * The name of the file, including the extension
     */
    const std::string file_name;

    const std::optional<unsigned short> port;


};

class UrlParser {
public:
    static UrlInfo parse(const std::string& url);
};



