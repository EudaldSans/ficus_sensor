#ifndef WIFI_HAL_HH
#define WIFI_HAL_HH

#include <inttypes.h>
#include <memory>

#include "fic_errors.hh"

struct ConnectionDetails {
    uint32_t addr;
    char ssid[33];
    int8_t rssi;
};

struct WiFiScanItem {
    char ssid[33];
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

    AP_STARTING,
    AP_ACTIVE,
    AP_STOPPING,  
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
        IWiFiScanner() = default;
        virtual ~IWiFiScanner() = default;

        IWiFiScanner(const IWiFiScanner&) = delete;            
        IWiFiScanner& operator=(const IWiFiScanner&) = delete; 
        IWiFiScanner(IWiFiScanner&&) = delete;      

        virtual fic_error_t start_scan() = 0; 

        virtual bool is_scan_busy() const = 0;
        virtual fic_error_t get_scan_results(WiFiScanItem* results, size_t &max_results) const = 0;
};

class IWiFiStatusManager {
    public:
        IWiFiStatusManager() = default;
        virtual ~IWiFiStatusManager() = default;

        IWiFiStatusManager(const IWiFiStatusManager&) = delete;            
        IWiFiStatusManager& operator=(const IWiFiStatusManager&) = delete; 
        IWiFiStatusManager(IWiFiStatusManager&&) = delete;

        virtual WiFiState get_state() const = 0;  
        virtual ConnectionDetails get_details() const = 0;    
};

class IWiFiLifecycle {
    public:
        IWiFiLifecycle() = default;
        virtual ~IWiFiLifecycle() = default;

        IWiFiLifecycle(const IWiFiLifecycle&) = delete;            
        IWiFiLifecycle& operator=(const IWiFiLifecycle&) = delete; 
        IWiFiLifecycle(IWiFiLifecycle&&) = delete;

        virtual fic_error_t init() = 0;
        virtual fic_error_t deinit() = 0;

        virtual void stop() = 0;
};

class IWiFiStation {
    public:
        IWiFiStation() = default;
        virtual ~IWiFiStation() = default;

        IWiFiStation(const IWiFiStation&) = delete;            
        IWiFiStation& operator=(const IWiFiStation&) = delete; 
        IWiFiStation(IWiFiStation&&) = delete;

        virtual fic_error_t sta_connect(const char* ssid, const char* password, uint16_t retries) = 0;
        virtual fic_error_t sta_disconnect() = 0;
};

class IWiFiAccessPoint {
    public:
        IWiFiAccessPoint() = default;
        virtual ~IWiFiAccessPoint() = default;

        IWiFiAccessPoint(const IWiFiAccessPoint&) = delete;            
        IWiFiAccessPoint& operator=(const IWiFiAccessPoint&) = delete; 
        IWiFiAccessPoint(IWiFiAccessPoint&&) = delete;

        virtual fic_error_t start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connections) = 0;
        virtual fic_error_t stop_ap() = 0;
};

#endif 
