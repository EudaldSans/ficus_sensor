#include "esp_log.h"
#include "esp_check.h"
#include "onewire_types.h"
#include "onewire_bus_interface.h"

#include <vector>

#ifndef ONEWIRE_BUS_H
#define ONEWIRE_BUS_H

class OnewireBus {
    public:
        OnewireBus(int bus_gpio_num, uint32_t max_rx_bytes);
        ~OnewireBus();

        esp_err_t init();

        esp_err_t find_device(onewire_device_address_t address, uint64_t address_mask);

        esp_err_t write_command(uint8_t cmd);
        esp_err_t read_command(uint8_t cmd, uint8_t *read_buffer, size_t read_buffer_size, size_t &read_bytes);
        
    protected:
    private:
        struct bus_device {
            onewire_device_address_t address;
        } bus_device_t;

        onewire_bus_handle_t bus = nullptr; 
        int bus_gpio_num;
        uint32_t max_rx_bytes;

        std::vector<bus_device> device_list;

        constexpr static char const *TAG = "ONE WIRE";
};

#endif