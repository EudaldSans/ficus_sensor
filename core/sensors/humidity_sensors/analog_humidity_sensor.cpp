#include "analog_humidity_sensor.hh"
#include "esp_log.h"
#include "esp_check.h"


AnalogHumiditySensor::AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t _max_voltage_mv) : 
        ISensor(), _adc(adc) {
    
    _max_voltage_mv = _max_voltage_mv;
}

AnalogHumiditySensor::~AnalogHumiditySensor() {}

esp_err_t AnalogHumiditySensor::init() {
    return _adc->init();
}

esp_err_t AnalogHumiditySensor::trigger_measurement(uint16_t &measurement_delay_ms) {
    measurement_delay_ms = 0;

    return ESP_OK;
}

float AnalogHumiditySensor::get_last_measurement() {
    int voltage;
    _adc->measure(voltage);
    float humidity = 100 - 100 * voltage/3300;

    if (humidity < 0) {
        humidity = 0;
    }

    return humidity;
}

const char* AnalogHumiditySensor::get_name() {
    return "Analog Humidity Sensor";
}