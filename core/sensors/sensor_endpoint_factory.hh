

#ifndef SENSOR_FACTORY_H 
#define SENSOR_FACTORY_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "sensor.hh"
#include "adc_hal.hh"
#include "onewire_hal.hh"
#include "task.hh"
#include "hal_registry.hh"

enum sensor_t {
    ANALOG_HUMIDITY_SENSOR = 0, 
    DS18B20_TEMPERATURE_SENSOR = 1 
};

struct SensorEndpointConfig {
    sensor_t sensor;
    std::string name;
    std::string hal_reference;
    uint16_t measurement_period_ms;
    std::unordered_map<std::string, int> int_params;
    std::unordered_map<std::string, std::string> string_params;
    
    bool get_int_param(const std::string& key, int& value) const;
    bool get_string_param(const std::string& key, std::string& value) const;
};


class SensorFactory {
public:
    explicit SensorFactory(std::shared_ptr<HalRegistry> registry);
    ~SensorFactory() = default;
    
    std::shared_ptr<ITask> create(const SensorEndpointConfig& config);
    
    std::vector<std::shared_ptr<ITask>> create_batch(
        const std::vector<SensorEndpointConfig>& configs
    );
    
private:
    std::shared_ptr<HalRegistry> _hal_registry;
    
    std::shared_ptr<ITask> create_ds18b20_endpoint(const SensorEndpointConfig& config);
    std::shared_ptr<ITask> create_analog_humidity_endpoint(const SensorEndpointConfig& config);
    
    constexpr static char const *TAG = "SENSOR_FACTORY";
};


class SensorFactoryBuilder {
public:
    SensorFactoryBuilder() : _hal_registry(std::make_shared<HalRegistry>()) {}
    
    SensorFactoryBuilder& with_adc(const std::string& name, std::shared_ptr<IADC> adc);
    SensorFactoryBuilder& with_onewire(const std::string& name, std::shared_ptr<IOnewireBus> bus);
    
    std::unique_ptr<SensorFactory> build();
    
private:
    std::shared_ptr<HalRegistry> _hal_registry;
};

#endif
