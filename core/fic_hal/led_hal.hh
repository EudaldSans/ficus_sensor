#include <vector>
#include <memory>

#include "fic_errors.hh"
#include "task.hh"


#ifndef LED_HAL_HH
#define LED_HAL_HH

class ILEDLifecycle {
public: 
    ILEDLifecycle() = default;
    virtual ~ILEDLifecycle() = default;

    ILEDLifecycle(const ILEDLifecycle&) = delete;            
    ILEDLifecycle& operator=(const ILEDLifecycle&) = delete; 
    ILEDLifecycle(ILEDLifecycle&&) = delete; 

    virtual fic_error_t init() = 0;
    virtual fic_error_t deinit() = 0;
};

class ILightable {
public: 
    ILightable() = default;
    virtual ~ILightable() = default;

    ILightable(const ILightable&) = delete;            
    ILightable& operator=(const ILightable&) = delete; 
    ILightable(ILightable&&) = delete; 

    virtual fic_error_t off() = 0;
    virtual fic_error_t on() = 0;
};

struct Color {
    uint32_t red, green, blue;
};

#define LED_WHITE {255, 255, 255}
#define LED_RED {255, 0, 0}
#define LED_GREEN {0, 255, 0}
#define LED_BLUE {0, 0, 255}
#define LED_OFF {0, 0, 0}

class IColorable : public ILightable {
public: 
    IColorable() = default;
    virtual ~IColorable() = default;

    IColorable(const IColorable&) = delete;            
    IColorable& operator=(const IColorable&) = delete; 
    IColorable(IColorable&&) = delete; 

    virtual fic_error_t set_color(Color color) = 0;
};

#endif