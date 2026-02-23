#include "analog_humidity_sensor.hh"
#include "esp_log.h"

AnalogHumiditySensor::AnalogHumiditySensor(IADC &adc, uint32_t max_voltage_mv) : 
        ISensor<float>(), _adc(adc), _max_voltage_mv(max_voltage_mv) {
}

AnalogHumiditySensor::~AnalogHumiditySensor() {}

fic_error_t AnalogHumiditySensor::init() {
    return _adc.init();
}

fic_error_t AnalogHumiditySensor::deinit() { 
    return FIC_OK; 
}

fic_error_t AnalogHumiditySensor::measure(float &value) {
    int voltage;
    FIC_RETURN_ON_ERROR(_adc.measure(voltage), ESP_LOGE(TAG, "Failed to measure ADC voltage"));
    float humidity = 100 - 100 * static_cast<float>(voltage) / static_cast<float>(_max_voltage_mv);

    if (humidity < 0) {
        humidity = 0;
    }

    value = humidity;
    return FIC_OK;
}