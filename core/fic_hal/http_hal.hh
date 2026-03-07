#ifndef HTTP_HAL_HH
#define HTTP_HAL_HH

#include <string>
#include <functional>

#include "fic_errors.hh"
#include "queue_hal.hh"


struct Response {
    int status_code;
    char* payload;
    size_t payload_size;
};

class IHtpptListener {
public:
    /**
     * @brief Callback used when an HTTP request is completed to avoid blocking tasks.
     * 
     * @warning Response is volatile, will ONLY be valid during this callback.
     * @warning Avoid heavy processing during the callback, it is best not to block the http worker.
     * 
     * @param resp Reference to a @c Response object, with all parameters related to the response
     */
    virtual void on_success(const Response& resp) = 0;

    /**
     * @brief Callback used when an HTTP request is fails due to internal errors.
     * 
     * @param error Different to @c FIC_OK if the client suffered an internal error
     */
    virtual void on_failure(fic_error_t error) = 0;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    virtual fic_error_t patch(std::string_view url, std::string_view payload, IHtpptListener& listener) = 0;
    virtual fic_error_t post(std::string_view url, std::string_view payload, IHtpptListener& listener) = 0;
    virtual fic_error_t get(std::string_view url, std::string_view payload, IHtpptListener& listener) = 0;
    virtual fic_error_t del(std::string_view url, std::string_view payload, IHtpptListener& listener) = 0;
    virtual fic_error_t put(std::string_view url, std::string_view payload, IHtpptListener& listener) = 0;
};

#endif
