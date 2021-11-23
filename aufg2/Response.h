#ifndef AUFG2_RESPONSE_H
#define AUFG2_RESPONSE_H

#include <string>

class Response {
public:
    Response(const std::string& data);
    std::string GetMetadata();
    std::string GetPayload();

private:
    int status_code;
    std::string status_info;
    std::string headers;
    std::string payload;
};
#endif //AUFG2_RESPONSE_H
