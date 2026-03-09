#ifndef HTTPS_PLAIN_CLIENT_HH
#define HTTPS_PLAIN_CLIENT_HH

#include "http_client.hh"
#include "credentials_provider.hh"

class HttpsClient : public HttpClient {
public:
    HttpsClient(ICredentialsProvider& creds) : _cred_provider(creds) {}

protected:
    void _configure_client(esp_http_client_config_t& config) override {       
        if (!_cred_provider.get_ca_cert().empty()) {
            config.cert_pem = _cred_provider.get_ca_cert().data();
        }

        if (!_cred_provider.get_client_cert().empty()) {
            config.client_cert_pem = _cred_provider.get_client_cert().data();
        }

        if (!_cred_provider.get_client_key().empty()) {
            config.client_key_pem = _cred_provider.get_client_key().data();
        }

        config.transport_type = HTTP_TRANSPORT_OVER_SSL;
    }

private:
    ICredentialsProvider& _cred_provider;
};

#endif