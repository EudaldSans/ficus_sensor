#ifndef WIFI_CONTEXT_HH
#define WIFI_CONTEXT_HH

#include <atomic>
#include <mutex>

#include "wifi_hal.hh"

#include "esp_wifi.h"
#include "esp_netif_net_stack.h"
#include "esp_netif.h"

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
        WiFiContext& _ctx;
        std::lock_guard<std::recursive_mutex> _lock;
    public:
        ScopedAccess(WiFiContext& ctx) : _ctx(ctx), _lock(ctx._mutex) {}
        
        WiFiState state() { return _ctx._state; }
        void set_state(WiFiState s) { _ctx._state = s; }

        InternalMode mode() { return _ctx._mode; }
        void set_mode(InternalMode m) { _ctx._mode = m; }   

        using netif_creator = std::function<esp_netif_t*()>;
        esp_netif_t* netif() { return _ctx.netif; }
        void set_netif(netif_creator n) { 
            if (_ctx.netif != nullptr) {
                delete_netif();
            }
            _ctx.netif = n(); 
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

#endif