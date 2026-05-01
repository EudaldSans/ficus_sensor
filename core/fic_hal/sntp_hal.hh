#ifndef SNTP_PROVIDER_HAL_HH
#define SNTP_PROVIDER_HAL_HH

#include <string>

#include "fic_errors.hh"

#include "wifi_hal.hh"

class ISntpClient {
public:
    virtual ~ISntpClient() = default;

    virtual fic_error_t sync_system_time() = 0;

    virtual fic_error_t add_server(std::string_view server_name) = 0;
    virtual fic_error_t clear_servers() = 0;

    virtual bool is_synced() const = 0;
    virtual bool is_syncing() const = 0;
};


#endif