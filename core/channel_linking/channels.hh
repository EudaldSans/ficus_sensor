#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

#include "fic_log.hh"

#include <vector>

template <typename T>
struct __attribute__((aligned(4),packed))  value_t {
private:
    T value;
    uint8_t flags = 0x01;

public:

    static constexpr uint8_t MASK_VALID = 0x01;
    static constexpr uint8_t MASK_NEW   = 0x02;

    inline void update(T val) {
        value = val;
        flags |= MASK_NEW; 
    }

    inline void validate() { flags |= MASK_VALID; }
    inline void invalidate() { flags &= ~MASK_VALID; }

    bool is_valid() const { return flags & 0x01; }
    bool is_new() const { return flags & 0x02; }
    inline T peek() const { return value; }

    inline T consume() {
        flags &= ~MASK_NEW;
        return value;
    }
};

template <typename From, typename To>
struct ChannelLink {
    value_t<From>& src;
    value_t<To>& dest;

    void sync() {
        if (src.is_new) {
            dest.value = static_cast<To>(src.value);
            dest.is_valid = src.is_valid;
            dest.is_new = true;
        }
    }
};

#endif