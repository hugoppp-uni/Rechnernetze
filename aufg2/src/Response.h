#ifndef AUFG2_RESPONSE_H
#define AUFG2_RESPONSE_H

#include <string>
#include <vector>

class Response {
public:
    Response(const std::vector<char>& data);
    std::string GetMetadata();
    std::string GetPayloadAsString();
    std::vector<char> GetPayloadAsBinary();

private:
    int status_code;
    std::string status_info;
    std::string headers;
    std::vector<char> payload;

};
#endif //AUFG2_RESPONSE_H
