
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "onewire_hal.hh"
#include "adc_hal.hh"

#ifndef HAL_REGISTRY_HH
#define HAL_REGISTRY_HH

// HAL registry for dependency injection
// Manages all hardware abstraction layer instances
class HalRegistry {
public:
    HalRegistry() = default;
    ~HalRegistry() = default;
    
    // Register HAL instances
    void register_adc(const std::string& name, std::shared_ptr<IADC> adc);
    void register_onewire(const std::string& name, std::shared_ptr<IOnewireBus> bus);
    
    // Retrieve HAL instances
    std::shared_ptr<IADC> get_adc(const std::string& name) const;
    std::shared_ptr<IOnewireBus> get_onewire(const std::string& name) const;
    
    // Check if instance exists
    bool has_adc(const std::string& name) const;
    bool has_onewire(const std::string& name) const;
    
    // Clear all registrations
    void clear();

private:
    std::unordered_map<std::string, std::shared_ptr<IADC>> adcs_;
    std::unordered_map<std::string, std::shared_ptr<IOnewireBus>> onewire_buses_;
    
    constexpr static char const *TAG = "HAL_REGISTRY";
};

#endif