#include <memory>

#include "sensor.hh"
#include "onewire_hal.hh"

#ifndef DS18B20_H
#define DS18B20_H

class DS18B20 : public IAsyncSensor<float> {
    public: 
        typedef enum {
            resolution_9B, 
            resolution_10B, 
            resolution_11B,
            resolution_12B, 
        } ds18b20_resolution_t;
        
        DS18B20(std::shared_ptr<IOnewireBus> bus, ds18b20_resolution_t resolution);
        ~DS18B20();

        in_error_t init() override;
        in_error_t deinit() override {return FIC_OK;}

        in_error_t trigger_measurement(uint16_t &measurement_delay_ms) override;
        in_error_t get_measurement(float &value) override;
        bool is_ready() override;

        std::string_view get_name() const override {return "DS18B20";}
        std::string_view get_type() const override {return "temperature";} 
        std::string_view get_unit() const override {return "°C";}

        in_error_t set_resolution(ds18b20_resolution_t resolution);
    
    private: 
        static constexpr  uint8_t cmd_convert_temp = 0x44;
        static constexpr  uint8_t cmd_write_scratchpad = 0x4E;
        static constexpr  uint8_t cmd_read_scratchpad = 0xBE;

        std::shared_ptr<IOnewireBus> _bus;
        ds18b20_resolution_t _resolution;
        uint64_t _measure_finish_time_ms = 0xFFFFFFFFFFFFFFFF;

        constexpr static char const *TAG = "DS18B20";
};

#endif