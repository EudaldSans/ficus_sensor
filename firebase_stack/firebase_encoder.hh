#ifndef FIREBASE_ENCODER_HH
#define FIREBASE_ENCODER_HH

#include <string>
#include <array>

#include "task.hh"

#include "fic_errors.hh"
#include "http_hal.hh"

#define MAX_FIREBASE_PAYLOAD_SIZE 1024

class FirebaseEncoder {
public:    
    FirebaseEncoder(IHttpClient& http_client) : _http_client(http_client) {}
    ~FirebaseEncoder() = default;

    fic_error_t initialize();

    template <typename T>
    fic_error_t update_data(const T& data, std::string_view name);
    fic_error_t send_accumulated();

    template <typename T>
    fic_error_t send_immediately(const T& data, std::string_view name);

private: 
    // Probably will end up being a JSON object
    std::array<char, MAX_FIREBASE_PAYLOAD_SIZE> _payload_buffer;
    size_t _payload_size = 0;

    IHttpClient& _http_client;
};

#endif