#include "channels.hh"

#ifndef CHANNEL_LINKING_ENDPOINT_HH
#define CHANNEL_LINKING_ENDPOINT_HH


class ChannelEndpoint {
    public:
        ChannelEndpoint() = default;
        ~ChannelEndpoint() = default;

        template <typename T>
        void add_input_channel(int id, std::function<void(const T&)> cb) {
            input_channels_[id] = std::make_shared<InputChannel<T>>(cb);
        }

        template <typename T>
        void add_output_channel(int id) {
            output_channels_[id] = std::make_shared<OutputChannel<T>>();
        }

    private:
        std::unordered_map<int, std::shared_ptr<ChannelBase>> input_channels_;
        std::unordered_map<int, std::shared_ptr<ChannelBase>> output_channels_;

        constexpr static char const *TAG = "CHANNEL ENDPOINT";
};


#endif