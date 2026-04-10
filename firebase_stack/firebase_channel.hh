#ifndef FIREBASE_CHANNEL_HH
#define FIREBASE_CHANNEL_HH

#include <string>

#include "channel_linking/channels.hh"

template<typename T>
struct firebase_channel {
    value_t<T> value;
    const std::string_view name;
    const std::string_view unit;

    firebase_channel(const std::string_view& n, const std::string_view& u) : name(n), unit(u) {}
    firebase_channel(const std::string_view& n) : name(n), unit("") {}
};

#endif