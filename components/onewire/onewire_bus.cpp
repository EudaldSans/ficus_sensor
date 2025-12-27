#include "onewire_bus.hh"

#include "onewire_bus.h"
#include "onewire_device.h"
#include "onewire_cmd.h"
#include "onewire_crc.h"

#include "esp_log.h"
#include "esp_check.h"


OnewireBus::OnewireBus(int bus_gpio_num, uint32_t max_rx_bytes) {
    this->bus_gpio_num = bus_gpio_num;
    this->max_rx_bytes = max_rx_bytes;
}

OnewireBus::~OnewireBus() {

}

esp_err_t OnewireBus::init() {
    onewire_bus_config_t bus_config = {};
    bus_config.bus_gpio_num = bus_gpio_num;
    bus_config.flags.en_pull_up = true;

    onewire_bus_rmt_config_t rmt_config = {};
    rmt_config.max_rx_bytes = max_rx_bytes;

    ESP_RETURN_ON_ERROR(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus), TAG, "Unable to create new onewire unit");
    return ESP_OK;
}

esp_err_t OnewireBus::find_device(onewire_device_address_t address, onewire_device_address_t address_mask) {
    int device_num = 0;
    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;

    ESP_RETURN_ON_ERROR(onewire_new_device_iter(bus, &iter), TAG, "Unable to create device iterator");
    ESP_LOGI(TAG, "Device iterator created, start searching...");

    do {
        search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
        if (search_result == ESP_OK) {

            if ((next_onewire_device.address & address_mask) == address) {
                ESP_LOGI(TAG, "Found a Onewire device[%d], address: %016llX", device_num, address);
                device_num++;

            } else {
                ESP_LOGI(TAG, "Found an unknown device, address: %016llX", next_onewire_device.address);
            }
        }
    } while (search_result != ESP_ERR_NOT_FOUND);

    ESP_RETURN_ON_ERROR(onewire_del_device_iter(iter), TAG, "Unable to delete device iterator");
    ESP_LOGI(TAG, "Searching done, %d Onewire device(s) found with address 0x%x", device_num, address);

    return ESP_OK;
}

esp_err_t OnewireBus::write_command(uint8_t cmd) {
    return ESP_OK;
}
esp_err_t OnewireBus::read_command(uint8_t cmd, uint8_t *read_buffer, size_t read_buffer_size, size_t &read_bytes) {
    return ESP_OK;
}