#include "sensor.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Sensor::Sensor(const std::string output_name) {
    measurement_output = add_output_channel<float>(output_name);
}

void Sensor::sensor_tic() {
    float temperature;
    uint16_t measurement_delay_ms = 0;

    trigger_measurement(measurement_delay_ms);
    vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
    temperature = get_last_measurement();

    measurement_output->emit(temperature);
}