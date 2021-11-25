#include "Response.h"

using namespace std;

Response::Response(const std::vector<char>& data) {

    if (!headers.empty()) {
        headers.clear();
    }
    if (!payload.empty()) {
        payload.clear();
    }

    // Get header data (until "empty" line)
    int i;
    for(i = 0; i < data.size(); i++) {
        // If EOF is not reached and there are 2* CRLF, this is the end of the header
        if ( i+4 <= data.size() && data[i]=='\r' && data[i+1]=='\n' && data[i+2]=='\r' && data[i+3]=='\n' ){
            headers += "\r\n";
            i += 4; // Move index to begin of payload
            break;
        }
        headers += data[i];
    }

    for(; i < data.size(); i++) {
        payload.insert(payload.end(), data[i]);
    }
}

std::string Response::GetMetadata() {
    return headers;
}

std::string Response::GetPayloadAsString() {
    return {payload.begin(), payload.end()};
}

std::vector<char> Response::GetPayloadAsBinary() {
    return payload;
}