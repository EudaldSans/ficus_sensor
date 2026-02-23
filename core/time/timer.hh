#include <inttypes.h>

#ifndef TIMER_HH
#define TIMER_HH

struct Timer {
    uint64_t _activation_time = 0;
    uint64_t _duration_ms = 0;
    
    /**
     * @brief Resets the timer 
     */
    void reset(uint64_t now) { _activation_time = now; }

    /**
     * @brief Updates the duration of the timer
     * 
     * @param duration_ms Duration in ms
     */
    void update_duration(uint64_t duration_ms) { _duration_ms = duration_ms; }

    /**
     * @brief Checks expiration of the timer
     * 
     * @param now Current time, use in intensive loops where multiple timers are checked
     * @return @c true if timer has expired
     */
    bool has_expired(uint64_t now) {      
        if (now - _activation_time >= _duration_ms) {
            // Do this to avoid phase shifting in long loops
            _activation_time += _duration_ms; 
            return true;
        }
        return false;
    }
};

#endif