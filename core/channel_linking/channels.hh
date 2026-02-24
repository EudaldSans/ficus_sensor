#include "fic_log.hh"
#include "esp_check.h"

#include <vector>

#include "conversions.hh"

#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

struct ChannelBase {
    ChannelBase() = default;
    virtual ~ChannelBase() = default;        
};

template <typename T>
class InputChannel : public ChannelBase {
    using Callback = std::function<void(const T&)>;

    public:
        InputChannel(Callback cb) : ChannelBase(), callback(cb) {}
        ~InputChannel() override = default;

        void receive(const T& value) {
            value_ = value;
            if (callback) {
                callback(value_);
            }
        }

        T get_value() const {
            return value_;
        }

    private:
        T value_;
        
        Callback callback;
};


template <typename T>
class OutputChannel : public ChannelBase {
    public:
        OutputChannel() : ChannelBase() {}
        ~OutputChannel() override = default;

        void connect(std::function<void(const T&)> listener) {
            _listeners.push_back(listener);
        }

        void emit(const T& value) {
            _value = value;

            for (const auto& listener : _listeners) {
                listener(_value);
            }
        }

    private:
        T _value;
        std::vector<std::function<void(const T&)>> _listeners;
};



#endif