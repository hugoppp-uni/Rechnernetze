#include "Response.h"

#include <regex>
#include <sstream>

using namespace std;

Response::Response(const std::string& data) {

    istringstream iss(data);
    string line;

    // TODO: Get first line and extract status code and status text
    status_code = 200;
    status_info = "OK";

    // Get header data (until "empty" line)
    headers = "";
    while (getline(iss, line)){
        if (line == "\r") {
            break;
        } else {
            headers += line + "\n";
        }
    }

    // Get payload - TODO: store as byte-array
    payload = "";
    while (getline(iss, line)){
        payload += line + "\n";
    }
}

std::string Response::GetMetadata() {
    return headers;
}

std::string Response::GetPayload() {
    // TODO: If payload is stored as byte-array, convert into string
    return payload;
}