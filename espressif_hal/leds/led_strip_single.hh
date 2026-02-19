#include "led_strip.h"

#include "led_hal.hh"

#ifndef LEDS_LED_STRIP_SINGLE_HH
#define EDS_LED_STRIP_SINGLE_HH


class LEDStripSingle : public ILEDLifecycle, public IColorable {
public:
    LEDStripSingle(uint8_t pin, led_model_t model);
    ~LEDStripSingle() = default;

    fic_error_t init() override;
    fic_error_t deinit() override;

    fic_error_t off() override;
    fic_error_t on() override;
    fic_error_t set_color(Color color) override;

private: 
    const uint8_t _pin;
    const led_model_t _model;

    Color _color;
    led_strip_handle_t _led_strip;

    bool _initialized;
};


#endif