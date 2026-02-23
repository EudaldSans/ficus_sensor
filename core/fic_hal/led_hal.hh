#include <vector>
#include <memory>

#include "fic_errors.hh"
#include "task.hh"


#ifndef LED_HAL_HH
#define LED_HAL_HH

class ILEDLifecycle {
public: 
    virtual ~ILEDLifecycle() = default;

    virtual fic_error_t init() = 0;
    virtual fic_error_t deinit() = 0;
};

class ILightable {
public: 
    virtual ~ILightable() = default;

    virtual fic_error_t off() = 0;
    virtual fic_error_t on() = 0;
};

struct Color {
    uint32_t red, green, blue;
};

class IColorable : public ILightable {
public: 
    virtual ~IColorable() = default;

    virtual fic_error_t set_color(Color color) = 0;
};

#endif