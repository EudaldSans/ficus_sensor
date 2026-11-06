#include "button.hh"
#include "fic_log.hh"

/**
 * @brief Checks button status and updates all listeners
 * 
 * @param now The current task execution time.
 */
void ButtonBase::update(uint32_t now) {
    bool raw_state = pin_.get_state();
    bool is_pressed = false;
    
    if (_active_low) {
        is_pressed = !raw_state;
    } else {
        is_pressed = raw_state;
    }

    if (is_pressed && _active) {
        uint32_t activation_time = now - _press_time;
        if (activation_time % _button_event_period_ms < 10) {
            FIC_LOGI(TAG, "Button long press %d", activation_time);
        }
    } else if (is_pressed) {
        FIC_LOGI(TAG, "Button short press");
        _active = true;
        _press_time = now;
    } else {
        _active = false;
    }
}