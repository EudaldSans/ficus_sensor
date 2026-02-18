#include "analog_humidity_sensor.hh"
#include "esp_log.h"

AnalogHumiditySensor::AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t max_voltage_mv) : 
        ISensor<float>(), _adc(adc), _max_voltage_mv(max_voltage_mv) {
}

AnalogHumiditySensor::~AnalogHumiditySensor() {}

fic_error_t AnalogHumiditySensor::init() {
    return _adc->init();
}

fic_error_t AnalogHumiditySensor::deinit() { 
    return FIC_OK; 
}

fic_error_t AnalogHumiditySensor::measure(float &value) {
    int voltage;
    _adc->measure(voltage);
    float humidity = 100 - 100 * voltage/_max_voltage_mv;

    if (humidity < 0) {
        humidity = 0;
    }

    value = humidity;
    return FIC_OK;
}