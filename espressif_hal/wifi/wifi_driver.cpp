#include <string.h>

#include "wifi_driver.hh"

#include "esp_check.h"
#include "esp_mac.h"

#include "fic_log.hh"

/**
 * @brief Initializes the driver
 * 
 * @return @c fic_error_t with any error, @c FIC_OK if Driver correctly initialized
 */
fic_error_t WifiDriver::init() {
    if (_initialized) return FIC_OK;

    if (esp_netif_init() != ESP_OK) {
        FIC_LOGE(TAG, "Could not initialize netif"); 
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_event_loop_create_default() != ESP_OK) {
        FIC_LOGE(TAG, "Could not create event loop"); 
        return FIC_ERR_SDK_FAIL;
    }

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ret = nvs_flash_erase();
        if (ret == ESP_OK) ret = nvs_flash_init();
    }

    if (ret != ESP_OK) {
        FIC_LOGE(TAG, "Could not initialize nvs flash"); 
        return FIC_ERR_SDK_FAIL;
    }

    _wifi_event_group = xEventGroupCreate();

    if (esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, this, NULL) != ESP_OK) {
        FIC_LOGE(TAG, "Could not register instance WIFI_EVENT"); 
        return FIC_ERR_SDK_FAIL;
    }
    if (esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, this, NULL) != ESP_OK) {
        FIC_LOGE(TAG, "Could not register instance IP_EVENT"); 
        return FIC_ERR_SDK_FAIL;
    }
    
    _cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&_cfg) != ESP_OK) {
        FIC_LOGE(TAG, "Could not initialize wifi config"); 
        return FIC_ERR_SDK_FAIL;
    }

    FIC_LOGI(TAG, "WifiDriver initialized successfully");

    _state = WiFiState::IDLE;
    _initialized = true;

    return FIC_OK;
}

/**
 * @brief Deinitializes the driver
 * 
 * @return @c fic_error_t with any error, @c FIC_OK if Driver correctly deinitialized
 */
fic_error_t WifiDriver::deinit() {
    if (!_initialized) return FIC_OK;

    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);

    if (_wifi_event_group != NULL) {
        vEventGroupDelete(_wifi_event_group);
        _wifi_event_group = NULL;
    }

    esp_event_loop_delete_default();

    esp_netif_deinit();

    nvs_flash_deinit();

    _initialized = false;
    _state = WiFiState::OFF;
    FIC_LOGI(TAG, "WifiDriver deinitialized successfully");

    return FIC_OK;
}

/**
 * @brief Stops the current WiFi process
 */
void WifiDriver::stop() {
    FIC_LOGE(TAG, "NOT YET IMPLEMENTED");
    return;
}

/**
 * @brief Performs a scan of all access points detected by the device
 * 
 * @return @c fic_error_t with relevant errors, @c FIC_OK if scan started successfuly
 */
fic_error_t WifiDriver::start_scan() {
    FIC_LOGE(TAG, "NOT YET IMPLEMENTED");
    return FIC_ERR_NOT_FOUND;
}
/**
 * @brief Checks whether a scan is currently running
 * 
 * @return @c true if a scan is running, @c false otherwise
 */
bool WifiDriver::is_scan_busy() const {
    return _state != WiFiState::SCANNING;
}

/**
 * @brief Returns the last scan results.
 * 
 * @param results pointer to @c WiFiScanItem array
 * @param count pointer to @c size_t
 * @return @c fic_error_t with relevant errors
 */
fic_error_t WifiDriver::get_scan_results(WiFiScanItem* results, size_t* count) const {
    FIC_LOGE(TAG, "NOT YET IMPLEMENTED");
    return FIC_ERR_NOT_FOUND;
}

/**
 * @brief Returns the current Driver state
 * 
 * @return @c WiFiState 
 */
WiFiState WifiDriver::get_state() const {return _state;}  

/**
 * @brief returns the current connection details
 * 
 * @return @c ConnectionDetails
 */
ConnectionDetails WifiDriver::get_details() const {
    wifi_ap_record_t ap;
    ConnectionDetails details = {};

    esp_wifi_sta_get_ap_info(&ap);

    details.rssi = ap.rssi;
    memcpy(details.ssid, ap.ssid, sizeof(ap.ssid));

    return details;
}  

/**
 * @brief Starts driver process to conect to a WiFi access point
 * 
 * @param ssid The SSID of the access point
 * @param password The password of the access point
 * @return @c FIC_ERR_INVALID_STATE if the Driver is already connecting to an STA, or is set to AP mode
 */
