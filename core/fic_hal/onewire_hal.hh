#include <vector>

#include "fic_errors.hh"

#ifndef ONEWIRE_HAL_H
#define ONEWIRE_HAL_H

class IOnewireBus {
    public:
        virtual ~IOnewireBus() = default;

        virtual fic_error_t init(uint32_t max_rx_bytes) = 0;
        virtual fic_error_t find_device(uint64_t address, uint64_t address_mask) = 0;
        virtual fic_error_t reset() = 0;
        virtual fic_error_t read_bytes(std::vector<uint8_t> &rx_buf) = 0;
        virtual fic_error_t write_bytes(std::vector<uint8_t> tx_data) = 0;
        virtual fic_error_t write_to_all(std::vector<uint8_t> tx_data) = 0;
        virtual fic_error_t write_to(uint64_t address, std::vector<uint8_t> tx_data) = 0;

        bool is_initialized() const { return _initialized; }

    private: 
        bool _initialized = false;
};

#endif