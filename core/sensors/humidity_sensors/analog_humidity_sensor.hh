#include "sensor.hh"
#include "adc.hh"

#include <memory>

#ifndef ANALOG_HUMIDITY_SENSOR_H
#define ANALOG_HUMIDITY_SENSOR_H

class AnalogHumiditySensor : public ISensor {
    public: 
      
        AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t max_voltage_mv);
        ~AnalogHumiditySensor();

        in_error_t init() override;
        in_error_t trigger_measurement(uint16_t &measurement_delay_ms) override;
        float get_last_measurement() override;
        const char* get_name() override;

        const std::string sensor_name = "analog_humidity_sensor";
    private: 
        std::shared_ptr<IADC> _adc;
        uint32_t _max_voltage_mv;

        constexpr static char const *TAG = "ANALOG NUMIDITY";
};

#endif