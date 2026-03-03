#ifndef HTTP_HAL_HH
#define HTTP_HAL_HH

#include <string>

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

    virtual Response patch(const Request& req, char* rx_buffer, size_t rx_size) = 0;
    virtual Response post(const Request& req, char* rx_buffer, size_t rx_size) = 0;
    virtual Response get(const Request& req, char* rx_buffer, size_t rx_size) = 0;
    virtual Response del(const Request& req, char* rx_buffer, size_t rx_size) = 0;
    virtual Response put(const Request& req, char* rx_buffer, size_t rx_size) = 0;
};

#endif
