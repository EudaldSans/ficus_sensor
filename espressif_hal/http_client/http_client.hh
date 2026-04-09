#ifndef HTTP_CLIENT_HH
#define HTTP_CLIENT_HH

#include <atomic>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "freertos_queue.hh"
#include "freertos_task.hh"

#include "object_pool.hh"

#include "esp_http_client.h"

#include "http_hal.hh"

#define RUNNER_STACK 8129

class HttpClient : public IHttpClient {
public:
    HttpClient() : _runner("http client", RUNNER_STACK) {}
    ~HttpClient();

    void start();
    void stop();

    fic_error_t patch(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t post(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t put(std::string_view url, std::string_view payload, IHTTPListener& listener) override;
    fic_error_t get(std::string_view url, IHTTPListener& listener) override;
    fic_error_t del(std::string_view url, IHTTPListener& listener) override;

protected:
    virtual void _configure_client(esp_http_client_config_t& config) = 0;

private:
    static constexpr size_t _k_url_max_len         = 128;
    static constexpr size_t _k_payload_max_len     = 256;
    static constexpr size_t _k_max_queued_requests = 2;
    static constexpr size_t _k_max_response_payload = 512;

    struct HttpJob {
        char url[_k_url_max_len];
        char payload[_k_payload_max_len];
        IHTTPListener* listener;
        esp_http_client_method_t method;
    };

    static esp_err_t _event_handler(esp_http_client_event_t *evt);
    static void _http_task(void *instance);

    fic_error_t _enqueue(std::string_view url, std::string_view payload, esp_http_client_method_t method, IHTTPListener& listener);
    
    ObjectPool<HttpJob, _k_max_queued_requests> _job_pool;
    FreeRTOSQueue<HttpJob*, _k_max_queued_requests> _job_queue;
    FreeRTOS_TaskRunner _runner;

    bool _initialized = false;
    
    std::atomic_bool _running = false;
    std::atomic_bool _active = false;

    std::array<char, _k_max_response_payload> _rx_buffer;
    size_t _rx_len = 0;

    constexpr static char const *TAG = "HTTP client";
};

#endif