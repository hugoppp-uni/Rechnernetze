#include <fstream>
#include <iostream>
#include "response.h"

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

std::string Response::get_metadata() {
    return headers;
}

std::vector<char> Response::get_payload_as_binary() {
    return payload;
}

bool Response::write_to_file(const std::string & filename) {
    std::ofstream output_file{filename, std::ios::binary};
    if (!output_file){
        std::cerr << "Error opening file '" + filename + "' for writing response data to..." << std::endl;
        return false;
    }

    for (char c : get_payload_as_binary()) {
        output_file << c;
    }

    std::cout << "Payload of reponse was written to file: " + filename << std::endl;
    output_file.close();
    return true;
}
