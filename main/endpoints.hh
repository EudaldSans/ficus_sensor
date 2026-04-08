#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "firebase_encoder.hh"
#include "https_client.hh"
#include "credentials_provider.hh"

#include "sensor_endpoints.hh"
#include "hardware.hh"
#include "routing.hh"

extern const char root_cert_pem_start[] asm("_binary_root_cert_pem_start");
extern const char root_cert_pem_end[]   asm("_binary_root_cert_pem_end");

const char firebase_url[] = "https://ficus-base-default-rtdb.europe-west1.firebasedatabase.app/test_node.json";

const uint16_t sensor_meas_period_ms = 30000;

AsyncSensorEndpoint<float> t_endpoint(
    t_sensor_output,
    t_sensor,
    sensor_meas_period_ms
);

SensorEndpoint<float> h_endpoint(
    h_sensor_output,
    h_sensor,
    sensor_meas_period_ms
);

class FakeTLSProvider : public ICredentialsProvider {
public: 
    std::string_view get_client_cert() const override {return "";}
    std::string_view get_client_key() const override {return "";}
    std::string_view get_ca_cert() const override {
        size_t size_with_null = (root_cert_pem_end - root_cert_pem_start); 
        return std::string_view(root_cert_pem_start, size_with_null);
    }
};

FakeTLSProvider tls_provider = FakeTLSProvider();
HttpsClient http_client = HttpsClient(tls_provider);
FirebaseEncoder encoder = FirebaseEncoder(firebase_url, "id_test", http_client);

#endif