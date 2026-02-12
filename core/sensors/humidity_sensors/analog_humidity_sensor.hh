#include "sensor.hh"
#include "adc.hh"

#include <memory>

#ifndef ANALOG_HUMIDITY_SENSOR_H
#define ANALOG_HUMIDITY_SENSOR_H

class AnalogHumiditySensor : public ISensorLifecycle, public ISensorMetadata, public ISensor<float> {
    public: 
      
        AnalogHumiditySensor(std::shared_ptr<IADC> adc, uint32_t max_voltage_mv);
        ~AnalogHumiditySensor();

        in_error_t init() override;
        in_error_t deinit() override;
        in_error_t measure(float &value) override;

        const char* get_name() override {return "Analog Humidity Sensor";}
        const char* get_type() override {return "Analog";}
        const char* get_unit() override {return "%";}

    private: 
        std::shared_ptr<IADC> _adc;
        uint32_t _max_voltage_mv;

        constexpr static char const *TAG = "ANALOG HUMIDITY";
};

#endif