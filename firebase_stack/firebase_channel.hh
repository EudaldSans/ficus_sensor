#ifndef FIREBASE_CHANNEL_HH
#define FIREBASE_CHANNEL_HH

#include <string>

#include "channels.hh"

template<typename T>
struct firebase_channel {
    value_t<T> value;
    const std::string_view name;
    const std::string_view unit;

    firebase_channel(std::string_view n, std::string_view u) : name(n), unit(u) {}
    firebase_channel(std::string_view n) : name(n), unit("") {}
};

using AnyChannelPtr = std::variant<firebase_channel<int>*, firebase_channel<float>*, firebase_channel<bool>*>;

#endif