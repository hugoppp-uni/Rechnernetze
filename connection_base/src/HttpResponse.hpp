#pragma once

#include <string>
#include <vector>
#include <sstream>


class HttpResponse {
public:
    /*
     * HTTP Status Codes according to RFC 2616
     * 1xx: Informational
     * 2xx: Successful
     * 3xx: Redirection
     * 4xx: Client Error
     * 5xx: Server Error
     */
    enum class Status {
        NONE = 0,
        OK = 200,
        ACCEPTED = 202,
        PARTIAL_CONTENT = 206,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        REQUEST_TIMEOUT = 408,
        UNSUPPORTED_MEDIA_TYPE = 415,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        SERVICE_UNAVAILABLE = 503,
        VERSION_NOT_SUPPORTED = 505,
    };

    HttpResponse();

    /**
     * Build Response from byte array
     * @param data byte-array of data which should be parsed as response
     */
    explicit HttpResponse(const std::vector<char>& data);

    /**
     * Get header data of Response
     * @return HTTP Header
     */
    std::string get_metadata();

    /**
     * Gets the content of the Response
     * @return Response content as byte array
     */
    std::vector<char> get_payload_as_binary();

    /**
     * Get the content of the response
     * @return Response content as string
     */
    std::string get_payload_as_string();

    /**
     * Write content of the Response to file
     * @param filename Path to the file that should be written to
     * @return
     */
    bool write_to_file(const std::string& filename);

    /**
     * Get text for the HTTP Status-Code
     * @param status HTTP-Status
     * @return status as string
     */
    static std::string get_status_text(Status &status);

    /**
     * Adds a new field to the HTTP header as key-value pair
     * @param key : Key (e.g. Content-Range)
     * @param value : Value (e.g. 0-1024)
     */
    void add_header(const std::string& key, const std::string& value);

    /**
     * Set the status of the Response
     * @param s HTTP-Status
     */
    void set_status(Status s);

    std::string get_status_text();
    Status get_status_code();

    /**
     * Sets the content that should be sent with the response
     * @param data byte-array with data
     */
    void set_content(const std::vector<char>& data);

    /**
     * Sets the content that should be sent with the response
     * @param data content as string
     */
    void set_content(const std::string& data);

    /**
     * Builds the HTTP-Response:
     * Status-Line + Header + Payload
     * @return the final HTTP-Response
     */
    std::string build();

private:
    Status status = Status::NONE;
    std::string header;
    std::vector<char> payload;
    std::stringstream result_stream;

};
