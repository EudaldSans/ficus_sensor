#ifndef FIREBASE_ENDPOINT_HH
#define FIREBASE_ENDPOINT_HH

#include <variant>

#include "task.hh"

#include "wifi_hal.hh"
#include "sntp_hal.hh"

#include "fic_log.hh"
#include "fic_errors.hh"

#include "firebase_channel.hh"
#include "firebase_encoder.hh"

class FirebaseEndpoint : public IContinuousTask {
public:
    template<size_t N>
    FirebaseEndpoint(FirebaseChannelPtr (&channels)[N], IWiFiStatusManager& wifi_manager, ISntpClient& sntp_client, FirebaseEncoder& firebase_encoder) 
        : _channels(channels), _num_channels(N), _wifi_manager(wifi_manager), _sntp_client(sntp_client), _firebase_encoder(firebase_encoder) {}

    ~FirebaseEndpoint() override;

    void setup() override;
    void update(uint64_t now) override;

private:
    FirebaseChannelPtr* _channels;
    size_t _num_channels;

    IWiFiStatusManager& _wifi_manager;
    ISntpClient& _sntp_client;
    FirebaseEncoder& _firebase_encoder;

    constexpr static char const *TAG = "FIREBASE ENDPOINT";
};



#endif