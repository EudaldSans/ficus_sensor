#include <string>
#include <cstring>

#include "wifi_access_point.hh"
#include "fic_log.hh"

/**
 * @brief Starts a secure access point on the device.
 * 
 * @param ssid SSID of the access point
 * @param password Password for the access point (empty for non secure AP)
 * @param channel The channel to open the AP on
 * @param max_connections Maxumun number of connection to the AP
 * @return @c fic_error_t with relevant errors, @c FIC_OK if AP started successfuly
 */
fic_error_t WiFiAccessPoint::start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connections) {
    auto context = _ctx.lock();
    if (context.mode() != InternalMode::NONE) {return FIC_ERR_INVALID_STATE;}

    wifi_config_t wifi_ap_config = {};

    size_t ssid_len = strlen(ssid);
    if (ssid_len >= sizeof(wifi_ap_config.ap.ssid)) {
        ssid_len = sizeof(wifi_ap_config.ap.ssid) - 1; 
    }
    memcpy(wifi_ap_config.ap.ssid, ssid, ssid_len);
    wifi_ap_config.ap.ssid_len = ssid_len;

    strncpy((char*)wifi_ap_config.ap.password, password, sizeof(wifi_ap_config.ap.password) - 1);

    wifi_ap_config.ap.channel = channel;
    wifi_ap_config.ap.max_connection = max_connections;
    wifi_ap_config.ap.authmode = (strlen(password) == 0) ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    wifi_ap_config.ap.pmf_cfg.required = false;

    if (esp_wifi_set_mode(WIFI_MODE_AP) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_AP");
        return FIC_ERR_SDK_FAIL;
    }

    context.set_netif(esp_netif_create_default_wifi_ap());

    if (esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_AP");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_wifi_start()) {
        FIC_LOGE(TAG, "Failed to start WiFi AP");
        return FIC_ERR_SDK_FAIL;
    }

    FIC_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", ssid, password, channel);
    
    context.set_mode(InternalMode::ACCESS_POINT);
    context.set_state(WiFiState::AP_ACTIVE);

    return FIC_OK;
}

/**
 * @brief Stops the AP if it is active
 * 
 * @return @c fic_error_t with relevant errors, @c FIC_OK if AP stopped successfuly
 */
fic_error_t WiFiAccessPoint::stop_ap() {
    auto context = _ctx.lock();
    if (context.mode() != InternalMode::ACCESS_POINT) {return FIC_ERR_INVALID_STATE;}

    context.set_state(WiFiState::AP_STOPPING);
    esp_wifi_stop();

    return FIC_OK;
}