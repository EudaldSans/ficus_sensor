
#include <string.h>
#include "endpoint.hh"
#include "sensor.hh"

#ifndef SENSOR_ENDPOINT_H
#define SENSOR_ENDPOINT_H


class SensorEndpoint : public ChannelEndpoint {
    public:
        SensorEndpoint(std::shared_ptr<ISensor> sensor, uint16_t measurement_period);
        virtual ~SensorEndpoint() = default; 

        void sensor_tic();

    private:
        OutputChannel<float>* _measurement_output;
        std::shared_ptr<ISensor> _sensor;

        uint16_t _measurement_period_ms;
};

#endif


