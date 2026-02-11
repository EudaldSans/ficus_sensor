
#include <string.h>

#include "endpoint.hh"
#include "sensor.hh"
#include "task.hh"

#ifndef SENSOR_ENDPOINT_H
#define SENSOR_ENDPOINT_H


class SensorEndpoint : public ChannelEndpoint, public ITask {
    public:
        SensorEndpoint(const std::string& output_name, std::shared_ptr<ISensor> sensor, uint16_t measurement_period);
        ~SensorEndpoint() = default; 

        void sensor_tic();

        void setup() override;
        void update() override;

        uint32_t get_run_period_ms() override {return _measurement_period_ms;}

    private:
        OutputChannel<float>* _measurement_output;
        uint64_t _next_measurement_time_ms;

        std::shared_ptr<ISensor> _sensor;
        const std::string _output_name;
        const uint16_t _measurement_period_ms;

        void _trigger_measurement();

        constexpr static char const *TAG = "SensorEndpoint";
};

#endif


