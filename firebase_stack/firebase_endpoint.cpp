#include "firebase_endpoint.hh"

#include <sys/time.h>


FirebaseEndpoint::~FirebaseEndpoint() {}

void FirebaseEndpoint::setup() {
    return;
}

void FirebaseEndpoint::update(uint64_t now) {
    if (_wifi_manager.get_state() != WiFiState::STA_CONNECTED) { return; }
    if (!_sntp_client.is_synced()) { return; }
    
    for (size_t i = 0; i < _num_channels; i++) {
        std::visit([this](auto& channel) {
            if (!channel->value.is_new()) { return; }

            auto value = channel->value.consume();
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, float>) {
                FIC_LOGI(TAG, "Evaluated channel %.*s to %f", (int)channel->name.size(), channel->name.data(), value);
            } else if constexpr (std::is_same_v<T, bool>) {
                FIC_LOGI(TAG, "Evaluated channel %.*s to %s", (int)channel->name.size(), channel->name.data(), value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                FIC_LOGI(TAG, "Evaluated channel %.*s to %d", (int)channel->name.size(), channel->name.data(), value);
            } else {
                FIC_LOGI(TAG, "FML");
            }
        }, _channels[i]);
    }
}



