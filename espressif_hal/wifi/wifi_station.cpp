#include <string>
#include <cstring>

#include "wifi_station.hh"
#include "fic_log.hh"

/**
 * @brief Starts driver process to conect to a WiFi access point
 * 
 * @param ssid The SSID of the access point
 * @param password The password of the access point
 * @param retries Number of times to retry connecting to the AP before giving up and setting the state to @c ERROR_AUTH_FAILED
 * @return @c FIC_ERR_INVALID_STATE if the Driver is already connecting to an STA, or is set to AP mode
 */
fic_error_t WiFiStation::sta_connect(const char* ssid, const char* password, uint16_t retries) {
    auto context = _ctx.lock();
    if (context.mode() != InternalMode::NONE) {return FIC_ERR_INVALID_STATE;}

    wifi_config_t wifi_sta_config = {};

    strncpy((char*)wifi_sta_config.sta.ssid, ssid, sizeof(wifi_sta_config.sta.ssid) - 1);
    strncpy((char*)wifi_sta_config.sta.password, password, sizeof(wifi_sta_config.sta.password) - 1);

    wifi_sta_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    wifi_sta_config.sta.failure_retry_cnt = retries;
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_sta_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_STA");
        return FIC_ERR_SDK_FAIL;
    }

    context.set_netif(esp_netif_create_default_wifi_sta);

    if (esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set STA config");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_netif_set_default_netif(context.netif())) {
        FIC_LOGE(TAG, "Failed to set default netif");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_wifi_start()) {
        FIC_LOGE(TAG, "Failed to start WiFi station");
        return FIC_ERR_SDK_FAIL;
    }

    FIC_LOGI(TAG, "STA connecting");
    context.set_state(WiFiState::STA_CONNECTING);
    context.set_mode(InternalMode::STATION);

    return FIC_OK;
}

/**
 * @brief Stops the AP if it is active
 * 
 * @return @c fic_error_t with relevant errors, @c FIC_OK if AP stopped successfuly
 */
fic_error_t WiFiStation::sta_disconnect() {
    auto context = _ctx.lock();
    if (context.mode() != InternalMode::STATION) {return FIC_ERR_INVALID_STATE;}

    context.set_state(WiFiState::STA_DISCONNECTING);
    esp_wifi_stop();

    return FIC_OK;
}