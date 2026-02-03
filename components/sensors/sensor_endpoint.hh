
#include <string.h>
#include "endpoint.hh"
#include "sensor.hh"

#ifndef SENSOR_ENDPOINT_H
#define SENSOR_ENDPOINT_H


class SensorEndpoint : public ChannelEndpoint {
    public:
        SensorEndpoint(const std::string& output_name, std::shared_ptr<ISensor> sensor);
        virtual ~SensorEndpoint() = default; 

        void sensor_tic();

    private:
        OutputChannel<float>* _measurement_output;
        std::shared_ptr<ISensor> _sensor;
};

#endif


