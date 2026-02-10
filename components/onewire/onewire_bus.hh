#include "esp_log.h"
#include "esp_check.h"
#include "onewire_types.h"
#include "onewire_bus_interface.h"

#include <vector>

#include "hal/onewire_hal.hh"

#ifndef ONEWIRE_BUS_H
#define ONEWIRE_BUS_H

class OnewireBus : public IOnewireBus {
    public:
        OnewireBus(int bus_gpio_num, uint32_t max_rx_bytes);
        ~OnewireBus();

        IN_error_t init() override;

        IN_error_t find_device(onewire_device_address_t address, uint64_t address_mask) override;

        IN_error_t reset() override;
        IN_error_t read_bytes(std::vector<uint8_t> &rx_buf) override;
        IN_error_t write_bytes(std::vector<uint8_t> tx_data) override;

        IN_error_t write_to_all(std::vector<uint8_t> tx_data) override;
        IN_error_t write_to(onewire_device_address_t address, std::vector<uint8_t> tx_data) override;
        
    protected:
    private:

        onewire_bus_handle_t bus = nullptr; 
        int bus_gpio_num;
        uint32_t max_rx_bytes;

        std::vector<onewire_device_address_t> device_list;

        constexpr static char const *TAG = "ONE WIRE";
};

#endif