/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "task_manager.hh"

#include "router.hh"
#include "endpoint.hh"
#include "conversions.hh"

#include "sensor_endpoints.hh"
#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"

#include "onewire_bus.hh"
#include "adc.hh"

#include "led_strip_single.hh"
#include "rgb_signalling.hh"

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

LEDStripSingle led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);
RGBSignaler rgb_signaler(led_strip);

const uint16_t sensor_meas_period_ms = 30000;

const std::string t_sensor_config_name = "ds18b20_0";
OnewireBus onewire(ONEWIRE_BUS_GPIO);
DS18B20 t_sensor = DS18B20(onewire, DS18B20::resolution_12B);
AsyncSensorEndpoint<float> t_endpoint(
    t_sensor_config_name,
    t_sensor,
    sensor_meas_period_ms
);

const std::string h_sensor_config_name = "analog_humidity_0";
const uint32_t h_sensor_max_mv = 3300;
ADC adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
AnalogHumiditySensor h_sensor = AnalogHumiditySensor(adc, h_sensor_max_mv);
SensorEndpoint<float> h_endpoint(
    h_sensor_config_name,
    h_sensor,
    sensor_meas_period_ms
);

static const char *TAG = "main";

extern "C" void app_main(void) {  
    led_strip.init();

    TaskManager task_manager = TaskManager("main_task_manager", 4096, tskNO_AFFINITY);
    task_manager.add_task(&t_endpoint);
    task_manager.add_task(&h_endpoint);

    task_manager.add_task(&rgb_signaler);
    
    std::vector<std::shared_ptr<IConversion<float>>> conversions;
    // conversions.push_back(std::make_shared<ToFahrenheitConversion<float>>());

    ESP_LOGI(TAG, "Size of conversions: %d", conversions.size());

    auto consumer = ChannelEndpoint();

    std::string t_consumer = "temperature_consumer"; 
    std::string h_consumer = "humidity_consumer";

    consumer.add_input_channel<int>(t_consumer, 
        [](const int& temperature) {
            ESP_LOGI(TAG, "t_consumer got %d", temperature);
        }
    );
    consumer.add_input_channel<int>(h_consumer, 
        [](const int& humidity) {
            ESP_LOGI(TAG, "h_consumer got %d", humidity);
        }
    );

    Router::link<float, int>(t_endpoint, t_sensor_config_name, consumer, t_consumer, conversions);
    Router::link<float, int>(h_endpoint, h_sensor_config_name, consumer, h_consumer, conversions);

    task_manager.start();

    std::array<RGB_action_t, MAX_STEPS_IN_RGB_SIGNAL> sos_pattern = {{
        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 300},

        {true, {50, 50, 50}, 300},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 300},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 300},
        {false, {0, 0, 0}, 300},

        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 100},
        {true, {50, 50, 50}, 100},
        {false, {0, 0, 0}, 700},
    }};

    while (1) {
        rgb_signaler.set_blink({255, 0, 0}, 500, {0, 0, 255}, 500, INFINITE_CYCLES);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        rgb_signaler.set_blink({0, 0, 0}, 500, {255, 255, 255}, 500, INFINITE_CYCLES);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        rgb_signaler.set_solid({0, 0, 0});
        vTaskDelay(2000/portTICK_PERIOD_MS);
        rgb_signaler.set_solid({0, 255, 0});
        vTaskDelay(2000/portTICK_PERIOD_MS);

        rgb_signaler.set_custom_signal({sos_pattern.begin(), sos_pattern.end()}, INFINITE_CYCLES);
        vTaskDelay(20000/portTICK_PERIOD_MS);
    }
}