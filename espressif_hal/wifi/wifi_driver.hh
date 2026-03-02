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

enum class InternalMode { NONE, STATION, ACCESS_POINT, SCANNING };

struct WiFiContext {
    esp_netif_t* netif = nullptr;    
    
    WiFiState state = WiFiState::OFF;
    InternalMode mode = InternalMode::NONE;
    
    std::recursive_mutex _mutex;    
};

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

class WiFiAccessPoint : public IWiFiAccessPoint {
public:
    WiFiAccessPoint(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connections) override;
    fic_error_t stop_ap() override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi AP";
};

class WiFiStation : public IWiFiStation {
public:
    WiFiStation(WiFiContext& ctx) : _ctx(ctx) {}

    fic_error_t sta_connect(const char* ssid, const char* password) override;
    fic_error_t sta_disconnect() override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi STA";
};
 
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
    fic_error_t get_scan_results(WiFiScanItem* results, size_t &max_results) const override;

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
    uint32_t _current_ip;

    std::atomic<WiFiState> _state = WiFiState::OFF;
    std::atomic<bool> _initialized = false;

    uint8_t _retries = 0;
    uint8_t _max_retries = 5;

};

#endif