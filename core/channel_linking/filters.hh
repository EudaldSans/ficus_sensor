#include <cmath>

/**
 * @brief No-op filter, lets all the values through
 * 
 * @tparam T The type of the value being filtered
 */
template<typename T>
struct Filter {
    inline virtual bool allows(uint32_t /*now*/, T /*v*/) { return true; }
    inline virtual void apply(uint32_t /*now*/, T /*v*/) { return; }
};

/**
 * @brief Lets values through only if difference between previous value is
 * greater than a threshold.
 * 
 * @tparam T The type of the value being filtered
 */
template<typename T>
struct DeadBand : public Filter<T> {
    DeadBand(T threshold) : _threshold(threshold) {}

    inline bool allows(uint32_t /*now*/, T v) override {
        if (!_has_value) {
            return true; // Always allow the very first sample
        }
        return get_diff(v, _last_value) >= _threshold;
    }

    inline void apply(uint32_t /*now*/, T v) override {
        _last_value = v;
        _has_value = true;
    }

private:
    T _last_value{};
    const T _threshold;
    bool _has_value = false;

    /**
     * @brief Calculates the difference between values without the risk of unsigned overflow
     * 
     * @param a 
     * @param b 
     * @return 
     */
    static inline T _get_diff(T a, T b) {
        if constexpr (std::is_unsigned_v<T>) {
            return (a >= b) ? (a - b) : (b - a);
        } else {
            return std::abs(a - b);
        }
    }
};

/**
 * @brief Lets values torough only if enough time has passed since the last report
 * 
 * @tparam T The type of the value being filtered
 */
template<typename T>
struct RateLimiter {
    RateLimiter(uint32_t interval_ms) : _interval_ms(interval_ms) {}
    
    inline bool allows(uint32_t now, T /*v*/) { return (now - _last_time) >= _interval_ms; }
    inline void apply(uint32_t now,  T /*v*/) { _last_time = now; }

private:
    uint32_t _last_time = 0;
    const uint32_t _interval_ms;
};

/**
 * @brief Applies a chain of filters to a value, in the order they are specified.
 * 
 * @tparam Steps all the filters to apply, lets value through only if all filters apply.
 * @tparam T the type of the value to apply the filters to
 */
template <typename... Steps>
struct AndFilterChain {
    template <typename T>
    static constexpr bool allows(uint32_t now, T v) {
        bool apply = false;
        ((apply &= Steps::apply(now, v)), ...);
        return apply;
    }

    template <typename T>
    static constexpr void apply(uint32_t now, T v) {
        ((Steps::apply(now, v)), ...);
    }
};

/**
 * @brief Applies a chain of filters to a value, in the order they are specified.
 * 
 * @tparam Steps all the filters to apply, lets value through at least one of the filters apply.
 * @tparam T the type of the value to apply the filters to
 */
template <typename... Steps>
struct OrFilterChain {
    template <typename T>
    static constexpr bool allows(uint32_t now, T v) {
        bool apply = false;
        ((apply |= Steps::apply(now, v)), ...);
        return apply;
    }

    template <typename T>
    static constexpr void apply(uint32_t now, T v) {
        ((Steps::apply(now, v)), ...);
    }
};