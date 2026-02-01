#include <functional>
#include <memory>

#include "channels.hh"

#ifndef CHANNEL_LINKING_ENDPOINT_HH
#define CHANNEL_LINKING_ENDPOINT_HH


class ChannelEndpoint {
    public:
        ChannelEndpoint() = default;
        ~ChannelEndpoint() = default;

        template <typename T>
        InputChannel<T>* add_input_channel(std::string id, std::function<void(const T&)> cb) {
            _input_channels[id] = std::make_shared<InputChannel<T>>(cb);
            return get_input<T>(id);
        }

        template <typename T>
        OutputChannel<T>* add_output_channel(std::string id) {
            _output_channels[id] = std::make_shared<OutputChannel<T>>();
            return get_output<T>(id);
        }

        template <typename T>
        OutputChannel<T>* get_output(const std::string& name) {
            if (_output_channels.find(name) == _output_channels.end()) return nullptr;
            return static_cast<OutputChannel<T>*>(_output_channels[name].get());
        }

        template <typename T>
        InputChannel<T>* get_input(const std::string& name) {
            if (_input_channels.find(name) == _input_channels.end()) return nullptr;
            return static_cast<InputChannel<T>*>(_input_channels[name].get());
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<ChannelBase>> _input_channels;
        std::unordered_map<std::string, std::shared_ptr<ChannelBase>> _output_channels;

        constexpr static char const *TAG = "CHANNEL ENDPOINT";
};


#endif