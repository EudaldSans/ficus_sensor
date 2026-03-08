#ifndef HTTP_CLIENT_HH
#define HTTP_CLIENT_HH

#include <atomic>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "freertos_queue.hh"
#include "freertos_task.hh"

#include "esp_http_client.h"

#include "http_hal.hh"

#define URL_MAX_LEN 128
#define PAYLOAD_MAX_LEN 256
#define MAX_QUEUED_REQUESTS 2
#define MAX_RESPONSE_PAYLOAD 512

class HttpClient : public IHttpClient {
public:
    HttpClient();
    ~HttpClient();

    void start();
    void stop();

    fic_error_t patch(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t post(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t put(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t get(std::string_view url, IHTTPListener& listener) override;
    fic_error_t del(std::string_view url, IHTTPListener& listener) override;

private:
    struct HttpJob {
        char url[URL_MAX_LEN];
        char payload[PAYLOAD_MAX_LEN];
        IHTTPListener* listener;
        esp_http_client_method_t method;
    };

    static esp_err_t _event_handler(esp_http_client_event_t *evt);
    static void _http_task(void *instance);

    fic_error_t _enqueue(std::string_view url, std::string_view payload, esp_http_client_method_t method, IHTTPListener& listener);
    
    FreeRTOSQueue<HttpJob, MAX_QUEUED_REQUESTS> _job_queue;
    FreeRTOS_TaskRunner _runner;

    bool _initialized = false;
    
    std::atomic_bool _running = false;
    std::atomic_bool _active = false;

    std::array<char, MAX_RESPONSE_PAYLOAD> _rx_buffer;
    size_t _rx_len = 0;

    constexpr static char const *TAG = "HTTP client";
};

#endif