#include "firebase_endpoint.hh"

#include <sys/time.h>


FirebaseEndpoint::~FirebaseEndpoint() {}

void FirebaseEndpoint::setup() {
    return;
}

void FirebaseEndpoint::update(uint64_t now) {
    static bool sent_data = false;

    if (_wifi_manager.get_state() != WiFiState::STA_CONNECTED) { return; }
    if (!_sntp_client.is_synced()) { return; }
    
    for (size_t i = 0; i < _num_channels; i++) {
        std::visit([this](auto& channel) {
            if (!channel->value.is_new()) { return; }

            auto value = channel->value.consume();
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, float>) {
                _firebase_encoder.update_data<float>(value, channel->name.data());
                FIC_LOGI(TAG, "Evaluated channel %.*s to %f", (int)channel->name.size(), channel->name.data(), value);
            } else if constexpr (std::is_same_v<T, bool>) {
                _firebase_encoder.update_data<bool>(value, channel->name.data());
                FIC_LOGI(TAG, "Evaluated channel %.*s to %s", (int)channel->name.size(), channel->name.data(), value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                _firebase_encoder.update_data<int>(value, channel->name.data());
                FIC_LOGI(TAG, "Evaluated channel %.*s to %d", (int)channel->name.size(), channel->name.data(), value);
            } else {
                FIC_LOGE(TAG, "Firebase channel is unknown type");
            }
        }, _channels[i]);
    }

    if(!sent_data) {
        _firebase_encoder.send_accumulated();
        sent_data = true;
    }
}



