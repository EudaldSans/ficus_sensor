#include "sensor_endpoint.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

SensorEndpoint::SensorEndpoint(std::shared_ptr<ISensor> sensor, uint16_t measurement_period_ms) : _sensor(sensor) {
    _measurement_period_ms = measurement_period_ms;
    _measurement_output = add_output_channel<float>(_sensor->sensor_name);
}

void SensorEndpoint::trigger_measurement() {
    uint16_t measurement_delay_ms = 0;
    uint32_t now = pdTICKS_TO_MS(xTaskGetTickCount());
    
    _sensor->trigger_measurement(measurement_delay_ms);

    _next_measurement_time_ms = now + measurement_delay_ms;
}

void SensorEndpoint::sensor_tic() {
    float value;

    uint32_t now = pdTICKS_TO_MS(xTaskGetTickCount());
    if (now < _next_measurement_time_ms) return;
    
    value = _sensor->get_last_measurement();

    _measurement_output->emit(value);

    trigger_measurement();
}

void SensorEndpoint::setup() {
    _sensor->init();

    trigger_measurement();
}

void SensorEndpoint::update() {
    sensor_tic();
}