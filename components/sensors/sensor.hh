#include "esp_check.h"

#ifndef SENSOR_H
#define SENSOR_H


class ISensor {
    public:
        virtual ~ISensor() = default; 

        virtual esp_err_t init() = 0;
        virtual esp_err_t trigger_measurement(uint16_t &measurement_delay_ms) = 0;
        virtual float get_last_measurement() = 0;
        virtual const char* get_name() = 0;        
};

#endif