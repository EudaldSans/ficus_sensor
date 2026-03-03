#ifndef WIFI_STATION_HH
#define WIFI_STATION_HH

#include "wifi_context.hh"
#include "esp_wifi.h"

class WiFiStation : public IWiFiStation {
public:
    WiFiStation(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t sta_connect(const char* ssid, const char* password, uint16_t retries) override;
    fic_error_t sta_disconnect() override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi STA";
};

#endif