#ifndef HTTP_CLIENT_HH
#define HTTP_CLIENT_HH

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"

#include "http_hal.hh"

class HttpClient : public IHttpClient {
public:
    ~HttpClient() {if (_initialized) deinit();}

    fic_error_t init();
    fic_error_t deinit();

    fic_error_t patch(const Request& req, Response& resp, char* rx_buffer, size_t rx_size);
    fic_error_t post(const Request& req, Response& resp, char* rx_buffer, size_t rx_size);
    fic_error_t get(const Request& req, Response& resp, char* rx_buffer, size_t rx_size);
    fic_error_t del(const Request& req, Response& resp, char* rx_buffer, size_t rx_size);
    fic_error_t put(const Request& req, Response& resp, char* rx_buffer, size_t rx_size);

private:
    esp_err_t _event_handler(esp_http_client_event_t *evt);

    bool _initialized = false;

};

#endif