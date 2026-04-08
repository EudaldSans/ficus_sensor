/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "fic_log.hh"

#include "nvs_flash.h"

#include "sdkconfig.h"
#include "fic_log.hh"
#include "esp_fic_log.hh"

#include "task_manager.hh"

#include "conversions.hh"

#include "esp_time.hh"

#include "freertos_task.hh"
#include "freertos_queue.hh"

#include "routing.hh"
#include "hardware.hh"
#include "endpoints.hh"

static const char *TAG = "main";

TaskManager task_manager = TaskManager(
    "main_task_manager", 
    std::make_unique<FreeRTOS_TaskRunner>("main_task_manager", 4096, tskNO_AFFINITY)
);

void add_tasks() {
    FIC_LOGI(TAG, "Setting up task manager");

    task_manager.add_task(&t_endpoint);
    task_manager.add_task(&h_endpoint);
    task_manager.add_task(&rgb_signaler);
    task_manager.add_task(&router);
}

void start_hardware() {
    FIC_LOGI(TAG, "Initializing hardware");
    ITimeSource::set_instance(&EspTimeSource::instance());
    ITimeDelay::set_instance(&EspTimeDelay::instance());

    led_strip.init();

    nvs_flash_init();
}

extern "C" void app_main(void) {  
    // Always start logging backend first, so other components can print logs
    fic_log_set_backend(esp32_backend);

    start_hardware();

    add_tasks();
    
    http_client.start();
    task_manager.start();

    // wifi.init();
    // wifi_station.sta_connect("XTA_47592", "Mh9gcxu5", 10);

    while (1) {
        uint32_t blink_time = 500;
        uint16_t cycles = 2;
        size_t free_mem = esp_get_free_heap_size();

        FIC_LOGI(TAG, "free heap size %d", free_mem);

        if (wifi.get_state() != WiFiState::STA_CONNECTED) {
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

        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}