#include "hal_registry.hh" 
#include "esp_log.h"


void HalRegistry::register_adc(const std::string& name, std::shared_ptr<IADC> adc) {
    if (adc == nullptr) {
        ESP_LOGW(TAG, "Attempted to register null ADC with name: %s", name.c_str());
        return;
    }
    adcs_[name] = adc;
    ESP_LOGI(TAG, "Registered ADC: %s", name.c_str());
}

void HalRegistry::register_onewire(const std::string& name, std::shared_ptr<IOnewireBus> bus) {
    if (bus == nullptr) {
        ESP_LOGW(TAG, "Attempted to register null OneWire bus with name: %s", name.c_str());
        return;
    }
    onewire_buses_[name] = bus;
    ESP_LOGI(TAG, "Registered OneWire bus: %s", name.c_str());
}

std::shared_ptr<IADC> HalRegistry::get_adc(const std::string& name) const {
    auto it = adcs_.find(name);
    if (it != adcs_.end()) {
        return it->second;
    }
    ESP_LOGE(TAG, "ADC not found: %s", name.c_str());
    return nullptr;
}

std::shared_ptr<IOnewireBus> HalRegistry::get_onewire(const std::string& name) const {
    auto it = onewire_buses_.find(name);
    if (it != onewire_buses_.end()) {
        return it->second;
    }
    ESP_LOGE(TAG, "OneWire bus not found: %s", name.c_str());
    return nullptr;
}

bool HalRegistry::has_adc(const std::string& name) const {
    return adcs_.find(name) != adcs_.end();
}

bool HalRegistry::has_onewire(const std::string& name) const {
    return onewire_buses_.find(name) != onewire_buses_.end();
}

void HalRegistry::clear() {
    adcs_.clear();
    onewire_buses_.clear();
    
    ESP_LOGI(TAG, "Cleared all HAL registrations");
}