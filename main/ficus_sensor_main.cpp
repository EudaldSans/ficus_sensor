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

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

LEDStripSingle led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);

static const char *TAG = "example";

typedef struct {
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
} temp_point;

void set_heat_led(float value) {
    float min = 16;
    float max = 32;

    uint32_t red = 0, green = 0, blue = 0;
    float point = 8 * (value - min) / (max - min);

    if (point < 0 ) point = 0;
    if (point > 7) point = 7;

    int point_low = point;
    int point_high = point + 1;

    temp_point temperature_points[9];
    temperature_points[0] = {204, 216, 253};
    temperature_points[1] = {242, 241, 254};
    temperature_points[2] = {254, 229, 207};
    temperature_points[3] = {254, 199, 140};
    temperature_points[4] = {254, 164, 73};
    temperature_points[5] = {254, 123, 0};
    temperature_points[6] = {253, 74, 0};
    temperature_points[7] = {254, 0, 0};
    temperature_points[8] = {254, 0, 0};

    red = temperature_points[point_low].red * (1 - (point - point_low)) + temperature_points[point_high].red * (point - point_low);
    blue = temperature_points[point_low].blue * (1 - (point - point_low)) + temperature_points[point_high].blue * (point - point_low);
    green = temperature_points[point_low].green * (1 - (point - point_low)) + temperature_points[point_high].green * (point - point_low);

    led_strip.set_color({red, green, blue});
}

void blink(uint32_t time_ms, uint32_t red, uint32_t green, uint32_t blue) {
    led_strip.off();
    vTaskDelay(pdMS_TO_TICKS(time_ms));
    
    led_strip.set_color({red, green, blue});
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

extern "C" void app_main(void) {
    const uint32_t h_sensor_max_mv = 3300;

    auto onewire = std::make_shared<OnewireBus>(ONEWIRE_BUS_GPIO);
    auto adc = std::make_shared<ADC>(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);

    auto sensor_endpoint_factory = SensorEndpointFactoryBuilder()
        .with_onewire("onewire_0", onewire)
        .with_adc("adc_0", adc)
        .build();

    SensorEndpointConfig t_sensor_config = SensorEndpointConfig{
        .sensor = DS18B20_TEMPERATURE_SENSOR, 
        .name = "DS18B20_0", 
        .hal_reference = "onewire_0", 
        .measurement_period_ms = 30000, 
        .int_params = {{"resolution", 3}},
        .string_params = {}
    }; 
    SensorEndpointConfig h_sensor_config = SensorEndpointConfig{ 
        .sensor = ANALOG_HUMIDITY_SENSOR, 
        .name = "analog_humidity_0", 
        .hal_reference = "adc_0", 
        .measurement_period_ms = 20000, 
        .int_params = {{"max_voltage_mv", h_sensor_max_mv}},
        .string_params = {}
    }; 
    
    auto t_endpoint = sensor_endpoint_factory->create(t_sensor_config);
    auto h_endpoint = sensor_endpoint_factory->create(h_sensor_config);

    TaskManager task_manager = TaskManager("main_task_manager", 4096, tskNO_AFFINITY);
    task_manager.add_task(t_endpoint);
    task_manager.add_task(h_endpoint);

    led_strip.init();

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