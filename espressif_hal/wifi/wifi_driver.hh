#ifndef WIFI_DRIVER_HH
#define WIFI_DRIVER_HH

#include <atomic>
#include <mutex>

#include "wifi_hal.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"

#include "esp_netif_net_stack.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
 
class WifiDriver :  public IWiFiScanner, 
                    public IWiFiStation, 
                    public IWiFiAccessPoint, 
                    public IWiFiLifecycle, 
                    public IWiFiStatusManager {
public:
    WifiDriver() = default;
    ~WifiDriver() = default;

    fic_error_t init() override;
    fic_error_t deinit() override;
    void stop() override;

    fic_error_t start_scan() override; 
    bool is_scan_busy() const override;
    fic_error_t get_scan_results(WiFiScanItem* results, size_t* count) const override;

    WiFiState get_state() const override;  
    ConnectionDetails get_details() const override;   
    
    fic_error_t sta_connect(const char* ssid, const char* password) override;
    fic_error_t sta_disconnect() override;

    fic_error_t start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connections) override;
    fic_error_t stop_ap() override;

private:
    static void wifi_event_handler(void *instance, esp_event_base_t event_base, int32_t event_id, void *event_data);

    EventGroupHandle_t _wifi_event_group;
    wifi_init_config_t _cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_netif_t *_esp_netif;

    std::atomic<WiFiState> _state = WiFiState::OFF;
    std::atomic<bool> _initialized = false;

    uint8_t _retries = 0;
    uint8_t _max_retries = 5;

    enum class InternalMode { NONE, STATION, ACCESS_POINT, SCANNING };
    InternalMode _current_mode = InternalMode::NONE;

    std::mutex _mutex;

    constexpr static char const *TAG = "WiFi Driver";
};

#endif