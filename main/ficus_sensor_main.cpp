/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc.hh"
#include "ds18b20.hh"

#define ONEWIRE_BUS_GPIO        18

static const char *TAG = "example";

extern "C" void app_main(void) {
    ADC adc = ADC(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
    adc.init();

    DS18B20 temp_sensor = DS18B20(ONEWIRE_BUS_GPIO);
    temp_sensor.init();
    temp_sensor.set_resolution(DS18B20::resolution_12B);

    uint16_t measurement_delay_ms = 0;
    float temperature, humidity;
    int voltage;
    while (1) {
        adc.measure(voltage);
        humidity = 100 - 100*voltage/3300;
        if (humidity < 0) {
            humidity = 0;
        }

        ESP_LOGI(TAG, "Humidity: %f%%", humidity);

        temp_sensor.trigger_measurement(measurement_delay_ms);
        vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
        temperature = temp_sensor.get_last_measurement();

        ESP_LOGI(TAG, "Temperature: %fºC", temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}