fic_error_t WifiDriver::sta_connect(const char* ssid, const char* password) {
    if (_current_mode != InternalMode::NONE) {return FIC_ERR_INVALID_STATE;}

    wifi_config_t wifi_sta_config = {};

    strncpy((char*)wifi_sta_config.sta.ssid, ssid, sizeof(wifi_sta_config.sta.ssid) - 1);
    strncpy((char*)wifi_sta_config.sta.password, password, sizeof(wifi_sta_config.sta.password) - 1);

    wifi_sta_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    wifi_sta_config.sta.failure_retry_cnt = _max_retries;
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_sta_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_STA");
        return FIC_ERR_SDK_FAIL;
    }

    _esp_netif = esp_netif_create_default_wifi_sta();

    if (esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set STA config");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_netif_set_default_netif(_esp_netif)) {
        FIC_LOGE(TAG, "Failed to set default netif");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_wifi_start()) {
        FIC_LOGE(TAG, "Failed to start WiFi");
        return FIC_ERR_SDK_FAIL;
    }

    FIC_LOGI(TAG, "STA connecting");
    _state = WiFiState::STA_CONNECTING;
    _current_mode = InternalMode::STATION;

    return FIC_OK;
}

/**
 * @brief Starts a secure access point on the device.
 * 
 * @param ssid SSID of the access point
 * @param password Password for the access point (empty for non secure AP)
 * @param channel The channel to open the AP on
 * @param max_connection Maxumun number of connection to the AP
 * @return @c fic_error_t with relevant errors, @c FIC_OK if AP started successfuly
 */
fic_error_t WifiDriver::start_ap(const char* ssid, const char* password, uint8_t channel, uint8_t max_connection) {
    if (_current_mode != InternalMode::NONE) {return FIC_ERR_INVALID_STATE;}

    wifi_config_t wifi_ap_config = {};

    size_t ssid_len = strlen(ssid);
    if (ssid_len >= sizeof(wifi_ap_config.ap.ssid)) {
        ssid_len = sizeof(wifi_ap_config.ap.ssid) - 1; 
    }
    memcpy(wifi_ap_config.ap.ssid, ssid, ssid_len);
    wifi_ap_config.ap.ssid_len = ssid_len;

    strncpy((char*)wifi_ap_config.ap.password, password, sizeof(wifi_ap_config.ap.password) - 1);

    wifi_ap_config.ap.channel = channel;
    wifi_ap_config.ap.max_connection = max_connection;
    wifi_ap_config.ap.authmode = (strlen(password) == 0) ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    wifi_ap_config.ap.pmf_cfg.required = false;

    if (esp_wifi_set_mode(WIFI_MODE_AP) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_AP");
        return FIC_ERR_SDK_FAIL;
    }

    _esp_netif = esp_netif_create_default_wifi_ap();

    if (esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config) != ESP_OK) {
        FIC_LOGE(TAG, "Failed to set mode to WIFI_MODE_AP");
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_wifi_start()) {
        FIC_LOGE(TAG, "Failed to start WiFi");
        return FIC_ERR_SDK_FAIL;
    }

    FIC_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", ssid, password, channel);

    _current_mode = InternalMode::ACCESS_POINT;
    _state = WiFiState::AP_ACTIVE;

    return FIC_OK;
}

/**
 * @brief Event handle for ESP IDF to dump all event information
 * 
 * @param instance Instance to @c WifiDriver
 * @param event_base The type of event
 * @param event_id The event identifier
 * @param event_data Pointer to data relevant to the event
 */
void WifiDriver::wifi_event_handler(void *instance, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    auto* self = static_cast<WifiDriver*>(instance);
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        FIC_LOGI(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
        FIC_LOGI(TAG, "Station " MACSTR " left, AID=%d, reason:%d", MAC2STR(event->mac), event->aid, event->reason);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        FIC_LOGI(TAG, "Station started");
        self->_state = WiFiState::STA_CONNECTING;

    }else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (self->_retries < self->_max_retries) {
            esp_wifi_connect();
            self->_retries++;
            FIC_LOGW(TAG, "retrying to connect to the AP");
        } else {
            FIC_LOGE(TAG,"failed to connect to AP , reverting to OFF");
            // xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        FIC_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        self->_retries = 0;

        self->_state = WiFiState::STA_CONNECTED;
    }
}