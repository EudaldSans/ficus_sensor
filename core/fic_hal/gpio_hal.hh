#ifndef GPIO_HAL_HH
#define GPIO_HAL_HH

#include <inttypes.h>
#include <concepts>

typedef void (*gpio_callback) (uint32_t activation_time_ms);

template <typename T>
concept GPIOInputConcept = requires(T a, gpio_callback cb) {
    { a.get_status() } -> std::same_as<bool>;
    { a.set_callback(cb) } -> std::same_as<void>;
};

template <typename T>
concept GPIOOutput = requires(T a) {
    { a.on() } -> std::same_as<void>;
    { a.off() } -> std::same_as<void>;
};

#endif