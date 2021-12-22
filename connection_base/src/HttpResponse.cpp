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

std::string HttpResponse::get_metadata() const {
    return header;
}

std::vector<char> HttpResponse::get_payload_as_binary() const {
    return payload;
}

std::string HttpResponse::get_payload_as_string() const {
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

    Logger::info("Payload of reponse was written to file: " + filename);
    output_file.close();
    return true;
}

void HttpResponse::set_content(const std::vector<char> &data) {
    payload = data;
}

void HttpResponse::set_content(const std::string &data) {
    payload = {data.begin(), data.end()};
}

void HttpResponse::add_header(const std::string &key, const std::string &value) {
    header += key + ": " + value + "\r\n";
}

std::string HttpResponse::get_status_text(HttpResponse::Status status){
    switch (status) {
        case Status::OK:
            return "OK";
        case Status::ACCEPTED:
            return "Accepted";
        case Status::PARTIAL_CONTENT:
            return "Partial Content";
        case Status::MULTIPLE_CHOICES:
            return "Multiple Choices";
        case Status::MOVED_PERMANENTLY:
            return "Moved Permanently";
        case Status::BAD_REQUEST:
            return "Bad Request";
        case Status::UNAUTHORIZED:
            return "Unauthorized";
        case Status::FORBIDDEN:
            return "Forbidden";
        case Status::NOT_FOUND:
            return "Not Found";
        case Status::METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case Status::NOT_ACCEPTABLE:
            return "Not Acceptable";
        case Status::REQUEST_TIMEOUT:
            return "Request Timeout";
        case Status::UNSUPPORTED_MEDIA_TYPE:
            return "Unsupported Media Type";
        case Status::INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case Status::NOT_IMPLEMENTED:
            return "Not Implemented";
        case Status::SERVICE_UNAVAILABLE:
            return "Service Unavailable";
        case Status::VERSION_NOT_SUPPORTED:
            return "HTTP Version Not Suported";
        case Status::NONE:
            return "unknown";
    }
    return "UNKNOWN STATUS CODE";
}

std::string HttpResponse::build_header() const{
    std::stringstream result_stream;
    result_stream.clear();

    // TODO: return Status-Line + Header + Data
    // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
    result_stream << "HTTP/1.1 " << static_cast<int>(status) << " " << get_status_text(status) << " \r\n";
    result_stream << header << "\r\n";// << std::string{payload.begin(), payload.end()};
    return result_stream.str();
}

HttpResponse::HttpResponse() {
    status = Status::ACCEPTED; // Initially unknown
}

void HttpResponse::set_status(const HttpResponse::Status s) {
    status = s;
}

std::string HttpResponse::get_status_text() const{
    return get_status_text(status);
}

HttpResponse::Status HttpResponse::get_status_code() const {
    return status;
}




