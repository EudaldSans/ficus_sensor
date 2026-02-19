
#include "led_strip_single.hh"

/**
 * @brief Construct a new LEDStripSingle::LEDStripSingle object
 * 
 * @param pin GPIO pin number to which the LED strip is connected
 * @param model @c led_model_t model of the LED strip (e.g., WS2812, SK6812, etc.)
 */
LEDStripSingle::LEDStripSingle(uint8_t pin, led_model_t model) : _pin(pin), _model(model) {}

/**
 * @brief Deinit and destroy the LEDStripSingle::LEDStripSingle object
 */
LEDStripSingle::~LEDStripSingle() {
    deinit();
}

/**
 * @brief Initialize the underlying led strip handle
 * 
 * @return @c fic_error_t with the result of the operation.
 */
fic_error_t LEDStripSingle::init() {
    if (_initialized) {
        ESP_LOGW(TAG, "LED strip already initialized");
        return FIC_OK;
    }
    
    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = _pin;
    strip_config.max_leds = 1;
    strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB;
    strip_config.led_model = _model;
    strip_config.flags.invert_out = false;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    rmt_config.resolution_hz = LED_STRIP_RMT_RES_HZ;
    rmt_config.flags.with_dma = false; // DMA is not supported on ESP32-C6

    if (led_strip_new_rmt_device(&strip_config, &rmt_config, &_led_strip) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to create new led strip device"); 
        return FIC_ERR_SDK_FAIL;
    }

    _initialized = true;

    return FIC_OK;
}

/**
 * @brief Deinitialize the underlying led strip handle
 * 
 * @return @c ºfic_error_t with the result of the operation.
 */
fic_error_t LEDStripSingle::deinit() {
    if (!_initialized) {
        ESP_LOGW(TAG, "LED strip not initialized");
        return FIC_OK;
    }

    if (led_strip_rmt_del(_led_strip) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to delete led strip device");
        return FIC_ERR_SDK_FAIL;
    }

    _initialized = false;
    return FIC_OK;
}

/**
 * @brief Turns the LED OFF
 * 
 * @return @c FIC_OK
 */
fic_error_t LEDStripSingle::off() {
    led_strip_set_pixel(_led_strip, 0, 0, 0, 0);
    led_strip_refresh(_led_strip);
    return FIC_OK;
}
    
/**
 * @brief Turns the LED ON with whichever color was previously set
 * 
 * @return @c FIC_OK
 */
fic_error_t LEDStripSingle::on() {
    led_strip_set_pixel(_led_strip, 0, _color.red, _color.green, _color.blue);
    led_strip_refresh(_led_strip);
    return FIC_OK;
}

/**
 * @brief Sets the color of the LED
 * 
 * @param color The new color to set the LED to
 * @return @c FIC_OK
 */
fic_error_t LEDStripSingle::set_color(Color color) {
    _color = color;
    
    led_strip_set_pixel(_led_strip, 0, 0.05 * _color.red, 0.05 * _color.green, 0.05 * _color.blue);
    led_strip_refresh(_led_strip);
    return FIC_OK;
}