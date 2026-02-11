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
                     std::vector<std::shared_ptr<IConversion<producerType>>> conversion_pipeline) {
        
        auto* outCh = producer.get_output<producerType>(outName);
        auto* inCh = consumer.get_input<consumerType>(inName);
        
        // Signal conversions and type conversion
        std::function<void(const producerType&)> adapter = [inCh, conversion_pipeline](const producerType& data) {
            producerType processed_value = data;

            for (const auto& stage : conversion_pipeline) {
                processed_value = stage->convert(processed_value);
            }

            consumerType final_data = static_cast<consumerType>(processed_value);
            
            inCh->receive(final_data);
        };

        outCh->connect(adapter);
        
        ESP_LOGI(TAG, "Linked %s -> %s", outName.data(), inName.data());
    }

    private: 
        constexpr static char const *TAG = "ROUTER";
};

#endif