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

#include "led_strip.h"

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

    DS18B20 temp_sensor = DS18B20(ONEWIRE_BUS_GPIO);
    temp_sensor.init();
    temp_sensor.set_resolution(DS18B20::resolution_12B);

    AnalogHumiditySensor humidity_sensor = AnalogHumiditySensor(humidity_sensor_max_voltage_mv, ADC_CHANNEL_2, ADC_UNIT_1);
    humidity_sensor.init();

    configure_led();
    led_strip_refresh(led_strip);

    uint16_t measurement_delay_ms = 0;
    float temperature, humidity;
    while (1) {
        humidity = humidity_sensor.get_last_measurement();
        ESP_LOGI(TAG, "Humidity: %f%%", humidity);

        temp_sensor.trigger_measurement(measurement_delay_ms);
        vTaskDelay(measurement_delay_ms / portTICK_PERIOD_MS);
        temperature = temp_sensor.get_last_measurement();

        ESP_LOGI(TAG, "Temperature: %fºC", temperature);
        set_heat_led(temperature);

        vTaskDelay(pdMS_TO_TICKS(25));

        if (temperature >= 30) break;
    }

    uint32_t blink_time_ms = 300;

    blink(blink_time_ms / 2, 120, 120, 120);
    blink(blink_time_ms / 2, 120, 120, 120);
    blink(blink_time_ms / 2, 120, 120, 120);

    while (1) {
        blink(blink_time_ms, 120, 0, 0);
        blink(blink_time_ms, 120, 0, 0);
        blink(blink_time_ms, 120, 0, 0);
        blink(blink_time_ms, 120, 0, 0);
        blink(blink_time_ms, 120, 0, 0);

        blink(blink_time_ms, 0, 120, 0);
        blink(blink_time_ms, 0, 120, 0);

        blink(blink_time_ms, 0, 0, 120);
        blink(blink_time_ms, 0, 0, 120);
        blink(blink_time_ms, 0, 0, 120);

        blink(blink_time_ms * 2, 0, 0, 0);
    }
}