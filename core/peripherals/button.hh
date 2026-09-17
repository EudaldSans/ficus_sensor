#ifndef BUTTON_HH
#define BUTTON_HH

#include "gpio_hal.hh"
#include "task.hh"

class ButtonBase : public ITask {
public:
    void setup() override {}
    void update(uint32_t now) override {
        bool raw_state = pin_.get_state();
        bool is_pressed = false;
        
        if (_active_low) {
            is_pressed = !raw_state;
        } else {
            is_pressed = raw_state;
        }

        if (is_pressed && _active) {
            uint32_t activation_time = now - _press_time;
            if (activation_time % _button_event_period_ms < 10) {
                FIC_LOGI(TAG, "Button long press %d", activation_time);
            }
        } else if (is_pressed) {
            FIC_LOGI(TAG, "Button short press");
            _active = true;
            _press_time = now;
        } else {
            _active = false;
        }
    }

protected:
    ButtonBase(IGPIO& pin, bool active_low) : pin_(pin), _active_low(active_low) {}

private:
    IGPIO& pin_;
    const bool _active_low;

    bool _active;
    uint32_t _press_time{0};

    constexpr static char const *TAG = "Button";
    constexpr static uint32_t _button_event_period_ms = 1000;
};

// Active low by default (internal/external pull-up, pressed = LOW/0)
template <uint32_t poll_interval_ms, bool ActiveLow = true>
class Button : public ButtonBase {
public:
    explicit Button(IGPIO& pin) : ButtonBase(pin, ActiveLow) {}
    bool should_run(uint32_t now) override {
        return now % poll_interval_ms == 0;
    }
};

#endif // BUTTON_HH