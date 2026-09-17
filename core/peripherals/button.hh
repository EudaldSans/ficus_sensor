#ifndef BUTTON_HH
#define BUTTON_HH

#include "gpio_hal.hh"

class ButtonBase {
public:
    // Returns true if the button is currently pressed
    bool is_pressed() const {
        bool raw_state = pin_.get_state();
        if constexpr (active_low) {
            return !raw_state;
        } else {
            return raw_state;
        }
    }

protected:
    ButtonBase(IGPIO& pin, bool active_low) : pin_(pin), _active_low(active_low) {}

private:
    IGPIO& pin_;
    const bool _active_low;

    uint32_t _press_time{0};
};

// Active low by default (internal/external pull-up, pressed = LOW/0)
template <bool ActiveLow = true>
class Button : public ButtonBase {
public:
    explicit Button(IGPIO& pin) : ButtonBase(pin, ActiveLow) {}
};

#endif // BUTTON_HH