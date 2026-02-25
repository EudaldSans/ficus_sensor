#ifndef FIC_TIME_HH
#define FIC_TIME_HH

#include <inttypes.h>

class ITimeSource {
public:
    virtual ~ITimeSource() = default;
    
    static ITimeSource& instance() { return *_instance; }
    static void set_instance(ITimeSource* src) { _instance = src; }

    static uint64_t get_time_ms() { 
        if (_instance != nullptr) return _instance->_get_time_ms(); 
        return 0; 
    }

protected:
    virtual uint64_t _get_time_ms() = 0;

private:
    static ITimeSource* _instance;
};


class ITimeDelay {
public:
    virtual ~ITimeDelay() = default;

    static ITimeDelay& instance() { return *_instance; }
    static void set_instance(ITimeDelay* delay) { _instance = delay; }

    static void delay_ms(uint32_t ms) { 
        if (_instance != nullptr) _instance->_delay_ms(ms); 
    }

protected: 
    virtual void _delay_ms(uint32_t ms) = 0;
private:
    static ITimeDelay* _instance;
};


#endif