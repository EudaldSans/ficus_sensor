#include "sntp_client.hh"

#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include "fic_log.hh"

bool EspSntpClient::_synced = false;
bool EspSntpClient::_is_syncing = false;


fic_error_t EspSntpClient::sync_system_time() {
    if (_wifi_manager.get_state() != WiFiState::STA_CONNECTED) {
        return FIC_ERR_INVALID_STATE;
    }

    if (esp_sntp_enabled()) {
        FIC_LOGW(TAG, "SNTP already running, skipping init.");
        return FIC_OK; 
    }

    _is_syncing = true;

    FIC_LOGI(TAG, "Initializing SNTP using esp_netif_sntp");

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.smooth_sync = true;
    config.sync_cb = _sync_notification_cb;

    esp_err_t err = esp_netif_sntp_init(&config);
    if (err != ESP_OK) {
        FIC_LOGE(TAG, "Failed to init esp netif sntp");
        return FIC_ERR_SDK_FAIL;
    }

    err = esp_netif_sntp_start();
    if (err != ESP_OK) {
        FIC_LOGE(TAG, "Failed to start esp netif sntp");
        return FIC_ERR_SDK_FAIL;
    }

    return FIC_OK;
}

fic_error_t EspSntpClient::add_server(std::string_view server_name) {
    esp_sntp_setservername(0, server_name.data()); 
    return FIC_OK;
}

fic_error_t EspSntpClient::clear_servers() {
    esp_netif_sntp_deinit();
    return FIC_OK;
}

void EspSntpClient::_sync_notification_cb(struct timeval *tv) {
    struct tm timeinfo;

    settimeofday(tv, NULL);
    localtime_r(&tv->tv_sec, &timeinfo);

    _synced = true;
    _is_syncing = false;

    char strftime_buf[64];
    
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    FIC_LOGI(TAG, "Time synchronized: %s", strftime_buf);
}