#include "analog_humidity_sensor.hh"
#include "esp_log.h"

AnalogHumiditySensor::AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t max_voltage_mv) : 
        ISensor(), _adc(adc), _max_voltage_mv(max_voltage_mv) {
}

AnalogHumiditySensor::~AnalogHumiditySensor() {}

in_error_t AnalogHumiditySensor::init() {
    return _adc->init();
}

in_error_t AnalogHumiditySensor::deinit() { 
    return IN_OK; 
}

in_error_t AnalogHumiditySensor::trigger_measurement(uint16_t &measurement_delay_ms) {
    measurement_delay_ms = 0;

    return IN_OK;
}

in_error_t AnalogHumiditySensor::measure(float &value) {
    int voltage;
    _adc->measure(voltage);
    float humidity = 100 - 100 * voltage/3300;

    if (humidity < 0) {
        humidity = 0;
    }

    value = humidity;
    return IN_OK;
}