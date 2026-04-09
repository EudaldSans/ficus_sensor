/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "fic_log.hh"
#include "esp_fic_log.hh"

#include "task_manager.hh"
#include "freertos_task.hh"

#include "version.hh"
#include "composition.hh"

static constexpr Version product_version = Version(0, 0, 1, 0);

static const char *TAG = "main";

static TaskManager task_manager(
    "main_task_manager", 
    std::make_unique<FreeRTOS_TaskRunner>("main_task_manager", 4096, tskNO_AFFINITY)
);

extern "C" void app_main(void) {  
    // Always start logging backend first, so other components can print logs
    fic_log_set_backend(esp32_backend);

    FIC_LOGI(TAG, "Starting Ficus Sensor, version %s", product_version.c_str());

    composition_init_hardware();
    composition_add_tasks(task_manager);
    composition_start_comms();

    task_manager.start();

    while (1) {
        uint32_t blink_time = 500;
        uint16_t cycles = 2;

        if (composition_get_wifi_state() != WiFiState::STA_CONNECTED) {
            rgb_signaler.set_blink(LED_BLUE, blink_time, LED_OFF, blink_time, cycles);
        } else {
            rgb_signaler.set_blink(LED_BLUE, blink_time / 5, LED_OFF, blink_time / 5, cycles * 5);
        }

        if (firebase_h_input.is_new()) {
            if (firebase_h_input.is_valid()) {
                int humidity = firebase_h_input.consume();
                FIC_LOGI(TAG, "humidity is %d", humidity);
            } else {
                FIC_LOGI(TAG, "humidity is not valid");
            }
        }

        if (firebase_t_input.is_new()) {
            if (firebase_t_input.is_valid()) {
                int temperature = firebase_t_input.consume();
                FIC_LOGI(TAG, "temperature is %d", temperature);
            } else {
                FIC_LOGI(TAG, "temperature is not valid");
            }
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}