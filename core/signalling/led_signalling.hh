#include <mutex>
#include <atomic>

#include "fic_errors.hh"
#include "task.hh"

#include "led_hal.hh"
#include "timer.hh"

#include "base_signalling.hh"

#ifndef LED_SIGNALLING_HH
#define LED_SIGNALLING_HH

#ifndef INFINITE_CYCLES
#define INFINITE_CYCLES -1
#endif

#define MAX_STEPS_IN_LED_SIGNAL 16

struct LED_action_t {
    bool on;
    uint32_t duration_ms;
};

class LEDSignaler : public SignalerBase<LED_action_t, 16> {
public:
    LEDSignaler(ILightable& led) : _led(led) {}

    fic_error_t set_solid(bool on) {
        return submit_signal({{on, 0xFFFFFFFF}}, INFINITE_CYCLES);
    }

    fic_error_t set_blink(uint32_t on_ms, uint32_t off_ms, int32_t cycles) {
        return submit_signal({{true, on_ms}, {false, off_ms}}, cycles);
    }

protected:
    void perform_action(const LED_action_t& action, uint64_t now) override {
        _step_timer.update_duration(action.duration_ms);
        _step_timer.reset(now);
        action.on ? _led.on() : _led.off();
    }
    void turn_off() override { _led.off(); }

private:
    ILightable& _led;
};

#endif