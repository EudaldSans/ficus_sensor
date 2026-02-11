#include <string>

#include "in_errors.hh"

#ifndef SENSOR_H
#define SENSOR_H


class ISensor {
    public:
        virtual ~ISensor() = default; 

        virtual in_error_t init() = 0;
        virtual in_error_t trigger_measurement(uint16_t &measurement_delay_ms) = 0;
        virtual float get_last_measurement() = 0;
        virtual const char* get_name() = 0; 

        const std::string sensor_name = "Abstract sensor";        
};

#endif