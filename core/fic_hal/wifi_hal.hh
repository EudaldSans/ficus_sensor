#ifndef WIFI_HAL_HH
#define WIFI_HAL_HH

#include <inttypes.h>
#include <memory>

#include "fic_errors.hh"

struct ConnectionDetails {
    char ip_address[16];
    int8_t rssi;
};

struct WiFiScanItem {
    char ssid[32];
    int8_t rssi;
    uint8_t channel;
    bool secure;
};

enum class WiFiState {
    OFF,
    IDLE,
    
    STA_CONNECTING,
    STA_CONNECTED,
    STA_DISCONNECTING,
    STA_DISCONNECTED,

    AP_STARTING,
    AP_ACTIVE,
    AP_STOPPED,

    SCANNING,

    ERROR_AUTH_FAILED,
    ERROR_AP_NOT_FOUND,
    ERROR_MODE_CONFLICT,
    ERROR_HARDWARE_FAILURE,
    ERROR_UNKNOWN
};

class IWiFiScanner {
    public:
        virtual ~IWiFiScanner() = default;

        virtual fic_error_t start_scan() = 0; 

        virtual bool is_scan_busy() const = 0;
        virtual fic_error_t get_scan_results(WiFiScanItem* results, size_t* count) const = 0;
};

class IWiFiStatusManager {
    public:
        virtual ~IWiFiStatusManager() = default;

        virtual WiFiState get_state() const = 0;  
        virtual ConnectionDetails get_details() const = 0;    
};

class IWiFiLifecycle {
    public:
        virtual ~IWiFiLifecycle() = default;

        virtual fic_error_t init() = 0;
        virtual fic_error_t deinit() = 0;

        virtual void stop();
};

class IWiFiStation {
    public:
        virtual ~IWiFiStation() = default;

        virtual fic_error_t sta_connect(const char* ssid, const char* password) = 0;
};

class IWiFiAccessPoint {
    public:
        virtual ~IWiFiAccessPoint() = default;

        virtual fic_error_t start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connection) = 0;
};

#endif 
