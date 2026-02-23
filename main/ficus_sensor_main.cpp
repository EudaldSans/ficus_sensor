/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "sensor_endpoint_factory.hh"

#include "task_manager.hh"

#include "router.hh"
#include "endpoint.hh"
#include "conversions.hh"

#include "onewire_bus.hh"
#include "adc.hh"

#include "led_strip_single.hh"
#include "rgb_signalling.hh"

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

LEDStripSingle led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);
RGBSignaler rgb_signaler(led_strip);

OnewireBus onewire(ONEWIRE_BUS_GPIO);
AsyncSensorEndpoint<float> t_endpoint(
    "ds18b20_0",
    std::make_shared<DS18B20>(onewire, DS18B20::resolution_12B),
    30000
);

ADC adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
SensorEndpoint<float> h_endpoint(
    "analog_humidity_0",
    std::make_shared<AnalogHumiditySensor>(adc, 3300),
    20000
);

static const char *TAG = "example";

typedef struct {
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
} temp_point;

extern "C" void app_main(void) {
    const uint32_t h_sensor_max_mv = 3300;

    auto onewire = std::make_shared<OnewireBus>(ONEWIRE_BUS_GPIO);
    auto adc = std::make_shared<ADC>(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
    
    led_strip.init();

    TaskManager task_manager = TaskManager("main_task_manager", 4096, tskNO_AFFINITY);
    task_manager.add_task(&t_endpoint);
    task_manager.add_task(&h_endpoint);

    task_manager.add_task(&rgb_signaler);
    
    std::vector<std::shared_ptr<IConversion<float>>> conversions;
    // conversions.push_back(std::make_shared<ToFahrenheitConversion<float>>());

    ESP_LOGI(TAG, "Size of conversions: %d", conversions.size());

    auto consumer = std::make_shared<ChannelEndpoint>();

    std::string t_consumer = "temperature_consumer"; 
    std::string h_consumer = "humidity_consumer";

    consumer->add_input_channel<int>(t_consumer, 
        [](const int& temperature) {
            ESP_LOGI(TAG, "t_consumer got %d", temperature);
            set_heat_led(temperature);
        }
    );
    consumer->add_input_channel<int>(h_consumer, 
        [](const int& humidity) {
            ESP_LOGI(TAG, "h_consumer got %d", humidity);
        }
    );

    Router::link<float, int>(t_endpoint, t_sensor_config.name, consumer, t_consumer, conversions);
    Router::link<float, int>(h_endpoint, h_sensor_config.name, consumer, h_consumer, conversions);

    task_manager.start();

    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}