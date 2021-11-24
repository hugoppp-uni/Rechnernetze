#pragma once
#include <string>

struct UrlInfo{
    const std::string host;
    const std::string file;

    UrlInfo(const std::string &host, const std::string &file);
};

class UrlParser {
public:
    static UrlInfo parse(const std::string& url);
};



