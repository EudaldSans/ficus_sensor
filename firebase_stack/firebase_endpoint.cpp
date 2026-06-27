#include "firebase_endpoint.hh"

#include <sys/time.h>


FirebaseEndpoint::~FirebaseEndpoint() {}

void FirebaseEndpoint::setup() {
    bool periodic = true;

    _emit_timeout.start(2000, periodic);
}

void FirebaseEndpoint::update(uint32_t now) {
    static bool send_data = false;

    if (!_emit_timeout.has_expired()) { return; }
    if (_wifi_manager.get_state() != WiFiState::STA_CONNECTED) { return; }
    if (!_sntp_client.is_synced()) { return; }

    if (_sync_state == no_sync) {
        struct timeval tv;
        
        gettimeofday(&tv, NULL);
        long long time_in_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

        uint32_t time_since_synchronization = time_in_ms % _emit_period_ms;
        uint32_t time_to_next_synchronization = _emit_period_ms - time_since_synchronization;

        FIC_LOGI(TAG, "Syncing report time: Time since sync %d, time to sync %d", time_since_synchronization, time_to_next_synchronization);

        _emit_timeout.update_duration(time_to_next_synchronization);
        _sync_state = first_sync;

        return;

    } else if (_sync_state == first_sync) {
        FIC_LOGI(TAG, "First sync completed");
        _emit_timeout.update_duration(_emit_period_ms);
        _sync_state = full_sync;
    }

    send_data = false;
    
    for (size_t i = 0; i < _num_channels; i++) {
        std::visit([this](auto& channel) {
            if (!channel->value.is_new()) { return; }

            auto value = channel->value.consume();
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, float>) {
                value = (int)(value * 100 + 0.5) / 100.0;
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

            send_data = true;

        }, _channels[i]);
    }

    if (send_data) _firebase_encoder.send_accumulated();
}



