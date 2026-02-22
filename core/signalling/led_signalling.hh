#include <mutex>
#include <atomic>

#include "fic_errors.hh"
#include "task.hh"

#include "led_hal.hh"
#include "timer.hh"


#ifndef LED_SIGNALLING_HH
#define LED_SIGNALLING_HH

#define MAX_STEPS_IN_SIGNAL 16
#define INFINITE_CYCLES -1

struct LED_action_t {
    bool on;
    uint32_t duration_ms;
};

class LEDSignaler : public ITask {
public:
    LEDSignaler(ILightable &led) : _led(led) {};
    ~LEDSignaler() = default;

    fic_error_t set_solid(bool on);
    fic_error_t set_blink(uint32_t on_time_ms, uint32_t off_time_ms, int32_t cycles);
    fic_error_t set_custom_signal(const std::vector<LED_action_t> pattern_composition, int32_t cycles);   
    
private: 
    struct signal_request_t {
        uint16_t steps = 0;
        int32_t cycles = 0;
        std::array<LED_action_t, MAX_STEPS_IN_SIGNAL> pattern;
    };

    void setup();
    void update(uint64_t now);
    void perform_action(LED_action_t action, uint64_t now);

    ILightable &_led;

    std::array<LED_action_t, MAX_STEPS_IN_SIGNAL> _pattern;

    uint16_t _current_step = 0;
    uint16_t _active_steps = 0;
    int32_t _remaining_cycles = 0;

    Timer _step_timer;
    std::atomic<bool> _new_request{false};
    signal_request_t _new_signal;

    std::mutex _mutex;
};

struct RGB_action_t {
    bool on;
    Color color;
    uint32_t duration_ms;
};

class RGBSignaler : public ITask {
public:
    RGBSignaler(IColorable &led) : _rgb_led(led) {};
    ~RGBSignaler() = default;

    fic_error_t set_solid(Color color);
    fic_error_t set_blink(Color color_1, uint32_t color_1_time_ms, Color color_2, uint32_t off_time_ms, int32_t cycles);
    fic_error_t set_custom_signal(const std::vector<RGB_action_t> pattern_composition, int32_t cycles);   
    
private: 
    void setup();
    void update(uint64_t now);

    IColorable &_rgb_led;

    std::array<RGB_action_t, MAX_STEPS_IN_SIGNAL> _pattern;

    uint16_t _current_step = 0;
    uint16_t _active_steps = 0;
    int32_t _remaining_cycles = 0;

    uint64_t _last_action_time_ms;
};


#endif