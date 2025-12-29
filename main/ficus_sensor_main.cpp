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

#include "adc.hh"
#include "onewire_bus.hh"

#define ONEWIRE_BUS_GPIO        18
#define ONEWIRE_MAX_RX_BYTES    10

#define DS18B20_ADDRESS         0x28
#define DS18B20_MASK            0xFF

static const char *TAG = "example";

#define DS18B20_CMD_CONVERT_TEMP      0x44
#define DS18B20_CMD_WRITE_SCRATCHPAD  0x4E
#define DS18B20_CMD_READ_SCRATCHPAD   0xBE

typedef enum {
    DS18B20_RESOLUTION_9B,  /*!<  9bit, needs ~93.75ms convert time */
    DS18B20_RESOLUTION_10B, /*!< 10bit, needs ~187.5ms convert time */
    DS18B20_RESOLUTION_11B, /*!< 11bit, needs ~375ms convert time */
    DS18B20_RESOLUTION_12B, /*!< 12bit, needs ~750ms convert time */
} ds18b20_resolution_t;
const uint8_t resolution_data[] = {0x1F, 0x3F, 0x5F, 0x7F};
const uint32_t resolution_delays_ms[] = {100, 200, 400, 800};

typedef struct  {
    uint8_t temp_lsb;      /*!< lsb of temperature */
    uint8_t temp_msb;      /*!< msb of temperature */
    uint8_t th_user1;      /*!< th register or user byte 1 */
    uint8_t tl_user2;      /*!< tl register or user byte 2 */
    uint8_t configuration; /*!< resolution configuration register */
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t _reserved3;
    uint8_t crc_value;     /*!< crc value of scratchpad data */
} __attribute__((packed)) ds18b20_scratchpad_t;

extern "C" void app_main(void) {
    std::vector<uint8_t> tx_buffer = {DS18B20_CMD_CONVERT_TEMP};
    std::vector<uint8_t> tx_resolution_buffer = {0, 0, DS18B20_RESOLUTION_12B};
    std::vector<uint8_t> rx_buffer(sizeof(ds18b20_scratchpad_t));
    ds18b20_scratchpad_t scratchpad;

    ADC adc = ADC(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
    adc.init();

    OnewireBus bus = OnewireBus(ONEWIRE_BUS_GPIO, ONEWIRE_MAX_RX_BYTES);
    bus.init();

    bus.find_device(DS18B20_ADDRESS, DS18B20_MASK);

    bus.reset();
    bus.write_to_all(tx_resolution_buffer);

    float temperature, humidity;
    int voltage;
    while (1) {
        adc.measure(voltage);
        humidity = 100 - 100*voltage/3300;
        if (humidity < 0) {
            humidity = 0;
        }

        ESP_LOGI(TAG, "Humidity: %f%%", humidity);

        bus.reset();
        tx_buffer[0] = DS18B20_CMD_CONVERT_TEMP;
        bus.write_to_all(tx_buffer);

        vTaskDelay(pdMS_TO_TICKS(resolution_delays_ms[DS18B20_RESOLUTION_12B]));

        bus.reset();
        tx_buffer[0] = DS18B20_CMD_READ_SCRATCHPAD;
        bus.write_to_all(tx_buffer);
        bus.read_bytes(rx_buffer);

        memcpy(&scratchpad, rx_buffer.data(), sizeof(scratchpad));

        const uint8_t lsb_mask[4] = {0x07, 0x03, 0x01, 0x00}; 
        uint8_t lsb_masked = scratchpad.temp_lsb & (~lsb_mask[scratchpad.configuration >> 5]);
        int16_t temperature_raw = (((int16_t)scratchpad.temp_msb << 8) | lsb_masked);
        temperature = temperature_raw / 16.0f;

        ESP_LOGI(TAG, "Temperature: %fºC", temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}