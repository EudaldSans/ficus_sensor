#ifndef CHANNEL_LINKING_CHANNELS_H
#define CHANNEL_LINKING_CHANNELS_H

#include "fic_log.hh"

#include "conversions.hh"
#include "filters.hh"

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
 * @brief Basic link structure
 * 
 * @tparam From Type of incoming @c value_t
 * @tparam To Type of outgoing @c value_t
 * @tparam Converter Conversion chain for the value, defaults to no-op
 * @tparam Filter Filter chain for the value, defaults to no-op
 */
template <typename From, typename To, typename Converter = NoConv, typename Filter = PassAll>
struct ChannelLink {
private:
    uint8_t _last_seen_version = 0;
    value_t<From>& src;
    value_t<To>& dest;

    Filter _filter_state{};

public:
    constexpr ChannelLink(value_t<From>& s, value_t<To>& d) : src(s), dest(d) {}

    /**
     * @brief Synchronizes the values of the channels
     */
    void sync(uint32_t now) {
        if (static_cast<uint8_t>(src.get_version() - _last_seen_version) == 0) {return;}
        auto const& current_val = src.peek();

        if constexpr (!std::is_same_v<Filter, PassAll>) {
            if (!_filter_state.allows(now, current_val)) return;
            _filter_state.apply(now, current_val);
        }

        _last_seen_version = src.get_version();
        dest.update(static_cast<To>(Converter::apply(current_val)));
        dest.make_new();

        if (src.is_valid()) {
            dest.validate();
        } else {
            dest.invalidate();
        }
    }
};

#endif