#include "sensor_endpoint_factory.hh"
#include "sensor_endpoints.hh"
#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"

#include "esp_log.h"

/* SENSOR ENDPOINT CONFIG */

bool SensorEndpointConfig::get_int_param(const std::string& key, int& value) const { 
    auto it = int_params.find(key); 

    if (it != int_params.end()) {
        value = it->second; 
        return true;
    }

    return false; 
} 

bool SensorEndpointConfig::get_string_param(const std::string& key, std::string& value) const { 
    auto it = string_params.find(key); 

    if (it != string_params.end()) {
        value = it->second; 
        return true;
    }

    return false; 
}


/* SENSOR ENDPOINT FACTORY */

SensorEndpointFactory::SensorEndpointFactory(std::shared_ptr<HalRegistry> registry) 
    : _hal_registry(registry) {
}

std::shared_ptr<IRunnableEndpoint> SensorEndpointFactory::create(const SensorEndpointConfig& config) {
    switch (config.sensor) {
        case sensor_t::DS18B20_TEMPERATURE_SENSOR:
            return create_ds18b20_endpoint(config);
        
        case sensor_t::ANALOG_HUMIDITY_SENSOR:
            return create_analog_humidity_endpoint(config);
        
        default:
            ESP_LOGE(TAG, "Unknown sensor type: %d", config.sensor);
            return nullptr;
    }
}

std::vector<std::shared_ptr<IRunnableEndpoint>> SensorEndpointFactory::create_batch(
    const std::vector<SensorEndpointConfig>& configs) {
    
    std::vector<std::shared_ptr<IRunnableEndpoint>> endpoints;
    endpoints.reserve(configs.size());
    
    for (const auto& config : configs) {
        auto endpoint = create(config);
        if (endpoint) {
            endpoints.push_back(endpoint);
        } else {
            ESP_LOGW(TAG, "Failed to create sensor endpoint for: %s", config.name.c_str());
        }
    }
    
    return endpoints;
}

std::shared_ptr<IRunnableEndpoint> SensorEndpointFactory::create_ds18b20_endpoint(const SensorEndpointConfig& config) {
    auto bus = _hal_registry->get_onewire(config.hal_reference);
    if (!bus) {
        ESP_LOGE(TAG, "Onewire bus '%s' not found in registry", config.hal_reference.c_str());
        return nullptr;
    }
    
    int resolution_int;
    if (!config.get_int_param("resolution", resolution_int)) {
        ESP_LOGE(TAG, "Required parameter 'resolution' not found for sensor '%s'", config.name.c_str());
        return nullptr;
    }
    
    DS18B20::ds18b20_resolution_t resolution;
    switch (resolution_int) {
        case 0: resolution = DS18B20::resolution_9B; break;
        case 1: resolution = DS18B20::resolution_10B; break;
        case 2: resolution = DS18B20::resolution_11B; break;
        case 3: resolution = DS18B20::resolution_12B; break;
        default:
            ESP_LOGE(TAG, "Invalid resolution value '%d' for sensor '%s'", resolution_int, config.name.c_str());
            return nullptr;
    }
    
    auto sensor = std::make_shared<DS18B20>(bus, resolution);
    
    return std::make_shared<AsyncSensorEndpoint<float>>(
        config.name,
        sensor,
        config.measurement_period_ms
    );
}

std::shared_ptr<IRunnableEndpoint> SensorEndpointFactory::create_analog_humidity_endpoint(const SensorEndpointConfig& config) {
    auto adc = _hal_registry->get_adc(config.hal_reference);
    if (!adc) {
        ESP_LOGE(TAG, "ADC '%s' not found in registry", config.hal_reference.c_str());
        return nullptr;
    }
    
    int max_voltage_mv;
    if (!config.get_int_param("max_voltage_mv", max_voltage_mv)) {
        ESP_LOGE(TAG, "Required parameter 'max_voltage_mv' not found for sensor '%s'", config.name.c_str());
        return nullptr;
    }
    
    auto sensor = std::make_shared<AnalogHumiditySensor>(adc, max_voltage_mv);
    
    return std::make_shared<SensorEndpoint<float>>(
        config.name,
        sensor,
        config.measurement_period_ms
    );
}


/* SENSOR ENDPOINT FACTORY BUILDER */

SensorEndpointFactoryBuilder& SensorEndpointFactoryBuilder::with_adc(const std::string& name, std::shared_ptr<IADC> adc) {
    _hal_registry->register_adc(name, adc);
    return *this;
}

SensorEndpointFactoryBuilder& SensorEndpointFactoryBuilder::with_onewire(const std::string& name, std::shared_ptr<IOnewireBus> bus) {
    _hal_registry->register_onewire(name, bus);
    return *this;
}

std::unique_ptr<SensorEndpointFactory> SensorEndpointFactoryBuilder::build() {
    return std::make_unique<SensorEndpointFactory>(_hal_registry);
}

