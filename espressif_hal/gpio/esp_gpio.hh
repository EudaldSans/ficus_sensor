#ifndef ESP_GPIO_HH
#define ESP_GPIO_HH

#include "driver/gpio.h"
#include "gpio_hal.hh"

enum pull_mode_t {
    PULL_DISABLED,
    PULL_UP,
    PULL_DOWN
};

class EspGPIOBase : public IGPIO {
public: 
    void set() override {
        gpio_set_level(pin, 1);
    }

    void reset() override {
        gpio_set_level(pin, 0);
    }

    bool get_state() const override {
        return static_cast<bool>(gpio_get_level(pin));
    }
    
protected:
    EspGPIOBase(gpio_num_t pin, gpio_mode_t mode, gpio_pullup_t pull_up_mode, gpio_pulldown_t pull_down_mode) : pin(pin) {
        gpio_pullup_t pull_up_mode = GPIO_PULLUP_DISABLE;
        gpio_pulldown_t pull_down_mode = GPIO_PULLDOWN_DISABLE;
        
        if constexpr (pull_mode == PULL_DOWN) pull_down_mode = GPIO_PULLDOWN_ENABLE;
        if constexpr (pull_mode == PULL_UP) pull_up_mode = GPIO_PULLUP_ENABLE;

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << pin),
            .mode = mode,
            .pull_up_en = pull_up_mode,
            .pull_down_en = pull_down_mode,
            .intr_type = GPIO_INTR_DISABLE,
        };
        
        gpio_config(&io_conf);
    }

private:
    gpio_num_t pin;
};

template<gpio_num_t pin, gpio_mode_t mode, gpio_pullup_t pull_up_mode, gpio_pulldown_t pull_down_mode>
class EspGPIO : public EspGPIOBase{
public:
    explicit EspGPIO() : EspGPIOBase(pin, mode, pull_up_mode, pull_down_mode) {
};

#endif