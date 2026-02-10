

#ifndef ONEWIRE_HAL_H
#define ONEWIRE_HAL_H

class IOnewireBus {
    public:
        virtual ~IOnewireBus() = default;

        virtual esp_err_t init() = 0;
        virtual esp_err_t find_device(onewire_device_address_t address, uint64_t address_mask) = 0;
        virtual esp_err_t reset() = 0;
        virtual esp_err_t read_bytes(std::vector<uint8_t> &rx_buf) = 0;
        virtual esp_err_t write_bytes(std::vector<uint8_t> tx_data) = 0;
        virtual esp_err_t write_to_all(std::vector<uint8_t> tx_data) = 0;
        virtual esp_err_t write_to(onewire_device_address_t address, std::vector<uint8_t> tx_data) = 0;
}



#endif