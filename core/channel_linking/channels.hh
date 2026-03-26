#include "fic_log.hh"

#include <vector>

#include "conversions.hh"

#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

template <typename T>
struct __attribute__((packed))  value_t {
    T value;
    bool is_valid : 1 = false;
    bool is_new: 1 = true;
};

// THINK ABOUT: should I separate into inputs and outputs?
template <typename T>
class ChannelBase {
    ChannelBase() = default;
    virtual ~ChannelBase() = default;   

    bool is_valid() const { return _value.is_valid; }
    bool is_new() const { return _value.is_new; }

    T get_value() const { return _value.value; }

    bool set_value(T new_value, bool is_valid) {
        _value.is_new = true;
        _value.is_valid = is_valid;
        _value.value = new_value;
    }

private:
    value_t<T> _value;
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