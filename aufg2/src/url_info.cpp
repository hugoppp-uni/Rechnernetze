
#include <regex>
#include "url_info.hpp"

UrlInfo UrlParser::parse(const std::string &url) {
    std::regex regex(".*?(?:http:[/][/])?(?:www[.])?([^\\.]*[.][^/]*)[/](.*)");
    std::smatch matches;
    if (!std::regex_match(url, matches, regex) || matches.size() != 3) {
        throw std::invalid_argument("url");
    }

    std::string host = matches[1];
    std::string full_file_name = matches[2]; // TODO: File has to be only the string after the last '/'. Currently it is the full path, e.g. "~fdc/picture-of-something.jpg"
    std::string file_name = url.substr(url.find_last_of('/') + 1);

    return UrlInfo{.host = matches[1], .file_path_name = matches[2], .file_name = file_name};

}
