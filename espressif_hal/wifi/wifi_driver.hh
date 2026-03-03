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

class WiFiContext {
    friend class ScopedAccess;
private:
    esp_netif_t* netif = nullptr;    
    
    WiFiState _state = WiFiState::OFF;
    InternalMode _mode = InternalMode::NONE;
    
    std::recursive_mutex _mutex;  

    uint16_t _attempts = 0;

public:
    class ScopedAccess {
        std::lock_guard<std::recursive_mutex> _lock;
        WiFiContext& _ctx;
    public:
        ScopedAccess(WiFiContext& ctx) : _ctx(ctx), _lock(ctx._mutex) {}
        
        WiFiState state() { return _ctx._state; }
        void set_state(WiFiState s) { _ctx._state = s; }

        InternalMode mode() { return _ctx._mode; }
        void set_mode(InternalMode m) { _ctx._mode = m; }

        esp_netif_t* netif() { return _ctx.netif; }
        void set_netif(esp_netif_t* n) { 
            if (_ctx.netif != nullptr) {
                delete_netif();
            }
            _ctx.netif = n; 
        }
        void delete_netif() { 
            if (_ctx.netif != nullptr) {
                esp_netif_destroy_default_wifi(_ctx.netif);
                _ctx.netif = nullptr;
            }
        }

        uint16_t attempts() { return _ctx._attempts; }
        void increment_attempts() { _ctx._attempts++; }
        void reset_attempts() { _ctx._attempts = 0; }
    };

    /**
     * @brief Factory method to get scoped access to the WiFi context.
     * 
     * @return @c ScopedAccess class, allowing accessing and modifying context safely within the scope
     */
    ScopedAccess lock() { return ScopedAccess(*this); }  
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

    fic_error_t sta_connect(const char* ssid, const char* password, uint16_t retries) override;
    fic_error_t sta_disconnect() override;

private:
    WiFiContext& _ctx;

    constexpr static char const *TAG = "WiFi STA";
};
 
#endif