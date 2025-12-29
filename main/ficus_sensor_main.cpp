/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"

#define ONEWIRE_BUS_GPIO        18

static const char *TAG = "example";

extern "C" void app_main(void) {
    const uint32_t humidity_sensor_max_voltage_mv = 3300;

    DS18B20 temp_sensor = DS18B20(ONEWIRE_BUS_GPIO);
    temp_sensor.init();
    temp_sensor.set_resolution(DS18B20::resolution_12B);

    AnalogHumiditySensor humidity_sensor = AnalogHumiditySensor(humidity_sensor_max_voltage_mv, ADC_CHANNEL_2, ADC_UNIT_1);
    humidity_sensor.init();

    uint16_t measurement_delay_ms = 0;
    float temperature, humidity;
    while (1) {
        humidity = humidity_sensor.get_last_measurement();
        ESP_LOGI(TAG, "Humidity: %f%%", humidity);

        temp_sensor.trigger_measurement(measurement_delay_ms);
        vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
        temperature = temp_sensor.get_last_measurement();

        ESP_LOGI(TAG, "Temperature: %fºC", temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}