#include "sensor.hh"
#include "adc_hal.hh"

#include <memory>

#ifndef ANALOG_HUMIDITY_SENSOR_H
#define ANALOG_HUMIDITY_SENSOR_H

class AnalogHumiditySensor : public ISensor<float> {
    public: 
      
        AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t max_voltage_mv);
        ~AnalogHumiditySensor();

        in_error_t init() override;
        in_error_t deinit() override;
        in_error_t measure(float &value) override;

        std::string_view get_name() const override {return "Analog Humidity Sensor";}
        std::string_view get_type() const override {return "Analog";}
        std::string_view get_unit() const override {return "%";}

    private: 
        std::shared_ptr<IADC> _adc;
        uint32_t _max_voltage_mv;

        constexpr static char const *TAG = "ANALOG HUMIDITY";
};

#endif