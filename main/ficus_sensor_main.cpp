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

#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"
#include "sensor_endpoint.hh"

#include "task_manager.hh"

#include "led_strip.h"

#include "router.hh"
#include "endpoint.hh"
#include "conversions.hh"

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

led_strip_handle_t led_strip;

static const char *TAG = "example";

void configure_led(void) {
    led_strip_config_t strip_config = {};
    // Set the GPIO8 that the LED is connected
    strip_config.strip_gpio_num = LED_GPIO;
    // Set the number of connected LEDs, 1
    strip_config.max_leds = 1;
    // Set the pixel format of your LED strip
    strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB;
    // LED model
    strip_config.led_model = LED_MODEL_WS2812;
    // In some cases, the logic is inverted
    strip_config.flags.invert_out = false;

    led_strip_rmt_config_t rmt_config = {};
    // Set the clock source
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    // Set the RMT counter clock
    rmt_config.resolution_hz = LED_STRIP_RMT_RES_HZ;
    // Set the DMA feature (not supported on the ESP32-C6)
    rmt_config.flags.with_dma = false;

    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
}

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

    led_strip_set_pixel(led_strip, 0, 0.05 * red, 0.05 * green, 0.05 * blue);
    led_strip_refresh(led_strip);
}

void blink(uint32_t time_ms, uint32_t red, uint32_t green, uint32_t blue) {
    led_strip_set_pixel(led_strip, 0, 0, 0, 0);
    led_strip_refresh(led_strip);
    vTaskDelay(pdMS_TO_TICKS(time_ms));

    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

extern "C" void app_main(void) {
    const uint32_t humidity_sensor_max_voltage_mv = 3300;

    const std::string temp_producer = "temp_producer";
    const std::string humidity_producer = "humidity_producer";

    const std::string temp_consumer = "temp_consumer";
    const std::string humidity_consumer = "humidity_consumer";

    auto bus = std::make_shared<OnewireBus>(OnewireBus(ONEWIRE_BUS_GPIO));
    DS18B20 temp_sensor = DS18B20(bus, DS18B20::resolution_12B);

    auto adc = std::make_shared<ADC>(ADC(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT));
    AnalogHumiditySensor humidity_sensor = AnalogHumiditySensor(adc, humidity_sensor_max_voltage_mv);

    SensorEndpoint temp_endpoint = SensorEndpoint(temp_producer, std::make_shared<DS18B20>(temp_sensor), 30000);
    SensorEndpoint humidity_endpoint = SensorEndpoint(humidity_producer, std::make_shared<AnalogHumiditySensor>(humidity_sensor), 20000);

    TaskManager task_manager = TaskManager("main_task_manager", 4096, tskNO_AFFINITY);
    task_manager.add_task(std::make_shared<SensorEndpoint>(temp_endpoint));
    task_manager.add_task(std::make_shared<SensorEndpoint>(humidity_endpoint));

    configure_led();
    led_strip_refresh(led_strip);

    std::vector<std::shared_ptr<IConversion<float>>> conversions;
    // conversions.push_back(std::make_shared<ToFahrenheitConversion<float>>());

    ESP_LOGI(TAG, "Size of conversions: %d", conversions.size());

    ChannelEndpoint consumer = ChannelEndpoint();

    consumer.add_input_channel<int>(temp_consumer, 
        [](const int& temperature) {
            ESP_LOGI(TAG, "temp_consumer got %d", temperature);
            set_heat_led(temperature);
        }
    );
    consumer.add_input_channel<int>(humidity_consumer, 
        [](const int& humidity) {
            ESP_LOGI(TAG, "humidity_consumer got %d", humidity);
        }
    );

    Router::link<float, int>(temp_endpoint, temp_producer, consumer, temp_consumer, conversions);
    Router::link<float, int>(humidity_endpoint, humidity_producer, consumer, humidity_consumer, conversions);

    task_manager.start();

    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}