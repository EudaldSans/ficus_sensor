#ifndef HTTP_HAL_HH
#define HTTP_HAL_HH

#include <string>

#include "fic_errors.hh"

class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    struct Request {
        std::string_view url;
        std::string_view payload;
    };

    struct Response {
        int status_code;
        char* buffer;
        size_t bytes_written;
    };

    virtual fic_error_t patch(const Request& req, Response& resp, char* rx_buffer, size_t rx_size) = 0;
    virtual fic_error_t post(const Request& req, Response& resp, char* rx_buffer, size_t rx_size) = 0;
    virtual fic_error_t get(const Request& req, Response& resp, char* rx_buffer, size_t rx_size) = 0;
    virtual fic_error_t del(const Request& req, Response& resp, char* rx_buffer, size_t rx_size) = 0;
    virtual fic_error_t put(const Request& req, Response& resp, char* rx_buffer, size_t rx_size) = 0;
};

#endif
