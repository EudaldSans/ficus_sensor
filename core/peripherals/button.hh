#ifndef BUTTON_HH
#define BUTTON_HH

#include "gpio_hal.hh"

class ButtonBase {
public:
    // Returns true if the button is currently pressed
    bool is_pressed() const {
        bool raw_state = pin_.get_state();
        if constexpr (ActiveLow) {
            return !raw_state;
        } else {
            return raw_state;
        }
    }

    // Returns true if the button is currently pressed
    bool is_pressed() const {
        bool raw_state = pin_.get_state();
        if constexpr (ActiveLow) {
            return !raw_state;
        } else {
            return raw_state;
        }
    }

protected:
    ButtonBase(GpioPin& pin) : pin_(pin) {}

private:
    GpioPin& pin_;
};

// Active low by default (internal/external pull-up, pressed = LOW/0)
template <GPIO GpioPin, bool ActiveLow = true>
class Button : public ButtonBase {
public:
    explicit Button(GpioPin& pin) : ButtonBase(pin) {}
};

#endif // BUTTON_HH