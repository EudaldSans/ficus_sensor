#include "endpoints.hh"
#include "channels.hh"

#ifndef CHANNEL_LINKING_ROUTER_HH
#define CHANNEL_LINKING_ROUTER_HH

class Router {
    public: 
    template <typename T>
    static void link(TransportInterface& producer, const std::string& outName,
                     TransportInterface& consumer, const std::string& inName) {
        
        // 1. Get Typed Pointers
        auto* outCh = producer.getOutput<T>(outName);
        auto* inCh = consumer.getInput<T>(inName);

        // 2. Connect
        // This creates a closure. The type T is baked into the std::function.
        outCh->connect(inCh);
        
        std::cout << "Linked " << outName << " -> " << inName << std::endl;
    }

    private: 
        constexpr static char const *TAG = "ROUTER";
};

#endif