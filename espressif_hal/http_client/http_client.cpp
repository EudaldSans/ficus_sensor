
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

/**
 * @brief Orders the http task end event handler to start 
 */
void HttpClient::start() {
    if (_running) return;
    _running = true;

    _runner.create_task(_http_task, this);
}

/**
 * @brief Orders the http task and event handler to stop 
 */
void HttpClient::stop() { 
    _running = false; 
}

/**
 * @brief Enqueue a PATCH request
 * 
 * @param url The URL to send the request to
 * @param payload The payload to send with the request
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL or payload are too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
fic_error_t HttpClient::patch(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_PATCH, listener);
}

/**
 * @brief Enqueue a PUT request
 * 
 * @param url The URL to send the request to
 * @param payload The payload to send with the request
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL or payload are too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
fic_error_t HttpClient::put(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_PUT, listener);
}

/**
 * @brief Enqueue a POST request
 * 
 * @param url The URL to send the request to
 * @param payload The payload to send with the request
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL or payload are too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
fic_error_t HttpClient::post(std::string_view url, std::string_view payload, IHTTPListener& listener) {
    return _enqueue(url, payload, HTTP_METHOD_POST, listener);
}

/**
 * @brief Enqueue a GET request
 * 
 * @param url The URL to send the request to
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL is too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
fic_error_t HttpClient::get(std::string_view url, IHTTPListener& listener) {
    return _enqueue(url, "", HTTP_METHOD_GET, listener);
}

/**
 * @brief Enqueue a DELETE request
 * 
 * @param url The URL to send the request to
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL is too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
fic_error_t HttpClient::del(std::string_view url, IHTTPListener& listener) {
    return _enqueue(url, "", HTTP_METHOD_DELETE, listener);
}

/**
 * @brief Enqueue an http request with the given parameters
 * 
 * @param url The URL to send the request to
 * @param payload The payload to send with the request
 * @param method The @c esp_http_client_method_t method to use for the request 
 * @param listener Reference to the listener to handle the response
 * @return 
 *  - @c FIC_ERR_INVALID_ARG if the URL is too long, 
 *  - @c FIC_ERR_FULL if the queue is full, 
 *  - @c FIC_OK if the request was enqueued successfully
 */
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

/**
 * @brief Espressif event handler. Handles all HTTP events. 
 * 
 * @param evt @c esp_http_client_event_t event received from the SDK
 * @return @c esp_err_t with the result of the event handling, usually @c ESP_OK
 */
esp_err_t HttpClient::_event_handler(esp_http_client_event_t *evt) {

    auto* self = static_cast<HttpClient*>(evt->user_data);

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
                self->_rx_len = MIN(evt->data_len, MAX_RESPONSE_PAYLOAD);
                if (self->_rx_len) {
                    memcpy(self->_rx_buffer.data(), evt->data, self->_rx_len);
                }

            } else {
                FIC_LOGE(TAG, "chunked responses are not yet supported!");
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            FIC_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED: 
            FIC_LOGW(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
            
        case HTTP_EVENT_REDIRECT:
            FIC_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_redirection(evt->client);
            break;
    }

    return ESP_OK;
}

/**
 * @brief Obtains @c HttpJob from the queue job and handles it.
 * 
 * @param instance Pointer to the @c HttpClient instance
 */
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
                response.payload_size = self->_rx_len;
                response.payload = self->_rx_buffer.data();
                
                job->listener->on_success(response);
            } else {
                FIC_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
                job->listener->on_failure(FIC_ERR_SDK_FAIL);
            }

            esp_http_client_cleanup(client);

            self->_job_pool.release(job);
        }
    }

    self->_runner.delete_task();
    self->_active = false;
}


//                                                       ..,;;:::::...,,,,:,,,,,,,,
//                                                     .,::,,::.1:.
//              ,,..        ;;:;,:::,.                .:,:  ,,. ,..
//         .1CCi:t;::,     ,1;::itttti:               ,,,. ,.
//        :GCCf1i111ii;.   .   .::11ii;.              ..,.
//       .CLffttffLLft1i. .   ,:::ii:,:.    ..... ..     .
//       tC1LttffftLL1;::.   ,;1tii1ti;    i111,.,,:.    .:,           ..
//      ,ti1t;iii::1fi;;::   :;;;;;ii;.    :;i:,::;;.     ;LCffftff1i1Lt.         .
//    ,,;;,;::i111tttttfi:.  .itfftLL1:::. :i: :;:::.      18@@@@@@@0Gf.
// itLft;;;;:;i11tfttt1ii, ,1CGGCCCLLCCLt. ,:. .,,:,      .GGG80LG8888:
// 1t:,:if1;1;;i1i1i::,:;;L088CfLC0fi;1;  ,,.    .,.      ,LCtL00GG08@i
// ;: ,1GC::;,:;i1tii11itGCCLtfLLCCf;,,:  ,,  .   .,      .ft;i;ifCCGG:
// i:tG0GftLtf1:::i111fCGft1,.:tti,.         .     .    .:;1tftttttttti;;:;:.  ....
// ;it;;:,.:;i;,ii.,1C0GCCi.   .,.                     ,t1...;111LLfftii;;t1i,tCGCf
// 1;,        .;;iifGGCCCi    ..,,                .  .:,..       ....,,,.,.,ifLffi:
// CCCL,:fLfi, :1L0GCLLf;      .,.                    ....,,,::,,,,,,,::1ttiitLfttt
// 1ii:.C8008GiLGCCLfff;       .,.       ..    ......          ::i;::;:,iLLLLCLLLLf
// ;11:1C0000GCCLLfftf;        .,.       ..,:,,,;;:;;;;;;;;iii1fLCLLCLt1it1ttttt1t1
// :;;;iffGCCLffftttt;..        .      .. .,,.........,,,,,:itLttffttfttt1111111i;;
//   .: :tLLfftttt1i, .         ,.    ...  ..,,........... ,i1fii1t1111111t1i11tt;t
//      Me yelling at my code                       My code doing exactly
//    for not doing what I want                   what I programmed it to do
