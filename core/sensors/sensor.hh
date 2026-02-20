#include <string>

#include "fic_errors.hh"

#ifndef SENSOR_H
#define SENSOR_H

class ISensorLifecycle {
public:
    virtual ~ISensorLifecycle() = default; 
    virtual fic_error_t init() = 0; 
    virtual fic_error_t deinit() = 0;
};

class ISensorMetadata {
public: 
    virtual ~ISensorMetadata() = default; 
    virtual std::string_view get_name() const = 0;
    virtual std::string_view get_type() const = 0;
    virtual std::string_view get_unit() const = 0;
};

class ISensorBase : public ISensorLifecycle, public ISensorMetadata {
public:
    virtual ~ISensorBase() = default;
};

template <typename T> 
class ISensor: public virtual ISensorBase {
public: 
    virtual ~ISensor() = default; 
    virtual fic_error_t measure(T &value) = 0;
};

template <typename T> 
class IAsyncSensor: public virtual ISensorBase {
public: 
    virtual ~IAsyncSensor() = default; 
    virtual fic_error_t trigger_measurement(uint16_t &measurement_delay_ms) = 0; 
    virtual fic_error_t get_measurement(T &value) = 0;
    virtual bool is_ready() = 0;
};

#endif