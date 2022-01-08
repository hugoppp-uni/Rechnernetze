
#include <regex>
#include "url_info.hpp"

UrlInfo UrlParser::parse(const std::string &url) {
    std::regex regex(".*?(?:http:[/][/])?(?:www[.])?([^\\.]*[.][^/]*)[/](.*)");
    std::smatch matches;
    if (!std::regex_match(url, matches, regex) || matches.size() != 3) {

        std::regex localhostRegex("localhost(?::(\\d+))?[/](.*)");
        if (std::regex_match(url, matches, localhostRegex)) {

            std::string port = matches[1];
            std::string full_file_name = matches[2];
            std::string file_name = url.substr(url.find_last_of('/') + 1);


            std::optional<unsigned short> portInt;
            if (!port.empty())
                portInt = std::stoi(port);

            return UrlInfo{.host = "localhost", .file_path_name = full_file_name, .file_name = file_name, .port = portInt};
        }

        throw std::invalid_argument("url");
    }

    std::string host = matches[1];
    std::string full_file_name = matches[2];
    std::string file_name = url.substr(url.find_last_of('/') + 1);

    return UrlInfo{.host = host, .file_path_name = full_file_name, .file_name = file_name};

}
