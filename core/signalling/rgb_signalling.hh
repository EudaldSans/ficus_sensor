#include <mutex>
#include <atomic>
#include <array>

#include "fic_errors.hh"
#include "task.hh"

#include "led_hal.hh"
#include "timer.hh"

#include "base_signalling.hh"

#ifndef RGB_SIGNALLING_HH
#define RGB_SIGNALLING_HH

#ifndef INFINITE_CYCLES
#define INFINITE_CYCLES -1
#endif

#define MAX_STEPS_IN_RGB_SIGNAL 24

struct RGB_action_t {
    bool on;
    Color color;
    uint32_t duration_ms;
};

class RGBSignaler : public SignalerBase<RGB_action_t, 24> {
public:
    RGBSignaler(IColorable& led) : _led(led) {}

    fic_error_t set_solid(Color color) {
        bool on = (color.red || color.green || color.blue);
        return submit_signal({{on, color, 0xFFFFFFFF}}, INFINITE_CYCLES);
    }

    fic_error_t set_blink(Color c1, uint32_t t1, Color c2, uint32_t t2, int32_t cycles) {
        auto is_on = [](Color c){ return c.red || c.green || c.blue; };
        return submit_signal({{is_on(c1), c1, t1}, {is_on(c2), c2, t2}}, cycles);
    }

protected:
    void perform_action(const RGB_action_t& action, uint64_t now) override {
        _step_timer.update_duration(action.duration_ms);
        _step_timer.reset(now);
        action.on ? _led.set_color(action.color) : _led.off();
    }
    void turn_off() override { _led.off(); }

private:
    IColorable& _led;
};

#endif