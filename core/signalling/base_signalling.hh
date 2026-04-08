#ifndef BASE_SIGNALLING_HH
#define BASE_SIGNALLING_HH

#include <mutex>
#include <atomic>

#include "fic_errors.hh"
#include "task.hh"

#include "led_hal.hh"
#include "timer.hh"

template <typename ActionT, size_t MaxSteps>
class SignalerBase : public IContinuousTask {
public:
    fic_error_t set_custom_signal(const std::vector<ActionT> pattern_composition, int32_t cycles) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (pattern_composition.size() > MaxSteps || pattern_composition.empty())
            return FIC_ERR_INVALID_ARG;

        _new_request = true;
        _new_signal = { .steps = static_cast<uint16_t>(pattern_composition.size()),
                        .cycles = cycles, .pattern = {} };
        std::copy(pattern_composition.begin(), pattern_composition.end(),
                  _new_signal.pattern.begin());
        return FIC_OK;
    }

protected:
    virtual void perform_action(const ActionT& action, uint64_t now) = 0;
    virtual void turn_off() = 0;

    /**
     * @brief Helper method so every child class can implement simple methods without bloat.
     * 
     * @param steps The steps of the signal pattern.
     * @param cycles The number of cycles to repeat the pattern.
     * @return fic_error_t @c FIC_OK if the signal was successfully submitted, otherwise an error code.
     */
    fic_error_t submit_signal(std::initializer_list<ActionT> steps, int32_t cycles) {
        std::lock_guard<std::mutex> lock(_mutex);
        _new_request = true;
        _new_signal = { .steps = static_cast<uint16_t>(steps.size()),
                        .cycles = cycles, .pattern = {} };
        std::copy(steps.begin(), steps.end(), _new_signal.pattern.begin());
        return FIC_OK;
    }

    Timer _step_timer;

private:
    struct signal_request_t {
        uint16_t steps = 0;
        int32_t cycles = 0;
        std::array<ActionT, MaxSteps> pattern;
    };

    void setup() override { turn_off(); }

    void update(uint64_t now) override {
        if (_new_request) {
            std::lock_guard<std::mutex> lock(_mutex);
            _current_step = 0;
            _remaining_cycles = _new_signal.cycles;
            _active_steps = _new_signal.steps;
            std::copy(_new_signal.pattern.begin(), _new_signal.pattern.end(),
                      _pattern.begin());
            perform_action(_pattern[0], now);
            _new_request = false;
        }

        if (_remaining_cycles == 0 || _active_steps == 0) return;
        if (!_step_timer.has_expired(now)) return;

        _current_step++;
        if (_current_step >= _active_steps) {
            _current_step = 0;
            if (_remaining_cycles > 0) _remaining_cycles--;
            if (_remaining_cycles == 0) { turn_off(); return; }
        }
        perform_action(_pattern[_current_step], now);
    }

    std::array<ActionT, MaxSteps> _pattern;

    uint16_t _current_step = 0;
    uint16_t _active_steps = 0;
    int32_t _remaining_cycles = 0;

    std::atomic<bool> _new_request{false};
    signal_request_t _new_signal;

    std::mutex _mutex;
};


#endif