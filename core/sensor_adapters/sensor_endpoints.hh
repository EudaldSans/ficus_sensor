
#include <string.h>

#include "channels.hh"
#include "sensor.hh"
#include "task.hh"

#ifndef SENSOR_ENDPOINTS_H
#define SENSOR_ENDPOINTS_H

class ISensorEndpointBase : public IIntervalTask {
public:
    virtual ~ISensorEndpointBase() = default;
};

template <typename T> 
class AsyncSensorEndpoint : public ISensorEndpointBase { 
public: 
    AsyncSensorEndpoint(value_t<T>* measurement_output, IAsyncSensor<T> &sensor, uint16_t measurement_period) 
        : _measurement_output(measurement_output), _sensor(sensor), _measurement_period_ms(measurement_period) 
        { }
    ~AsyncSensorEndpoint() = default; 

    void setup() override { 
        uint16_t measurement_delay_ms;

        FIC_LOGI(TAG, "Setting up AsyncSensorEndpoint");

        _sensor.init(); 
        _sensor.trigger_measurement(measurement_delay_ms); 
    }

    void update(uint64_t now) override { 
        T value;
        uint16_t measurement_delay_ms;

        if (!_sensor.is_ready()) { FIC_LOGW(TAG, "Measurement not ready yet"); return;}
        FIC_RETURN_VOID_ON_ERROR(_sensor.get_measurement(value), FIC_LOGE(TAG, "Failed to get sensor value"));
        
        _measurement_output->update(value);
        _sensor.trigger_measurement(measurement_delay_ms); 

        last_run_time_ms = now;
    }
    
    uint32_t get_run_period_ms() const override {
        return _measurement_period_ms;
    } 

private: 
    value_t<T>* _measurement_output; 
    
    IAsyncSensor<T> &_sensor; 
    const uint16_t _measurement_period_ms;
    
    constexpr static char const *TAG = "AsyncSensorEndpoint"; 
};

template <typename T> 
class SensorEndpoint : public ISensorEndpointBase {
public:
    SensorEndpoint(value_t<T>* measurement_output, ISensor<T> &sensor, uint16_t measurement_period) 
        : _measurement_output(measurement_output), _sensor(sensor), _measurement_period_ms(measurement_period) 
        {  } 
    ~SensorEndpoint() = default; 

    void setup() override {
        _sensor.init();
    }
    
    void update(uint64_t now) override {
        T value;

        FIC_RETURN_VOID_ON_ERROR(_sensor.measure(value), FIC_LOGE(TAG, "Failed to measure sensor value"));

        _measurement_output->update(value);

        last_run_time_ms = now;
    }

    uint32_t get_run_period_ms() const override {return _measurement_period_ms;}

private:
    value_t<T>* _measurement_output; 

    ISensor<T> &_sensor;
    const uint16_t _measurement_period_ms;

    constexpr static char const *TAG = "SensorEndpoint";
};

#endif


