#ifndef TASK_HAL_HH
#define TASK_HAL_HH

#include "fic_errors.hh"

enum WiFiEvents {
    WIFI_CONNECTED,
    WIFI_DISCONNECTED
};

class IWiFiAdapter {
    public:
        virtual ~IWiFiAdapter() = default;

        virtual fic_error_t connect_to(const char* ssid, const char* password) = 0;

        virtual fic_error_t create_ap_secure(const char* ssid, const char* password) = 0;

        virtual void disconnect() = 0;
        virtual void register_event_callback() = 0;
};

#endif 
