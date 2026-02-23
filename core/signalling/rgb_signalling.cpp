#include "rgb_signalling.hh"

#include <algorithm>

/**
 * @brief Sets the LED to a Solid state
 * 
 * @param color The color to be set. For OFF use black (0, 0, 0)
 * @return @c FIC_OK
 */
fic_error_t RGBSignaler::set_solid(Color color) {
	std::lock_guard<std::mutex> lock(_mutex);
    bool led_on = true;

    if (color.red == 0 && color.green == 0 && color.blue == 0) {
        led_on = false;
    }

	_new_request = true;
	_new_signal = {
		.steps = 1,
		.cycles = INFINITE_CYCLES,
		.pattern = {0}
	};

	_new_signal.pattern[0] = {led_on, color, 0xFFFFFFFF};

	return FIC_OK;
}

/**
 * @brief Makes the LED blink between two colors
 * 
 * @param color_1 @c Color Time where the first color is active, in milliseconds
 * @param color_1_time_ms Time in ms where the first color is active
 * @param color_2 @c Color with the second color
 * @param off_time_ms Time where the second color is active, in milliseconds
 * @param cycles How many cycles to run signal for
 * @return @c FIC_OK
 */
fic_error_t RGBSignaler::set_blink(Color color_1, uint32_t color_1_time_ms, Color color_2, uint32_t color_2_time_ms, int32_t cycles) {
	std::lock_guard<std::mutex> lock(_mutex);

    bool color_1_on = true;
    bool color_2_on = true;

    if (color_1.red == 0 && color_1.green == 0 && color_1.blue == 0) {
        color_1_on = false;
    }

    if (color_2.red == 0 && color_2.green == 0 && color_2.blue == 0) {
        color_2_on = false;
    }

	_new_request = true;
	_new_signal = {
		.steps = 2,
		.cycles = cycles,
		.pattern = {0}
	};

	_new_signal.pattern[0] = {color_1_on, color_1, color_1_time_ms};
	_new_signal.pattern[1] = {color_2_on, color_2, color_2_time_ms};

	return FIC_OK;
}

fic_error_t RGBSignaler::set_custom_signal(const std::vector<RGB_action_t> pattern_composition, int32_t cycles) {
	std::lock_guard<std::mutex> lock(_mutex);

	if (pattern_composition.size() > MAX_STEPS_IN_RGB_SIGNAL) return FIC_ERR_INVALID_ARG;
	if (pattern_composition.size() == 0) return FIC_ERR_INVALID_ARG;

	_new_request = true;
	_new_signal = {
		.steps = static_cast<uint16_t>(pattern_composition.size()),
		.cycles = cycles,
		.pattern = {0}
	};

	std::copy(pattern_composition.begin(), pattern_composition.end(), _new_signal.pattern.begin());

	return FIC_OK;
}

/**
 * @brief Sets up the signaller.
 */
void RGBSignaler::setup() {
	_rgb_led.off();
}

/**
 * @brief Runs a tic of the signaller 
 */
void RGBSignaler::update(uint64_t now) {
	if (_new_request) {
		std::lock_guard<std::mutex> lock(_mutex);

		_current_step = 0;
		_remaining_cycles = _new_signal.cycles;
		_active_steps = _new_signal.steps;
		std::copy(_new_signal.pattern.begin(), _new_signal.pattern.end(), _pattern.begin());

		perform_action(_pattern[0], now);
		_new_request = false;
	}

	if (_remaining_cycles == 0 || _active_steps == 0) return;
	if (_step_timer.has_expired(now) == false) return;

	_current_step++;

	if (_current_step >= _active_steps) {
		_current_step = 0;
		if (_remaining_cycles > 0) {
			_remaining_cycles--;
		}

		if (_remaining_cycles == 0) {
			_rgb_led.off();
			return;
		}
	}

	perform_action(_pattern[_current_step], now);
}

void RGBSignaler::perform_action(RGB_action_t action, uint64_t now) {
	_step_timer.update_duration(action.duration_ms);
	_step_timer.reset(now);

	if (action.on) {
		_rgb_led.set_color(action.color);
	} else {
		_rgb_led.off();
	}
}
