#ifndef TIMER_HH
#define TIMER_HH

#include <inttypes.h>

#include "time_source.hh"

struct Timer {
    uint64_t _activation_time_ms = 0;
    uint32_t _duration_ms = 0;
    bool _active = false;
    bool _periodic = true;

    /**
     * @brief Starts the timer with an externally provided time
     * 
     * @param duration_ms Timeout duration in ms
     * @param now The current time in ms
     * @param periodic Whether the timer is periodic
     */
    void start(uint32_t duration_ms, uint64_t now, bool periodic = true) {
        _duration_ms = duration_ms;
        _activation_time_ms = now;
        _active = true;
        _periodic = periodic;
    }

    /**
     * @brief Starts the timer using the time source
     * 
     * @param duration_ms Timeout duration in ms
     * @param periodic Whether the timer is periodic
     */
    void start(uint32_t duration_ms, bool periodic = true) {
        start(duration_ms, TimeSource::get_time_ms(), periodic);
    }
    
    /**
     * @brief Resets the timer 
     */
    void reset(uint64_t now) { 
        _activation_time_ms = now; 
        _active = true;
    }

    /**
     * @brief Updates the duration of the timer
     * 
     * @param duration_ms Duration in ms
     */
    void update_duration(uint32_t duration_ms) { 
        _duration_ms = duration_ms; 
        _active = true;
    }

    /**
     * @brief Checks expiration of the timer with external time, use in intensive loops where multiple timers are checked
     * 
     * @param now Current time
     * @return @c true if timer has expired
     */
    bool has_expired(uint64_t now) {   
        if (!_active) return false;   

        if (now - _activation_time_ms >= _duration_ms) {
            if (!_periodic) {
                _active = false;
            } else {
                _activation_time_ms += _duration_ms; // Avoid phase shifting in long loops
            }
            return true;
        }
        return false;
    }

    /**
     * @brief Checks expiration of the timer using the time source, use in non-intensive loops where only one timer is checked
     * 
     * @return @c true if timer has expired
     */
    bool has_expired() { 
        return has_expired(TimeSource::get_time_ms()); 
    }
};

#endif