#ifndef SNTP_CLIENT_HH
#define SNTP_CLIENT_HH

#include "sntp_hal.hh"

class EspSntpClient : public ISntpClient {
public:
    EspSntpClient(IWiFiStatusManager& wifi_manager) : _wifi_manager(wifi_manager) {}
    ~EspSntpClient() = default;

    fic_error_t sync_system_time() override;

    bool is_synced() const override { return _synced; }
    bool is_syncing() const override { return _is_syncing; }

    fic_error_t add_server(std::string_view server_name) override;
    fic_error_t clear_servers() override;

private:
    static bool _synced;
    static bool _is_syncing;

    static void _sync_notification_cb(struct timeval *tv);
    static EspSntpClient* _instance;

    IWiFiStatusManager& _wifi_manager;

    constexpr static char const *TAG = "EspSntpClient";
};


#endif