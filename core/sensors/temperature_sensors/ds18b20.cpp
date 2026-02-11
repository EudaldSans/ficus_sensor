#include "ds18b20.hh"
#include "esp_log.h"
#include "esp_check.h"

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

DS18B20::DS18B20(std::shared_ptr<IOnewireBus> bus, ds18b20_resolution_t resolution) : ISensor(), _bus(bus) {
    this->resolution = resolution;
}

DS18B20::~DS18B20() {}

in_error_t DS18B20::init() {
    const uint64_t ds18b20_address = 0x28;
    const uint64_t ds18b20_mask = 0xFF;
    const uint32_t max_rx_bytes = 10;

    IN_RETURN_ON_ERROR(_bus->init(max_rx_bytes), ESP_LOGE(TAG, "Failed to initialize bus"));
    IN_RETURN_ON_ERROR(_bus->find_device(ds18b20_address, ds18b20_mask), ESP_LOGE(TAG, "Failed to find device"));

    set_resolution(DS18B20::resolution_12B);

    return IN_OK;
}

in_error_t DS18B20::trigger_measurement(uint16_t &measurement_delay_ms) {
    constexpr uint32_t resolution_delays_ms[4] = {100, 200, 400, 800};
    std::vector<uint8_t> tx_buffer = {cmd_convert_temp};

    IN_RETURN_ON_ERROR(_bus->reset(), ESP_LOGE(TAG, "Reset failed to trigger measurement"));
    IN_RETURN_ON_ERROR(_bus->write_to_all(tx_buffer), ESP_LOGE(TAG, "Write data failed to trigger measurement"));

    measurement_delay_ms = resolution_delays_ms[resolution];

    return IN_OK;
}

in_error_t DS18B20::set_resolution(ds18b20_resolution_t resolution) {
    const uint8_t resolution_data[4] = {0x1F, 0x3F, 0x5F, 0x7F};
    std::vector<uint8_t> tx_buffer = {0, 0, resolution_data[resolution]};

    IN_RETURN_ON_ERROR(_bus->reset(), ESP_LOGE(TAG, "Reset failed to set resolution"));
    IN_RETURN_ON_ERROR(_bus->write_to_all(tx_buffer), ESP_LOGE(TAG, "Write data failed to set resolution"));

    return IN_OK;
}

float DS18B20::get_last_measurement() {
    ds18b20_scratchpad_t scratchpad = {};
    std::vector<uint8_t> tx_buffer = {cmd_read_scratchpad};
    std::vector<uint8_t> rx_buffer(sizeof(ds18b20_scratchpad_t));
    const uint8_t lsb_mask[4] = {0x07, 0x03, 0x01, 0x00}; 

    IN_RETURN_VALUE_ON_ERROR(_bus->reset(), 0, ESP_LOGE(TAG, "Reset failed to get measurement"));
    IN_RETURN_VALUE_ON_ERROR(_bus->write_to_all(tx_buffer), 0, ESP_LOGE(TAG, "Write data failed to get measurement"));
    IN_RETURN_VALUE_ON_ERROR(_bus->read_bytes(rx_buffer), 0, ESP_LOGE(TAG, "Read data failed to get measurement"));

    memcpy(&scratchpad, rx_buffer.data(), sizeof(ds18b20_scratchpad_t));

    uint8_t lsb_masked = scratchpad.temp_lsb & (~lsb_mask[scratchpad.configuration >> 5]);
    int16_t temperature_raw = (((int16_t)scratchpad.temp_msb << 8) | lsb_masked);
    return temperature_raw / 16.0f;
}

const char* DS18B20::get_name() {
    return "DS18B20";
}