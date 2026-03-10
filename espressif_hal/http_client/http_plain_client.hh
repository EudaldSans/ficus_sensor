#ifndef HTTP_PLAIN_CLIENT_HH
#define HTTP_PLAIN_CLIENT_HH

#include "http_client.hh"

class PlainHttpClient : public HttpClient {
protected:
    /**
     * @brief Just sets transport type to TCP, no TLS support
     * 
     * @param config @c esp_http_client_config_t to complement client configuration
     */
    void _configure_client(esp_http_client_config_t& config) override {
        config.transport_type = HTTP_TRANSPORT_OVER_TCP;
    }
};

#endif
