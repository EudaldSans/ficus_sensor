#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

#include "fic_log.hh"

#include "conversions.hh"

#include <inttypes.h>
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
    uint8_t flags = 0x00;
    uint8_t version = 0;

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
        version++;
    }

    /**
     * @brief Sets the valid flag to @c true
     */
    inline void validate() { flags |= MASK_VALID; }

    /**
     * @brief Sets the valid flag to @c false
     */
    inline void invalidate() { flags &= ~MASK_VALID; }

    /**
     * @brief Checks if the valid flag is set
     */
    inline bool is_valid() const { return flags & MASK_VALID; }

    /**
     * @brief Checks if the new flag is set
     */
    inline bool is_new() const { return flags & MASK_NEW; }

    /**
     * @brief Sets the new flag to @c true
     */
    inline void make_new() { flags |= MASK_NEW; }

    /**
     * @brief Allows to peek the value without clearing the new flag
     */
    inline T peek() const { return value; }

    /**
     * @brief Returns the value and clears the new flag
     */
    inline T consume() {
        flags &= ~MASK_NEW;
        return value;
    }

    /**
     * @brief Geter for value version
     * 
     * @return uint8_t The version
     */
    uint8_t get_version() const { return version; }
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
template <typename From, typename To, typename Converter = NoConv>
struct ChannelLink : public ILink {
private:
    uint8_t _last_seen_version = 0;

public:
    value_t<From>& src;
    value_t<To>& dest;

    ChannelLink(value_t<From>& s, value_t<To>& d) : src(s), dest(d) {}

    /**
     * @brief Synchronizes the values of the channels
     */
    void sync() {
        if (static_cast<uint8_t>(src.get_version() - _last_seen_version) == 0) {return;}
        _last_seen_version = src.get_version();

        dest.update(static_cast<To>(Converter::apply(src.peek())));
        dest.make_new();

        if (src.is_valid()) {
            dest.validate();
        } else {
            dest.invalidate();
        }
    }
};

#endif