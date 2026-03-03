#ifndef WIFI_ACCESS_POINT_HH
#define WIFI_ACCESS_POINT_HH

#include "wifi_context.hh"

#include "fic_errors.hh"

class WiFiAccessPoint : public IWiFiAccessPoint {
public:
    WiFiAccessPoint(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connections) override;
    fic_error_t stop_ap() override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi AP";
};

#endif