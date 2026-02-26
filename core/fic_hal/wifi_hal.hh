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
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED,

    ERROR_AUTH_FAILED,
    ERROR_AP_NOT_FOUND,
    ERROR_UNKNOWN
};

class IWiFiScanner {
    public:
        virtual ~IWiFiScanner() = default;

        virtual fic_error_t start_scan() = 0; 

        virtual bool is_scan_busy() const = 0;
        virtual fic_error_t get_scan_results(WiFiScanItem* results, size_t* count) = 0;
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

        virtual void init() = 0;
        virtual void deinit() = 0;
};

class IWiFiStation {
    public:
        virtual ~IWiFiStation() = default;

        virtual fic_error_t connect(const char* ssid, const char* password) = 0;
        virtual void disconnect() = 0;
};

class IWiFiAccessPoint {
    public:
        virtual ~IWiFiAccessPoint() = default;

        virtual fic_error_t start(const char* ssid, const char* password) = 0;
        virtual void stop() = 0;
};

#endif 
