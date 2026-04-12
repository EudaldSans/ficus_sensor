#ifndef FIREBASE_ENCODER_HH
#define FIREBASE_ENCODER_HH

#include <ArduinoJson.h>

#include <string>
#include <array>

#include "task.hh"

#include "fic_errors.hh"
#include "http_hal.hh"

#include "fic_log.hh"

#include <sys/time.h>

#define MAX_FIREBASE_PAYLOAD_SIZE 512

class FirebaseEncoder : public IHTTPListener {
public:    
    FirebaseEncoder(const char* url, const char* device_id, IHttpClient& http_client) : _url(url), _device_id(device_id), _http_client(http_client), _doc() {
        reset_doc();
    }
    ~FirebaseEncoder() = default;

    template <typename T>
    fic_error_t update_data(const T& data, const char* name) {
        if (_doc.overflowed()) {
            FIC_LOGE(TAG, "Could not add %s to JSON doc", name);
            return FIC_ERR_NO_MEM;
        }

        _doc[name] = data;
        FIC_LOGI(TAG, "Added %s to JSON doc", name);
        return FIC_OK;
    } 
    
    fic_error_t send_accumulated() {
        JsonDocument final_document;
        struct timeval tv;
        
        gettimeofday(&tv, NULL);

        final_document[_device_id][tv.tv_sec] = _doc;

        size_t output_len = serializeJson(final_document, _output_buffer);

        FIC_LOGI(TAG, "Sending payload (%d), %.*s", output_len, output_len, _output_buffer);

        return _http_client.post(_url, _output_buffer, *this);
    }

    void on_success(const Response& resp) {
        if (resp.status_code == 200) {
            FIC_LOGI(TAG, "Payload sent successfuly");
            reset_doc();
        } else {
            FIC_LOGE(TAG, "Payload failed with %d", resp.status_code);
        }
    }

    void on_failure(fic_error_t error) {
        FIC_LOGE(TAG, "Payload failed internally with %d", error);
    }

private: 
    size_t _payload_size = 0;

    const char* _url;
    const char* _device_id;

    char _output_buffer[MAX_FIREBASE_PAYLOAD_SIZE];

    IHttpClient& _http_client;
    JsonDocument _doc; // TODO: Find about a way to make memory management safer and more predictable

    void reset_doc() {
        _doc.clear();
        _doc[_device_id] = JsonObject();
    }

    constexpr static char const *TAG = "FIREBASE ENCODER";
};

#endif