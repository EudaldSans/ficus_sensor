#include "esp_log.h"
#include "esp_check.h"

#include <vector>

#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

struct ChannelBase {
    size_t type_hash; // Store a hash of the type
    ChannelBase(size_t t) : type_hash(t) {}
    virtual ~ChannelBase() = default;        
};

template <typename T>
class InputChannel : public ChannelBase {
    public:
        InputChannel(Callback cb) : ChannelBase(typeid(T).hash_code()), callback(nullptr) {}
        ~InputChannel() override = default;

        void set_value(const T& value) {
            value_ = value;
            if (callback) {
                callback(value_);
            }
        }

        void set_callback(Callback cb) {
            callback = cb;
        }

        T get_value() const {
            return value_;
        }

    private:
        T value_;

        using Callback = std::function<void(const T&)>;
        Callback callback;

        constexpr static char const *TAG = "INPUT CHANNEL";
};


template <typename T>
class OutputChannel : public ChannelBase {
    public:
        OutputChannel() : ChannelBase(typeid(T).hash_code()) {}
        ~OutputChannel() override = default;

        void connect(InputChannel<T>* listener) {
            listeners_.push_back(listener);
        }

        void emit(const T& value) {
            value_ = value;
            for (const auto& listener : listeners_) {
                listener->set_value(value_);
            }
        }

    private:
        T value_;
        std::vector<InputChannel<T>*> listeners_;

        constexpr static char const *TAG = "OUTPUT CHANNEL";
};



#endif