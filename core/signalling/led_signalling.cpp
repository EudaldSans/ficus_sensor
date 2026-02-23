#include "timer.hh"

#include "led_signalling.hh"

/**
 * @brief Sets the LED to a Solid state
 * 
 * @param on Wheter the led is ON or OFF
 * @return @c FIC_OK
 */
fic_error_t LEDSignaler::set_solid(bool on) {
    std::lock_guard<std::mutex> lock(_mutex);

    _new_request = true;
    _new_signal = {
        .steps = 1,
        .cycles = INFINITE_CYCLES,
        .pattern = {}
    };

    _new_signal.pattern[0] = {on, 0xFFFFFFFF};
    
    return FIC_OK;
}

/**
 * @brief Makes the LED blink ON and OFF
 * 
 * @param on_time_ms Time ON in ms
 * @param off_time_ms Time OFF in ms
 * @param cycles How many cycles to run signal for
 * @return @c FIC_OK
 */
fic_error_t LEDSignaler::set_blink(uint32_t on_time_ms, uint32_t off_time_ms, int32_t cycles) {
    std::lock_guard<std::mutex> lock(_mutex);

    _new_request = true;
    _new_signal = {
        .steps = 2,
        .cycles = cycles,
        .pattern = {}
    };

    _new_signal.pattern[0] = {true, on_time_ms};
    _new_signal.pattern[1] = {false, off_time_ms};

    return FIC_OK;
}

/**
 * @brief Starts a custom signal
 * 
 * @param pattern_composition Think about a limit
 * @param cycles How many full cicles to run the pattern
 * @return @c FIC_OK
 */
fic_error_t LEDSignaler::set_custom_signal(const std::vector<LED_action_t> pattern_composition, int32_t cycles) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (pattern_composition.size() > MAX_STEPS_IN_LED_SIGNAL) return FIC_ERR_INVALID_ARG;
    if (pattern_composition.size() == 0) return FIC_ERR_INVALID_ARG;
    
    _new_request = true;
    _new_signal = {
        .steps = static_cast<uint16_t>(pattern_composition.size()),
        .cycles = cycles,
        .pattern = {}
    };

    std::copy(pattern_composition.begin(), pattern_composition.end(), _new_signal.pattern.begin());
    
    return FIC_OK;
}   

/**
 * @brief Sets up the signaller.
 */
void LEDSignaler::setup() {
    _led.off();
}

/**
 * @brief Runs a tic of the signaller 
 */
void LEDSignaler::update(uint64_t now) {
    if (_new_request) {
        std::lock_guard<std::mutex> lock(_mutex);
        _current_step = 0;
        _remaining_cycles = _new_signal.cycles;
        _active_steps = _new_signal.steps;
        std::copy(_new_signal.pattern.begin(), _new_signal.pattern.end(), _pattern.begin());

        perform_action(_pattern[0], now);
        _new_request = false;
    }

    if (_remaining_cycles == 0 || _active_steps == 0)  return;
    if (_step_timer.has_expired(now) == false)  return;

    _current_step++;

    if (_current_step >= _active_steps) {
        _current_step = 0;
        if (_remaining_cycles > 0) {
            _remaining_cycles--;
        }

        if (_remaining_cycles == 0) {
            _led.off();
            return;
        }
    } 

    perform_action(_pattern[_current_step], now);
}

/**
 * @brief Runs any @c LED_action_t
 * 
 * @param action The @c LED_action_t to run
 */
void LEDSignaler::perform_action(LED_action_t action, uint64_t now) {
    _step_timer.update_duration(action.duration_ms);
    _step_timer.reset(now);
    
    if (action.on) {
        _led.on();
    } else {
        _led.off();
    }  
}
