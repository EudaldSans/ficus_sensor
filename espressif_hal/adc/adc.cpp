#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_check.h"

#include "adc.hh"

ADC::ADC(adc_channel_t channel, adc_unit_t unit, adc_atten_t attenuation, adc_bitwidth_t bitwidth) {
    this->channel = channel;
    this->unit = unit;
    this->attenuation = attenuation;
    this->bitwidth = bitwidth;
}

fic_error_t ADC::init() {
    ESP_LOGI(TAG, "Setting up ADC channel %d", channel);
    if (_initialized) {
        ESP_LOGW(TAG, "Tried to initialize an already initialized ADC!");
        return FIC_ERR_NOT_ALLOWED;
    }

    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.unit_id = unit;

    adc_oneshot_chan_cfg_t adc_config = {};
    adc_config.atten = attenuation;
    adc_config.bitwidth = bitwidth;

    adc_cali_curve_fitting_config_t cali_config = {};
    cali_config.unit_id = unit;
    cali_config.chan = channel;
    cali_config.atten = attenuation;
    cali_config.bitwidth = bitwidth;

    if (adc_oneshot_new_unit(&init_config, &adc_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to create new adc unit"); 
        return FIC_ERR_SDK_FAIL;
    }

    if (adc_oneshot_config_channel(adc_handle, channel, &adc_config) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to config channel %d", channel); 
        return FIC_ERR_SDK_FAIL;
    }

    if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to Create calibration scheme", channel); 
        return FIC_ERR_SDK_FAIL;
    }

    _initialized = true;

    return FIC_OK;
}

ADC::~ADC() {
    ESP_LOGI(TAG, "Deregister Curve Fitting calibration scheme for ADC channel %d", channel);
    if (_initialized) adc_cali_delete_scheme_curve_fitting(adc_cali_handle);
}

fic_error_t ADC::measure(int &voltage_out) {
    int adc_raw[2][10];
    int voltage[2][10];

    if (adc_oneshot_read(adc_handle, channel, &adc_raw[0][0]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read raw voltage"); 
        return FIC_ERR_SDK_FAIL;
    }

    if (adc_cali_raw_to_voltage(adc_cali_handle, adc_raw[0][0], &voltage[0][0]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calibrate raw voltage"); 
        return FIC_ERR_SDK_FAIL;
    }

    ESP_LOGD(TAG, "ADC channel %d measurement: %dmV", channel, voltage[0][0]);
    voltage_out = voltage[0][0];

    return FIC_OK;
}

