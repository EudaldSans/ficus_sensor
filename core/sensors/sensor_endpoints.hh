
#include <string.h>

#include "endpoint.hh"
#include "sensor.hh"
#include "task.hh"

#ifndef SENSOR_ENDPOINTS_H
#define SENSOR_ENDPOINTS_H

template <typename T> 
class AsyncSensorEndpoint : public ChannelEndpoint, public ITask { 
public: 
    AsyncSensorEndpoint(const std::string& output_name, std::shared_ptr<IAsyncSensor<T>> sensor, uint16_t measurement_period) {
        _measurement_output = add_output_channel<T>(_output_name);
    }
    ~AsyncSensorEndpoint() = default; 
        
    void setup() override {
        _sensor->init();
    }

    void update() override {
        T value;

        IN_RETURN_ON_ERROR(_sensor->measure(value), ESP_LOGE(TAG, "Failed to measure sensor value"));

        _measurement_output->emit(value);
    }
    
    uint32_t get_run_period_ms() override {
        return _measurement_period_ms;
    } 

private: 
    OutputChannel<T>* _measurement_output; 
    
    std::shared_ptr<IAsyncSensor<T>> _sensor; 

    const std::string _output_name; 
    const uint16_t _measurement_period_ms;
    
    constexpr static char const *TAG = "AsyncSensorEndpoint"; 
};

template <typename T> 
class SensorEndpoint : public ChannelEndpoint, public ITask {
public:
    SensorEndpoint(const std::string& output_name, std::shared_ptr<ISensor<T>> sensor, uint16_t measurement_period) { 
        _measurement_output = add_output_channel<T>(_output_name); 
    } 
    ~SensorEndpoint() = default; 

    void setup() override { 
        _sensor->init(); 
        _sensor->trigger_measurement(_measurement_period_ms); 
    }
    
    void update() override { 
        T value;

        if (!_sensor->is_ready()) { ESP_LOGW(TAG, "Measurement not ready yet"); return;}

        IN_RETURN_ON_ERROR(sensor->get_measurement(), ESP_LOGE(TAG, "Failed to measure sensor value")); 
        
        _measurement_output->emit(value);
    }

    uint32_t get_run_period_ms() override {return _measurement_period_ms;}

private:
    OutputChannel<T>* _measurement_output;

    std::shared_ptr<ISensor<T>> _sensor;
    const std::string _output_name;
    const uint16_t _measurement_period_ms;

    constexpr static char const *TAG = "SensorEndpoint";
};

#endif


