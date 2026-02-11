#include "onewire_bus.hh"

#include "onewire_bus.h"
#include "onewire_device.h"
#include "onewire_cmd.h"
#include "onewire_crc.h"

#include "esp_log.h"
#include "esp_check.h"


OnewireBus::OnewireBus(int bus_gpio_num) {
    this->bus_gpio_num = bus_gpio_num;
}

OnewireBus::~OnewireBus() {
    if (bus != nullptr) onewire_bus_del(bus);
}

in_error_t OnewireBus::init(uint32_t max_rx_bytes) {
    onewire_bus_config_t bus_config = {};
    bus_config.bus_gpio_num = bus_gpio_num;
    bus_config.flags.en_pull_up = true;

    onewire_bus_rmt_config_t rmt_config = {};
    rmt_config.max_rx_bytes = max_rx_bytes;

    if (onewire_new_bus_rmt(&bus_config, &rmt_config, &bus) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to create new onewire unit"); 
        return IN_ERR_SDK_FAIL;
    }

    return IN_OK;
}

in_error_t OnewireBus::find_device(uint64_t address, uint64_t address_mask) {
    int device_num = 0;
    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;

    if (onewire_new_device_iter(bus, &iter) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to create device iterator"); 
        return IN_ERR_SDK_FAIL;
    }

    ESP_LOGI(TAG, "Device iterator created, start searching...");

    do {
        search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
        if (search_result == ESP_OK) {

            if ((next_onewire_device.address & address_mask) == address) {
                ESP_LOGI(TAG, "Found a Onewire device[%d], address: %016llX", device_num, address);
                device_list.push_back(next_onewire_device.address);
                device_num++;

            } else {
                ESP_LOGI(TAG, "Found an unknown device, address: %016llX", next_onewire_device.address);
            }
        }
    } while (search_result != ESP_ERR_NOT_FOUND);

    if (onewire_del_device_iter(iter) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to delete device iterator"); 
        return IN_ERR_SDK_FAIL;
    }

    ESP_LOGI(TAG, "Searching done, %d Onewire device(s) found with address 0x%x", device_num, address);

    return IN_OK;
}

in_error_t OnewireBus::write_to_all(std::vector<uint8_t> tx_data) {
    std::vector<uint8_t> tx_buffer;
    
    tx_buffer.push_back(ONEWIRE_CMD_SKIP_ROM);
    tx_buffer.insert(tx_buffer.end(), tx_data.begin(), tx_data.end());
    return write_bytes(tx_buffer);
}

in_error_t OnewireBus::write_to(uint64_t address, std::vector<uint8_t> tx_data) {
    std::vector<uint8_t> tx_buffer;
    
    tx_buffer.push_back(ONEWIRE_CMD_MATCH_ROM);

    for (int i = 7; i >= 0; --i) {
        tx_buffer.push_back(static_cast<uint8_t>((address >> (i * 8)) & 0xFF));
    }

    tx_buffer.insert(tx_buffer.end(), tx_data.begin(), tx_data.end());
    return write_bytes(tx_buffer);
}

in_error_t OnewireBus::reset() {
    return (onewire_bus_reset(bus) == ESP_OK) ? IN_OK : IN_ERR_SDK_FAIL;
}

in_error_t OnewireBus::read_bytes(std::vector<uint8_t> &rx_buf) {
    return (onewire_bus_read_bytes(bus, rx_buf.data(), rx_buf.size()) == ESP_OK) ? IN_OK : IN_ERR_SDK_FAIL;
}

in_error_t OnewireBus::write_bytes(std::vector<uint8_t> tx_data) {
    return (onewire_bus_write_bytes(bus, tx_data.data(), tx_data.size()) == ESP_OK) ? IN_OK : IN_ERR_SDK_FAIL;
}

