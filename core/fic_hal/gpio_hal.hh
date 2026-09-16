#ifndef GPIO_HAL_HH
#define GPIO_HAL_HH

#include <concepts>

template <typename T>
concept GPIO = requires(T pin) {
    { pin.set() } -> std::same_as<void>;
    { pin.reset() } -> std::same_as<void>;

    { pin.get_state() } -> std::convertible_to<bool>;
};

#endif