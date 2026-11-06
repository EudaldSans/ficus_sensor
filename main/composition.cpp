#include "composition.hh"

#include <cstdio>

// ── Hardware includes ──
#include "onewire_bus.hh"
#include "adc.hh"
#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"
#include "led_strip_single.hh"
#include "esp_gpio.hh"
#include "button.hh"

// ── WiFi includes ──
#include "wifi_context.hh"
#include "wifi_controller.hh"
#include "wifi_station.hh"

// ── Routing includes ──
#include "router.hh"
#include "conversions.hh"

// ── Sensor includes ──
#include "sensor_endpoints.hh"

// ── Firebase includes ──
#include "firebase_encoder.hh"
#include "firebase_endpoint.hh"
#include "https_client.hh"
#include "credentials_provider.hh"

// ── Platform includes ──
#include "esp_time.hh"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "task_manager.hh"

#include "fic_log.hh"

static std::string get_device_id();

// ── Constants ──
static constexpr uint8_t  ONEWIRE_BUS_GPIO     = 18;
static constexpr uint8_t  LED_GPIO              = 8;
static constexpr uint32_t LED_STRIP_RMT_RES_HZ  = 10 * 1000 * 1000;
static constexpr uint16_t SENSOR_MEAS_PERIOD_MS = 30000;

static const char firebase_url[] = "https://ficus-base-default-rtdb.europe-west1.firebasedatabase.app";
static const char firebase_root[] = "plants";

static const char temperature_key[] = "t";
static const char soil_moisture_key[] = "sm";

// ── Embedded certificate ──
extern const char root_cert_pem_start[] asm("_binary_root_cert_pem_start");
extern const char root_cert_pem_end[]   asm("_binary_root_cert_pem_end");

// ── TLS Provider ──
class FakeTLSProvider : public ICredentialsProvider {
public:
    std::string_view get_client_cert() const override { return ""; }
    std::string_view get_client_key() const override { return ""; }
    std::string_view get_ca_cert() const override {
        size_t size_with_null = (root_cert_pem_end - root_cert_pem_start);
        return std::string_view(root_cert_pem_start, size_with_null);
    }
};

// ── Hardware ──
static LEDStripSingle         led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);
static RGBSignaler            rgb_signaler_impl(led_strip);

static OnewireBus             onewire(ONEWIRE_BUS_GPIO);
static DS18B20                temperature_sensor(onewire, DS18B20::resolution_12B);

static ADC                    adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
static AnalogHumiditySensor   soil_moisture_sensor(adc, 3300);

constexpr uint32_t button_poll_period_ms = 100;
static EspGPIO<GPIO_NUM_9, GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE>              button_gpio{};
static Button<button_poll_period_ms>               boot_button_impl{button_gpio};

// ── WiFi ──
static WiFiContext            wifi_context;
static WiFiController         wifi_controller(wifi_context);
static WiFiStation            wifi_station(wifi_context);

EspSntpClient sntp_client_impl = EspSntpClient(wifi_controller);

// ── Channels ──
static value_t<float> temperature_sensor_output;
static value_t<float> soil_moisture_sensor_output;
static firebase_channel<float>        firebase_tempertaure(temperature_key);
static firebase_channel<float>        firebase_soil_moisture(soil_moisture_key);

// ── Firebase channels ──
static FirebaseChannelPtr firebase_channel_list[] = {
    &firebase_tempertaure, 
    &firebase_soil_moisture,
};

// ── Routing ──
static Router router{
    ChannelLink<float, float>     {temperature_sensor_output, firebase_tempertaure.value},
    ChannelLink<float, float>     {soil_moisture_sensor_output, firebase_soil_moisture.value},
};

// -- Identity --
static const std::string device_id = get_device_id();

// ── Endpoints ──
static AsyncSensorEndpoint<float> temperature_endpoint(temperature_sensor_output, temperature_sensor, SENSOR_MEAS_PERIOD_MS);
static SensorEndpoint<float>      soil_moisture_endpoint(soil_moisture_sensor_output, soil_moisture_sensor, SENSOR_MEAS_PERIOD_MS);

static FakeTLSProvider    tls_provider;
static HttpsClient        http_client(tls_provider);
static FirebaseEncoder    encoder(firebase_url, device_id.c_str(), firebase_root, http_client);

static uint32_t firebase_emit_period_ms = 3600 * 1000;
static FirebaseEndpoint firebase_endpoint(firebase_channel_list, wifi_controller, sntp_client, encoder, firebase_emit_period_ms);

// ── Extern references for main ──
RGBSignaler& rgb_signaler       = rgb_signaler_impl;
EspSntpClient& sntp_client      = sntp_client_impl;

ButtonBase& boot_button         = boot_button_impl;

WiFiState composition_get_wifi_state() {
    return wifi_controller.get_state();
}

void composition_init_hardware() {
    TimeSource::set_instance(&EspTimeSource::instance());
    TimeDelay::set_instance(&EspTimeDelay::instance());

    led_strip.init();
    nvs_flash_init();
}

void composition_add_tasks(TaskManager& tm) {
    tm.add_task(&temperature_endpoint);
    tm.add_task(&soil_moisture_endpoint);
    tm.add_task(&router);
    tm.add_task(&firebase_endpoint);
}

void composition_add_hw_tasks(TaskManager&tm) {
    tm.add_task(&boot_button);
    tm.add_task(&rgb_signaler_impl);
}

void composition_start_comms() {
    wifi_controller.init();
    wifi_station.sta_connect("XTA_47592", "Mh9gcxu5", 10);
    
    http_client.start();

    sntp_client_impl.add_server("pool.ntp.org");
}

static std::string get_device_id() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    char mac_cstr[13];
    snprintf(mac_cstr, sizeof(mac_cstr), "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return std::string(mac_cstr);
}
