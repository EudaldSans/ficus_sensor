#include "analog_humidity_sensor.hh"
#include "esp_log.h"
#include "esp_check.h"


AnalogHumiditySensor::AnalogHumiditySensor(uint32_t max_voltage_mv, adc_channel_t channel, adc_unit_t unit) : 
        ISensor(), 
        adc(channel, unit, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT) {
    this->max_voltage_mv = max_voltage_mv;
}

AnalogHumiditySensor::~AnalogHumiditySensor() {}

esp_err_t AnalogHumiditySensor::init() {
    return adc.init();
}

esp_err_t AnalogHumiditySensor::trigger_measurement(uint16_t &measurement_delay_ms) {
    measurement_delay_ms = 0;

    return ESP_OK;
}

float AnalogHumiditySensor::get_last_measurement() {
    int voltage;
    adc.measure(voltage);
    float humidity = 100 - 100 * voltage/3300;

    if (humidity < 0) {
        humidity = 0;
    }

    return humidity;
}

const char* AnalogHumiditySensor::get_name() {
    return "Analog Humidity Sensor";
}