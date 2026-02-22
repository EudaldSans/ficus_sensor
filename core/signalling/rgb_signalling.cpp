

/**
 * @brief Sets the LED to a Solid state
 * 
 * @param color The color to be set. For OFF use black (0, 0, 0)
 * @return ??
 */
fic_error_t RGBSignaler::set_solid(Color color) {

}

/**
 * @brief Makes the LED blink between two colors
 * 
 * @param color_1 @c Color Time where the first color is active, in milliseconds
 * @param color_1_time_ms Time in ms where the first color is active
 * @param color_2 @c Color with the second color
 * @param off_time_ms Time where the second color is active, in milliseconds
 * @param cycles How many cycles to run signal for (0 is infinite)
 * @return fic_error_t ??
 */
fic_error_t RGBSignaler::set_blink(Color color_1, uint32_t color_1_time_ms, Color color_2, uint32_t off_time_ms, int32_t cycles) {

}


fic_error_t RGBSignaler::set_custom_signal(const std::vector<RGB_action_t> pattern_composition, int32_t cycles) {

}

/**
 * @brief Sets up the signaller.
 */
void RGBSignaler::setup() {

}

/**
 * @brief Runs a tic of the signaller 
 */
void RGBSignaler::update() {

}
