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
    /**
     * @brief Sets the state of the GPIO to @c true
     */
    inline void set() override {
        gpio_set_level(_pin, 1);
    }

    /**
     * @brief Sets the state of the GPIO to @c false
     */
    inline void reset() override {
        gpio_set_level(_pin, 0);
    }

    /**
     * @brief Get the state GPIO
     * 
     * @return true 
     * @return false 
     */
    inline bool get_state() const override {
        return static_cast<bool>(gpio_get_level(_pin));
    }
    
protected:
    EspGPIOBase(gpio_num_t pin, gpio_mode_t mode, gpio_pullup_t pull_up_mode, gpio_pulldown_t pull_down_mode) : IGPIO(), _pin(pin) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << _pin),
            .mode = mode,
            .pull_up_en = pull_up_mode,
            .pull_down_en = pull_down_mode,
            .intr_type = GPIO_INTR_DISABLE,
        };
        
        gpio_config(&io_conf);
    }

private:
    const gpio_num_t _pin;
};

template<gpio_num_t pin, gpio_mode_t mode, gpio_pullup_t pull_up_mode, gpio_pulldown_t pull_down_mode>
class EspGPIO : public EspGPIOBase{
public:
    explicit EspGPIO() : EspGPIOBase(pin, mode, pull_up_mode, pull_down_mode) {}
};

#endif