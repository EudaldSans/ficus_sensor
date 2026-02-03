#include "sensor_endpoint.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

SensorEndpoint::SensorEndpoint(std::shared_ptr<ISensor> sensor, uint16_t measurement_period_ms) {
    _measurement_period_ms = measurement_period_ms;
    _sensor = sensor;
    _measurement_output = add_output_channel<float>(_sensor->sensor_name);
}

void SensorEndpoint::sensor_tic() {
    float temperature;
    uint16_t measurement_delay_ms = 0;

    _sensor->trigger_measurement(measurement_delay_ms);
    vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
    temperature = _sensor->get_last_measurement();

    _measurement_output->emit(temperature);

    measurement_delay_ms = _measurement_period_ms - measurement_delay_ms;
    if (measurement_delay_ms > 0) {
        vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
    }
}