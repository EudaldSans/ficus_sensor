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

#include "sntp_client.hh"

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

    EspSntpClient sntp_client = EspSntpClient(wifi_controller_ref);
    sntp_client.add_server("pool.ntp.org");

    uint32_t blink_time = 500;
    uint16_t cycles = 2;

    while (1) {
        if (!sntp_client.is_syncing() && !sntp_client.is_synced()) {
            if (composition_get_wifi_state() == WiFiState::STA_CONNECTED) {
                FIC_LOGI(TAG, "Starting time sync");
                sntp_client.sync_system_time();
            }
        }

        if (composition_get_wifi_state() != WiFiState::STA_CONNECTED) {
            rgb_signaler.set_blink(LED_BLUE, blink_time, LED_OFF, blink_time, cycles);
        } else {
            rgb_signaler.set_blink(LED_BLUE, blink_time / 5, LED_OFF, blink_time / 5, cycles * 5);
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}