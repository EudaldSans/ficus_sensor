#include <functional>

#include "esp_log.h"

#include "endpoint.hh"
#include "channels.hh"
#include "conversions.hh"

#ifndef CHANNEL_LINKING_ROUTER_HH
#define CHANNEL_LINKING_ROUTER_HH

class Router {
    public: 
    template <typename producerType, typename consumerType>
    static void link(ChannelEndpoint& producer, const std::string& outName,
                     ChannelEndpoint& consumer, const std::string& inName,
                     std::vector<std::shared_ptr<IConversion<producerType>>> conversions) {
        
        auto* outCh = producer.get_output<producerType>(outName);
        auto* inCh = consumer.get_input<consumerType>(inName);
        
        // Type conversion
        std::function<void(const producerType&)> adapter = [inCh](const producerType& data) {
            consumerType converted_data = static_cast<consumerType>(data); 
            
            inCh->receive(converted_data);
        };

        outCh->connect(adapter, conversions);
        
        ESP_LOGI(TAG, "Linked %s -> %s", outName.data(), inName.data());
    }

    private: 
        constexpr static char const *TAG = "ROUTER";
};

#endif