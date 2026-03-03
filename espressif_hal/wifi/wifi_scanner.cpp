#include <string.h>
#include <algorithm>

#include "wifi_scanner.hh"
#include "fic_log.hh"

/**
 * @brief Performs a scan of all access points detected by the device
 * 
 * @return @c fic_error_t with relevant errors, @c FIC_OK if scan started successfuly
 */
fic_error_t WiFiScanner::start_scan() {
    auto context = _ctx.lock();
    if (context.mode() != InternalMode::NONE) {return FIC_ERR_INVALID_STATE;}

    context.set_netif(esp_netif_create_default_wifi_sta());

    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_STA for scan");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_wifi_start()) {
        FIC_LOGE(TAG, "Failed to start WiFi scan");
        return FIC_ERR_SDK_FAIL;
    }

    esp_wifi_scan_start(NULL, false);

    context.set_state(WiFiState::SCANNING);
    context.set_mode(InternalMode::SCANNING);
    
    return FIC_OK;
}

/**
 * @brief Checks whether a scan is currently running
 * 
 * @return @c true if a scan is running, @c false otherwise
 */
bool WiFiScanner::is_scan_busy() const {
    auto context = _ctx.lock();
    return context.state() == WiFiState::SCANNING;
}

/**
 * @brief Returns the last scan results.
 * 
 * @param results pointer to @c WiFiScanItem array
 * @param[inout]  max_results As input param, it stores max AP number ap_records can hold.
 *                As output param, it receives the actual AP number this API returns.
 * @return @c fic_error_t with relevant errors
 */
fic_error_t WiFiScanner::get_scan_results(WiFiScanItem* results, size_t &max_results) const {
    uint16_t result_count = max_results;
    uint16_t ap_count = 0;

    if (esp_wifi_scan_get_ap_num(&ap_count) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to get scan AP num");
        return FIC_ERR_SDK_FAIL;
    }

    result_count = std::min((uint16_t)result_count, ap_count);
    auto ap_info = new wifi_ap_record_t[result_count];
    if (!ap_info) {
        FIC_LOGE(TAG, "Failed to allocate memory for scan results");
        return FIC_ERR_NO_MEM;
    }

    if (esp_wifi_scan_get_ap_records(&result_count, ap_info) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to get scan AP records");
        return FIC_ERR_SDK_FAIL;
    }

    max_results = result_count;

    for (int i = 0; i < max_results; i++) {
        results[i].rssi = ap_info[i].rssi;
        results[i].channel = ap_info[i].primary;
        results[i].secure = ap_info[i].authmode != WIFI_AUTH_OPEN;

        memcpy(results[i].ssid, ap_info[i].ssid, sizeof(ap_info[i].ssid));
    }

    delete[] ap_info;

    return FIC_OK;
}

