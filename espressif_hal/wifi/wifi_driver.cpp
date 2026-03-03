#include <string.h>
#include <algorithm>

#include "wifi_driver.hh"

#include "esp_check.h"
#include "esp_mac.h"

#include "fic_log.hh"

/**
 * @brief Initializes the controller
 * 
 * @return @c fic_error_t with any error, @c FIC_OK if Controller correctly initialized
 */
fic_error_t WiFiController::init() {
    if (_initialized) return FIC_OK;

    if (esp_netif_init() != ESP_OK) {
        FIC_LOGE(TAG, "Could not initialize netif"); 
        return FIC_ERR_SDK_FAIL;
    }

    if (esp_event_loop_create_default() != ESP_OK) {
        FIC_LOGE(TAG, "Could not create event loop"); 
        return FIC_ERR_SDK_FAIL;
    }

    if (_event_group != NULL) {
        vEventGroupDelete(_event_group);
        _event_group = NULL;
    }
    _event_group = xEventGroupCreate();

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

    auto context = _ctx.lock();
    context.set_mode(InternalMode::NONE);
    context.set_state(WiFiState::IDLE);

    _initialized = true;

    FIC_LOGI(TAG, "WifiDriver initialized successfully");

    return FIC_OK;
}

/**
 * @brief Deinitializes the driver
 * 
 * @return @c fic_error_t with any error, @c FIC_OK if Driver correctly deinitialized
 */
fic_error_t WiFiController::deinit() {
    if (!_initialized) return FIC_OK;

    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);

    if (_event_group != NULL) {
        vEventGroupDelete(_event_group);
        _event_group = NULL;
    }

    esp_event_loop_delete_default();

    auto context = _ctx.lock();
    context.delete_netif();

    esp_netif_deinit();

    auto context = _ctx.lock();
    context.set_mode(InternalMode::NONE);
    context.set_state(WiFiState::IDLE);
    FIC_LOGI(TAG, "WifiDriver deinitialized successfully");

    return FIC_OK;
}

/**
 * @brief Stops the current WiFi process
 */
void WiFiController::stop() {
    if (esp_wifi_stop() != ESP_OK) {
        FIC_LOGE(TAG, "Failed to stop WiFi station");
        return;
    }

    auto context = _ctx.lock();
    context.delete_netif();
    context.set_mode(InternalMode::NONE);
    context.set_state(WiFiState::IDLE);

    FIC_LOGI(TAG, "WiFi stopped successfully");
}

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
 * @brief Returns the current Driver state
 * 
 * @return @c WiFiState 
 */
WiFiState WiFiController::get_state() const {
    auto context = _ctx.lock();
    return context.state();
}  

/**
 * @brief returns the current connection details
 * 
 * @return @c ConnectionDetails
 */
ConnectionDetails WiFiController::get_details() const {
    wifi_ap_record_t ap;
    ConnectionDetails details = {};

    esp_wifi_sta_get_ap_info(&ap);

    details.rssi = ap.rssi;
    details.addr = _current_ip;
    memcpy(details.ssid, ap.ssid, sizeof(ap.ssid));

    return details;
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

    context.set_netif(esp_netif_create_default_wifi_sta());

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

/**
 * @brief Event handle for ESP IDF to dump all event information
 * 
 * @param instance Instance to @c WiFiController
 * @param event_base The type of event
 * @param event_id The event identifier
 * @param event_data Pointer to data relevant to the event
 */
void WiFiController::wifi_event_handler(void *instance, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    auto* self = static_cast<WiFiController*>(instance);

    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
            FIC_LOGI(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);

        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
            FIC_LOGI(TAG, "Station " MACSTR " left, AID=%d, reason:%d", MAC2STR(event->mac), event->aid, event->reason);

        } else if (event_id == WIFI_EVENT_STA_START) {
            auto context = self->_ctx.lock();

            if (context.mode() != InternalMode::SCANNING) esp_wifi_connect();
            
            FIC_LOGI(TAG, "Station started");
            context.set_state(WiFiState::STA_CONNECTING);

        }else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            auto context = self->_ctx.lock();

            if (context.state() == WiFiState::STA_DISCONNECTING) {
                FIC_LOGI(TAG, "Station disconnected");
                context.set_state(WiFiState::IDLE);
                context.set_mode(InternalMode::NONE);
                return;
            }

            esp_wifi_connect();
            self->_retries++;
            FIC_LOGW(TAG, "retrying to connect to the AP");
        
        } else if (event_id == WIFI_EVENT_STA_STOP) {
            FIC_LOGI(TAG, "Station stopped");
            context.set_mode(InternalMode::NONE);
            context.set_state(WiFiState::IDLE);

        } else if (event_id == WIFI_EVENT_SCAN_DONE) {
            FIC_LOGI(TAG, "Scan done");

            auto context = self->_ctx.lock();
            context.set_mode(InternalMode::NONE);
            context.set_state(WiFiState::IDLE);
        }

    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
            FIC_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
            self->_retries = 0;
            self->_current_ip = event->ip_info.ip.addr;

            auto context = self->_ctx.lock();
            context.set_state(WiFiState::STA_CONNECTED);
        }
    }
}