/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "onewire_bus.h"
#include "ds18b20.h"

#include "adc.hh"
#include "onewire_bus.hh"

#define ONEWIRE_BUS_GPIO        18
#define ONEWIRE_MAX_RX_BYTES    10

#define DS18B20_ADDRESS         0x28
#define DS18B20_MASK            0xFF

static const char *TAG = "example";

extern "C" void app_main(void) {
    ADC adc = ADC(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
    adc.init();

    OnewireBus bus = OnewireBus(ONEWIRE_BUS_GPIO, ONEWIRE_MAX_RX_BYTES);
    bus.init();

    bus.find_device(DS18B20_ADDRESS, DS18B20_MASK);

    float temperature, humidity;
    int voltage;
    while (1) {
        adc.measure(voltage);
        humidity = 100 - 100*voltage/3300;
        if (humidity < 0) {
            humidity = 0;
        }

        ESP_LOGI(TAG, "Humidity: %f%%", humidity);

        // trigger temperature conversion for all sensors on the bus
        // ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion_for_all(bus));
        // for (int i = 0; i < ds18b20_device_num; i ++) {
        //     ESP_ERROR_CHECK(ds18b20_get_temperature(ds18b20s[i], &temperature));
        //     ESP_LOGI(TAG, "temperature read from DS18B20[%d]: %.2fC", i, temperature);
        // }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}