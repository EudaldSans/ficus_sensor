#ifndef BUTTON_HH
#define BUTTON_HH

#include "gpio_hal.hh"

class ButtonBase {
public:
    // Returns true if the button is currently pressed
    bool is_pressed() const {
        bool raw_state = _pin.get_state();
        if constexpr (_active_low) {
            return !raw_state;
        } else {
            return raw_state;
        }
    }

protected:
    ButtonBase(GpioPin& pin, bool active_low) : _pin(pin), _active_low(active_low) {}

private:
    GpioPin& _pin;
    bool _active_low;
};

template <GPIO GpioPin, bool active_low>
class Button : public ButtonBase {
public:
    explicit Button(GpioPin& pin) : ButtonBase(pin, active_low) {}
};

#endif // BUTTON_HH