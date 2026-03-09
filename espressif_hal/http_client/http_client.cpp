
#include <cstring>

#include "http_client.hh"

#include "fic_log.hh"
#include "fic_time.hh"


HttpClient::HttpClient() : _runner("http client", 8192){}

HttpClient::~HttpClient() {
    stop();

    while(_active) {
        ITimeDelay::delay_ms(10);
    }
}

void HttpClient::start() {
    if (_running) return;
    _running = true;

    _runner.create_task(_http_task, this);
}

void HttpClient::stop() { 
    _running = false; 
}

fic_error_t HttpClient::patch(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_PATCH, listener);
}

fic_error_t HttpClient::put(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_PUT, listener);
}

fic_error_t HttpClient::post(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_POST, listener);
}

fic_error_t HttpClient::get(std::string_view url, IHTTPListener& listener) {
    return _enqueue(url, "", HTTP_METHOD_GET, listener);
}

fic_error_t HttpClient::del(std::string_view url, IHTTPListener& listener) {
    return _enqueue(url, "", HTTP_METHOD_DELETE, listener);
}

fic_error_t HttpClient::_enqueue(std::string_view url, std::string_view payload, esp_http_client_method_t method, IHTTPListener& listener) {
    if (url.size() >= URL_MAX_LEN) return FIC_ERR_INVALID_ARG;
    if (payload.size() >= PAYLOAD_MAX_LEN) return FIC_ERR_INVALID_ARG;
    if (_job_queue.full()) return FIC_ERR_FULL;
    
    HttpJob* job = _job_pool.acquire();
    if (!job) return FIC_ERR_FULL;

    memset(job, 0, sizeof(HttpJob));

    job->method = method;
    strlcpy(job->url, url.data(), URL_MAX_LEN);

    if (!payload.empty()) {
        strlcpy(job->payload, payload.data(), PAYLOAD_MAX_LEN);
    }
    
    job->listener = &listener;

    return _job_queue.push(job) ? FIC_OK : FIC_ERR_FULL;
}

esp_err_t HttpClient::_event_handler(esp_http_client_event_t *evt) {

    auto* self = static_cast<HttpClient*>(evt->user_data);

    self->_rx_len = 0;
    self->_rx_buffer.fill(0);

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            FIC_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            FIC_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            FIC_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            FIC_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            FIC_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            self->_rx_len = 0;
            self->_rx_buffer.fill(0);

            if (!esp_http_client_is_chunked_response(evt->client)) {
                int copy_len = MIN(evt->data_len, MAX_RESPONSE_PAYLOAD);
                if (copy_len) {
                    memcpy(evt->user_data, evt->data, copy_len);
                }

            } else {
                FIC_LOGE(TAG, "chunked responses are not yet supported!");
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            FIC_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED: {
                FIC_LOGW(TAG, "HTTP_EVENT_DISCONNECTED");
                // int mbedtls_err = 0;
                // esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
                // if (err != 0) {
                //     FIC_LOGI(TAG, "Last esp error code: 0x%x", err);
                //     FIC_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
                // }
                break;
            }

        case HTTP_EVENT_REDIRECT:
            FIC_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_redirection(evt->client);
            break;
    }

    return ESP_OK;
}

void HttpClient::_http_task(void *instance) {
    auto* self = static_cast<HttpClient*>(instance);
    HttpJob* job = nullptr;

    self->_active = true;

    while(self->_running) {
        if (self->_job_queue.pop(job, true, 0xFFFF)) {
            FIC_LOGI(TAG, "Requesting to %s with payload: %s", job->url, job->payload);

            esp_http_client_config_t config = {};

            config.event_handler = self->_event_handler;
            config.user_data = self;
            config.url = job->url;
            config.method = job->method;
            config.max_redirection_count = 3;

            esp_http_client_handle_t client = esp_http_client_init(&config);

            esp_http_client_set_header(client, "User-Agent", "Plant-monitor_9000");

            if (job->method == HTTP_METHOD_POST || job->method == HTTP_METHOD_PUT || job->method == HTTP_METHOD_PATCH) {
                esp_http_client_set_header(client, "Content-Type", "application/json");
                esp_http_client_set_post_field(client, job->payload, strlen(job->payload));
            }
            
            esp_err_t err = esp_http_client_perform(client);
            if (err == ESP_OK) {
                FIC_LOGI(TAG, "HTTP Status = %d, content_length = %" PRId64,
                        esp_http_client_get_status_code(client),
                        esp_http_client_get_content_length(client));
                
                Response response = {};
                response.status_code = esp_http_client_get_status_code(client);
                
                job->listener->on_success(response);
            } else {
                FIC_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
                job->listener->on_failure(FIC_ERR_SDK_FAIL);
            }

            esp_http_client_cleanup(client);

            _job_pool.release(job);
        }
    }

    self->_runner.delete_task();
    self->_active = false;
}
