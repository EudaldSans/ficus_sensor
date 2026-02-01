#include "esp_check.h"

#include <string.h>
#include "endpoint.hh"

#ifndef SENSOR_H
#define SENSOR_H


class Sensor : public ChannelEndpoint {
    public:
        Sensor(const std::string output_name);
        virtual ~Sensor() = default; 

        virtual esp_err_t init() = 0;
        virtual esp_err_t trigger_measurement(uint16_t &measurement_delay_ms) = 0;
        virtual float get_last_measurement() = 0;
        virtual const char* get_name() = 0; 
        
        void sensor_tic();

    private:
        OutputChannel<float>* measurement_output;
};

#endif