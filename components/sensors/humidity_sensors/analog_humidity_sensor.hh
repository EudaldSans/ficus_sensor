#include "sensor.hh"
#include "adc.hh"

#ifndef ANALOG_HUMIDITY_SENSOR_H
#define ANALOG_HUMIDITY_SENSOR_H

class AnalogHumiditySensor : public ISensor {
    public: 
      
        AnalogHumiditySensor(uint32_t max_voltage_mv, adc_channel_t chanel, adc_unit_t unit);
        ~AnalogHumiditySensor();

        esp_err_t init() override;
        esp_err_t trigger_measurement(uint16_t &measurement_delay_ms) override;
        float get_last_measurement() override;
        const char* get_name() override;
    
    private: 
        ADC adc;
        uint32_t max_voltage_mv;

        const std::string sensor_name = "analog_humidity_sensor";

        constexpr static char const *TAG = "ANALOG NUMIDITY";
};

#endif