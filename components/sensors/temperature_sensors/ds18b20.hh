#include <memory>

#include "sensor.hh"
#include "onewire_bus.hh"

#ifndef DS18B20_H
#define DS18B20_H

class DS18B20 : public ISensor {
    public: 
        typedef enum {
            resolution_9B, 
            resolution_10B, 
            resolution_11B,
            resolution_12B, 
        } ds18b20_resolution_t;
        
        DS18B20(std::shared_ptr<IOnewireBus> bus, ds18b20_resolution_t resolution);
        ~DS18B20();

        esp_err_t init() override;
        esp_err_t trigger_measurement(uint16_t &measurement_delay_ms) override;
        float get_last_measurement() override;
        const char* get_name() override;

        void sensor_tic();

        esp_err_t set_resolution(ds18b20_resolution_t resolution);

        const std::string sensor_name = "DS18B20";
    
    private: 
        static constexpr  uint8_t cmd_convert_temp = 0x44;
        static constexpr  uint8_t cmd_write_scratchpad = 0x4E;
        static constexpr  uint8_t cmd_read_scratchpad = 0xBE;

        std::shared_ptr<IOnewireBus> _bus;
        ds18b20_resolution_t resolution;

        constexpr static char const *TAG = "DS18B20";
};

#endif