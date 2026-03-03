#ifndef WIFI_CONTROLLER_HH
#define WIFI_CONTROLLER_HH

#include "wifi_context.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"

#include "fic_errors.hh"

class WiFiController : public IWiFiLifecycle, public IWiFiStatusManager {
public:
    WiFiController(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t init() override;
    fic_error_t deinit() override;
    void stop() override;

    WiFiState get_state() const override;  
    ConnectionDetails get_details() const override;   

private:
    static void wifi_event_handler(void *instance, esp_event_base_t event_base, int32_t event_id, void *event_data);

    WiFiContext& _ctx;
    uint32_t _current_ip;

    std::atomic<bool> _initialized = false;
    wifi_init_config_t _cfg = WIFI_INIT_CONFIG_DEFAULT();

    EventGroupHandle_t _event_group;

    constexpr static char const *TAG = "WiFi Controller";
};

#endif