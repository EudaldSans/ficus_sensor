#ifndef FIC_TIME_HH
#define FIC_TIME_HH

#include <inttypes.h>

class TimeSource {
public:
    virtual ~TimeSource() = default;
    
    static TimeSource& instance() { return *_instance; }
    static void set_instance(TimeSource* src) { _instance = src; }

    static uint64_t get_time_ms() { 
        if (_instance != nullptr) return _instance->_get_time_ms(); 
        return 0; 
    }

protected:
    virtual uint64_t _get_time_ms() = 0;

private:
    static TimeSource* _instance;
};


class TimeDelay {
public:
    virtual ~TimeDelay() = default;

    static TimeDelay& instance() { return *_instance; }
    static void set_instance(TimeDelay* delay) { _instance = delay; }

    static void delay_ms(uint32_t ms) { 
        if (_instance != nullptr) _instance->_delay_ms(ms); 
    }

protected: 
    virtual void _delay_ms(uint32_t ms) = 0;
private:
    static TimeDelay* _instance;
};


#endif