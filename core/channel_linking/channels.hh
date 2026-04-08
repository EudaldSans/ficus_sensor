#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

#include "fic_log.hh"

#include <vector>

/**
 * @brief POD holding the value and flags of the channel
 * 
 * @tparam T Type of the value
 */
template <typename T>
struct __attribute__((aligned(4),packed))  value_t {
private:
    T value;
    uint8_t flags = 0x01;

public:

    static constexpr uint8_t MASK_VALID = 0x01;
    static constexpr uint8_t MASK_NEW   = 0x02;

    /**
     * @brief Updates the value and sets the new flag
     * 
     * @param val The value itself
     */
    inline void update(T val) {
        value = val;
        flags |= MASK_NEW; 
    }

    inline void validate() { flags |= MASK_VALID; }
    inline void invalidate() { flags &= ~MASK_VALID; }

    bool is_valid() const { return flags & 0x01; }
    bool is_new() const { return flags & 0x02; }
    inline T peek() const { return value; }

    /**
     * @brief Returns the value and clears the new flag
     */
    inline T consume() {
        flags &= ~MASK_NEW;
        return value;
    }
};

/**
 * @brief Tiny contract so the router can hold an single array of ILinks, 
 * useful if a channel requires a custom link
 */
struct ILink {
    virtual void sync() = 0;
};

/**
 * @brief Basic link structure
 * 
 * @tparam From Type of incoming @c value_t
 * @tparam To Type of outgoing @c value_t
 */
template <typename From, typename To>
struct ChannelLink : public ILink {
    value_t<From>& src;
    value_t<To>& dest;

    ChannelLink(value_t<From>& s, value_t<To>& d) : src(s), dest(d) {}

    /**
     * @brief Synchronizes the values of the channels
     */
    void sync() {
        dest.update(src.peek());

        if (src.is_valid()) {
            dest.validate();
        } else {
            dest.invalidate();
        }
    }
};

#endif