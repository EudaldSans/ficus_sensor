#include <memory.h>

#include "ds18b20.hh"
#include "fic_log.hh"

#include "fic_time.hh"

typedef struct {
    uint8_t temp_lsb;      /*!< lsb of temperature */
    uint8_t temp_msb;      /*!< msb of temperature */
    uint8_t th_user1;      /*!< th register or user byte 1 */
    uint8_t tl_user2;      /*!< tl register or user byte 2 */
    uint8_t configuration; /*!< resolution configuration register */
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t _reserved3;
    uint8_t crc_value;     /*!< crc value of scratchpad data */
} __attribute__((packed)) ds18b20_scratchpad_t;

DS18B20::DS18B20(IOnewireBus &bus, ds18b20_resolution_t resolution) : IAsyncSensor<float>(), _bus(bus) {
    _resolution = resolution;
}

DS18B20::~DS18B20() {}

/**
 * @brief Initializes the underlying onewire bus and configures the sensor resolution.
 * 
 * @return fic_error_t @c FIC_OK on success, error code otherwise
 */
fic_error_t DS18B20::init() {
    const uint64_t ds18b20_address = 0x28;
    const uint64_t ds18b20_mask = 0xFF;
    const uint32_t max_rx_bytes = 10;

    FIC_LOGI(TAG, "Initializing DS18B20");

    FIC_RETURN_ON_ERROR(_bus.init(max_rx_bytes), FIC_LOGE(TAG, "Failed to initialize bus"));

    FIC_RETURN_ON_ERROR(_bus.find_device(ds18b20_address, ds18b20_mask), FIC_LOGE(TAG, "Failed to find device"));

    set_resolution(DS18B20::resolution_12B);

    FIC_LOGI(TAG, "Initialized DS18B20");

    return FIC_OK;
}

/**
 * @brief Triggers a measurement
 * 
 * @warning This function relies on ITimeSource to set the measurement finish time, creating a hidden dependency. 
 * So far this has been considered an acceptable DIP violation, since it allows to keep the sensor interface simple.
 * 
 * @param measurement_delay_ms [out] the delay after which the measurement will be ready, in milliseconds
 * @return fic_error_t @c FIC_OK on success, error code otherwise
 */
fic_error_t DS18B20::trigger_measurement(uint16_t &measurement_delay_ms) {
    constexpr uint32_t resolution_delays_ms[4] = {100, 200, 400, 800};
    std::vector<uint8_t> tx_buffer = {cmd_convert_temp};

    FIC_RETURN_ON_ERROR(_bus.reset(), FIC_LOGE(TAG, "Reset failed to trigger measurement"));
    FIC_RETURN_ON_ERROR(_bus.write_to_all(tx_buffer), FIC_LOGE(TAG, "Write data failed to trigger measurement"));

    measurement_delay_ms = resolution_delays_ms[_resolution];

    _measure_finish_time_ms = ITimeSource::get_time_ms() + measurement_delay_ms;

    return FIC_OK;
}

/**
 * @brief Checks whether the last measurement is ready.
 * 
 * @warning This function relies on ITimeSource to check if the measurement is ready, creating a hidden dependency. 
 * So far this has been considered an acceptable DIP violation, since it allows to keep the sensor interface simple.
 * 
 * @return true if the measurement is ready, false otherwise
 */
bool DS18B20::is_ready() { 
    return ITimeSource::get_time_ms() >= _measure_finish_time_ms; 
}

fic_error_t DS18B20::set_resolution(ds18b20_resolution_t resolution) {
    const uint8_t resolution_data[4] = {0x1F, 0x3F, 0x5F, 0x7F};
    std::vector<uint8_t> tx_buffer = {0, 0, resolution_data[resolution]};

    FIC_LOGI(TAG, "Setting resolution to %d", resolution);

    FIC_RETURN_ON_ERROR(_bus.reset(), FIC_LOGE(TAG, "Reset failed to set resolution"));
    FIC_RETURN_ON_ERROR(_bus.write_to_all(tx_buffer), FIC_LOGE(TAG, "Write data failed to set resolution"));

    _resolution = resolution;

    return FIC_OK;
}

/**
 * @brief Obtains a measurement if it is ready
 * 
 * @param value [out] the measured temperature in Celsius
 * @return fic_error_t @c FIC_OK on success, error code otherwise
 */
fic_error_t DS18B20::get_measurement(float &value) {
    ds18b20_scratchpad_t scratchpad = {};
    std::vector<uint8_t> tx_buffer = {cmd_read_scratchpad};
    std::vector<uint8_t> rx_buffer(sizeof(ds18b20_scratchpad_t));
    const uint8_t lsb_mask[4] = {0x07, 0x03, 0x01, 0x00}; 

    value = 0;

    FIC_ERR_RETURN_ON_FALSE(is_ready(), FIC_ERR_NOT_FINISHED, FIC_LOGE(TAG, "Measurement is not yet ready"));

    FIC_RETURN_ON_ERROR(_bus.reset(), FIC_LOGE(TAG, "Reset failed to get measurement"));
    FIC_RETURN_ON_ERROR(_bus.write_to_all(tx_buffer), FIC_LOGE(TAG, "Write data failed to get measurement"));
    FIC_RETURN_ON_ERROR(_bus.read_bytes(rx_buffer), FIC_LOGE(TAG, "Read data failed to get measurement"));

    memcpy(&scratchpad, rx_buffer.data(), sizeof(ds18b20_scratchpad_t));

    uint8_t lsb_masked = scratchpad.temp_lsb & (~lsb_mask[scratchpad.configuration >> 5]);
    int16_t temperature_raw = (((int16_t)scratchpad.temp_msb << 8) | lsb_masked);
    value = temperature_raw / 16.0f; 
    return FIC_OK;
}