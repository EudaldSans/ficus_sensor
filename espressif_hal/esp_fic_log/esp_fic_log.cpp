#include "esp_fic_log.hh"
#include "esp_log.h"

/**
 * @brief Log backend for espressif devices, serves the pipe between fic_log and esp_log
 * 
 * @param level @c log_level_t with the level for the log message
 * @param tag pointer to const char with the tag for the log message
 * @param format Log format string
 * @param args Variable argument list
 */
void esp32_backend(log_level_t level, const char* tag, const char* format, va_list args) {
    esp_log_level_t esp_level = ESP_LOG_INFO; // Map your enum here
    switch (level) {
        case FIC_NONE:
            esp_level = ESP_LOG_NONE;
            break;
        case FIC_VERBOSE:
            esp_level = ESP_LOG_VERBOSE;
            break;
        case FIC_DEBUG:
            esp_level = ESP_LOG_DEBUG;
            break;
        case FIC_INFO:
            esp_level = ESP_LOG_INFO;
            break;
        case FIC_WARNING:
            esp_level = ESP_LOG_WARN;
            break;
        case FIC_ERROR:
            esp_level = ESP_LOG_ERROR;
            break;
    }
    
    esp_log_config_t config = ESP_LOG_CONFIG_INIT((esp_level) | ESP_LOG_CONFIGS_DEFAULT);

    esp_log_va(config, tag, format, args);
    // esp_log_writev(esp_level, tag, format, args); 
}