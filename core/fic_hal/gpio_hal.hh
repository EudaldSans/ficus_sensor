#ifndef GPIO_HAL_HH
#define GPIO_HAL_HH

#include "fic_errors.hh"

#include <inttypes.h>

typedef void (*gpio_callback) (uint32_t activation_time_ms);

struct GPIOConfig;

class GPIOLifecycle {
public:
    GPIOLifecycle(const GPIOConfig& config) : _config(config) {};
    ~GPIOLifecycle() = default;

    fic_error_t init();
    fic_error_t deinit();
private:
    const GPIOConfig& _config;
};

class GPIOInput {
public:
    GPIOInput(gpio_callback cb = nullptr) : _cb(cb) {};
    ~GPIOInput() = default;

    bool get_status();

    void set_callback(gpio_callback cb) {_cb = cb;}
private:
    gpio_callback _cb;
};

class GPIOOutput {
public:
    GPIOOutput() = default;
    ~GPIOOutput() = default;

    void on();
    void off();
};


#endif