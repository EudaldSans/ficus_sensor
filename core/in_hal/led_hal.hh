#include <vector>
#include <memory>

#include "in_errors.hh"
#include "task.hh"


#ifndef LED_HAL_HH
#define LED_HAL_HH

class ILEDLifecycle {
public: 
    virtual ~ILEDLifecycle() = default;

    virtual in_error_t init() = 0;
    virtual in_error_t deinit() = 0;
};

class ILightable {
public: 
    virtual ~ILightable() = default;

    virtual in_error_t off() = 0;
    virtual in_error_t on() = 0;
};

struct Color {
    uint8_t r, g, b;
};

class IColorable {
public: 
    virtual ~IColorable() = default;

    virtual in_error_t set_color(Color color) = 0;
};

class ISignaler : public ITask {
public:
    virtual ~ISignaler() = default;

    virtual in_error_t blink(uint32_t on_time, uint32_t off_time, uint32_t blinks) = 0;
    virtual in_error_t custom_signal(std::vector<uint32_t> signal_composition, bool repeat) = 0;        
};



#endif