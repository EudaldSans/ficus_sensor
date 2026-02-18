#include "fic_errors.hh"
#include "task.hh"

#include "led_hal.hh"


#ifndef LED_SIGNALLING_HH
#define LED_SIGNALLING_HH

enum LED_signal_t {
    LED_OFF,
    LED_ON,
    LED_COLOR
};

struct LED_action_t {
    bool on,
    uint32_t duration_ms,
};

class LEDSignaler : public ITask {
public:
    LEDSignaler(&ILightable led) : _led(led) {}
    ~LEDSignaler() = default;

    fic_error_t set_solid(bool on);
    fic_error_t set_blink(uint32_t on_time_ms, uint32_t off_time_ms, uint32_t cycles) = 0;
    fic_error_t set_custom_signal(std::vector<LED_action_t> pattern_composition, uint32_t cycles) = 0;   
    
private: 
    void setup();
    void update();

    const &ILightable _led;
};

struct RGB_action_t {
    bool on,
    Color color,
    uint32_t duration_ms,
};

class RGBSignaler : public ITask {
public:
    RGBSignaler(&IColorable led) : _led(led) {}
    ~RGBSignaler() = default;

    fic_error_t set_solid(Color color);
    fic_error_t set_blink(Color color_1, uint32_t color_1_time_ms, Color color_2, uint32_t off_time_ms, uint32_t cycles);
    fic_error_t set_custom_signal(std::vector<LED_action_t> pattern_composition, uint32_t cycles);   
    
private: 
    void setup();
    void update();

    const &IColorable _rgb_led;
};


#endif