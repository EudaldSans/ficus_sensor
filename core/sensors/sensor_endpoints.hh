
#include <string.h>

#include "endpoint.hh"
#include "sensor.hh"
#include "task.hh"

#ifndef SENSOR_ENDPOINTS_H
#define SENSOR_ENDPOINTS_H

template <typename T> 
class AsyncSensorEndpoint : public IRunnableEndpoint { 
public: 
    AsyncSensorEndpoint(const std::string& output_name, std::shared_ptr<IAsyncSensor<T>> sensor, uint16_t measurement_period) : _output_name(output_name), _sensor(sensor), _measurement_period_ms(measurement_period) {
        _measurement_output = add_output_channel<T>(_output_name);
    }
    ~AsyncSensorEndpoint() = default; 

    void setup() override { 
        uint16_t measurement_delay_ms;

        _sensor->init(); 
        _sensor->trigger_measurement(measurement_delay_ms); 
    }

    void update() override { 
        T value;
        uint16_t measurement_delay_ms;

        if (!_sensor->is_ready()) { ESP_LOGW(TAG, "Measurement not ready yet"); return;}
        IN_RETURN_VOID_ON_ERROR(_sensor->get_measurement(value), ESP_LOGE(TAG, "Failed to get sensor value"));
        
        _measurement_output->emit(value);
        _sensor->trigger_measurement(measurement_delay_ms); 
    }
    
    uint32_t get_run_period_ms() override {
        return _measurement_period_ms;
    } 

private: 
    OutputChannel<T>* _measurement_output; 
    
    const std::string _output_name; 
    std::shared_ptr<IAsyncSensor<T>> _sensor; 
    const uint16_t _measurement_period_ms;
    
    constexpr static char const *TAG = "AsyncSensorEndpoint"; 
};

template <typename T> 
class SensorEndpoint : public IRunnableEndpoint {
public:
    SensorEndpoint(const std::string& output_name, std::shared_ptr<ISensor<T>> sensor, uint16_t measurement_period) : _output_name(output_name), _sensor(sensor), _measurement_period_ms(measurement_period) { 
        _measurement_output = add_output_channel<T>(_output_name); 
    } 
    ~SensorEndpoint() = default; 

    void setup() override {
        _sensor->init();
    }
    
    void update() override {
        T value;

        IN_RETURN_VOID_ON_ERROR(_sensor->measure(value), ESP_LOGE(TAG, "Failed to measure sensor value"));

        _measurement_output->emit(value);
    }

    uint32_t get_run_period_ms() override {return _measurement_period_ms;}

private:
    OutputChannel<T>* _measurement_output;

    const std::string _output_name;
    const std::shared_ptr<ISensor<T>> _sensor;
    const uint16_t _measurement_period_ms;

    constexpr static char const *TAG = "SensorEndpoint";
};

#endif


