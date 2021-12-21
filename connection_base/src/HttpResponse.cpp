#include <fstream>
#include <iostream>
#include "HttpResponse.hpp"
#include "logger.hpp"

HttpResponse::HttpResponse(const std::vector<char> &data) {

    if (!header.empty()) {
        header.clear();
    }
    if (!payload.empty()) {
        payload.clear();
    }

    // Get header data (until "empty" line)
    int i;
    for (i = 0; i < data.size(); i++) {
        // If EOF is not reached and there are 2* CRLF, this is the end of the header
        if (i + 4 <= data.size()
            && data[i] == '\r'
            && data[i + 1] == '\n'
            && data[i + 2] == '\r'
            && data[i + 3] == '\n') {

            header += "\r\n";
            i += 4; // Move index to begin of payload
            break;
        }
        header += data[i];
    }

    for (; i < data.size(); i++) {
        payload.insert(payload.end(), data[i]);
    }
}

std::string HttpResponse::get_metadata() {
    return header;
}

std::vector<char> HttpResponse::get_payload_as_binary() {
    return payload;
}

std::string HttpResponse::get_payload_as_string() {
    return {payload.begin(), payload.end()};
}

bool HttpResponse::write_to_file(const std::string &filename) {
    std::ofstream output_file{filename, std::ios::binary};
    if (!output_file) {
        std::cerr << "Error opening file '" + filename + "' for writing response data to..." << std::endl;
        return false;
    }

    for (char c: get_payload_as_binary()) {
        output_file << c;
    }

    Logger::info( "Payload of reponse was written to file: " + filename);
    output_file.close();
    return true;
}

void HttpResponse::set_content(const std::vector<char> &data) {
    payload = data;
}

void HttpResponse::set_content(const std::string &data) {
    payload = { data.begin(), data.end() };
}

void HttpResponse::add_header(const std::string &key, const std::string &value) {
    header += key + ": " + value + "\r\n";
}

std::string HttpResponse::get_status_text(HttpResponse::Status &status) {
    switch (status) {
        case OK:                        return "OK";
        case ACCEPTED:                  return "Accepted";
        case PARTIAL_CONTENT:           return "Partial Content";
        case MULTIPLE_CHOICES:          return "Multiple Choices";
        case MOVED_PERMANENTLY:         return "Moved Permanently";
        case BAD_REQUEST:               return "Bad Request";
        case UNAUTHORIZED:              return "Unauthorized";
        case FORBIDDEN:                 return "Forbidden";
        case NOT_FOUND:                 return "Not Found";
        case METHOD_NOT_ALLOWED:        return "Method Not Allowed";
        case NOT_ACCEPTABLE:            return "Not Acceptable";
        case REQUEST_TIMEOUT:           return "Request Timeout";
        case UNSUPPORTED_MEDIA_TYPE:    return "Unsupported Media Type";
        case INTERNAL_SERVER_ERROR:     return "Internal Server Error";
        case NOT_IMPLEMENTED:           return "Not Implemented";
        case SERVICE_UNAVAILABLE:       return "Service Unavailable";
        case VERSION_NOT_SUPPORTED:     return "HTTP Version Not Suported";
    }
    return "UNKNOWN STATUS CODE";
}

std::string HttpResponse::build() {
    result_stream.clear();

    // TODO: return Status-Line + Header + Data
    // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
    result_stream << "HTTP/1.1 " << status << " " << get_status_text(status) << " \r\n";
    result_stream << header << "\r\n" << std::string{payload.begin(), payload.end()};
    return result_stream.str();
}

HttpResponse::HttpResponse() {
    status = ACCEPTED; // Initially unknown
}

void HttpResponse::set_status(HttpResponse::Status s) {
    status = s;
}



