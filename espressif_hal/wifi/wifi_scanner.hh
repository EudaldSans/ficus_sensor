#ifndef WIFI_SCANNER_HH
#define WIFI_SCANNER_HH

#include "wifi_context.hh"

#include "fic_errors.hh"

class WiFiScanner : public IWiFiScanner {
public:
    WiFiScanner(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t start_scan() override; 
    bool is_scan_busy() const override;
    fic_error_t get_scan_results(WiFiScanItem* results, size_t &max_results) const override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi Scanner";
};

#endif