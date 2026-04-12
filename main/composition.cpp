#include "composition.hh"

// ── Hardware includes ──
#include "onewire_bus.hh"
#include "adc.hh"
#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"
#include "led_strip_single.hh"

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
#include "nvs_flash.h"

#include "task_manager.hh"

#include "fic_log.hh"

// ── Constants ──
static constexpr uint8_t  ONEWIRE_BUS_GPIO     = 18;
static constexpr uint8_t  LED_GPIO              = 8;
static constexpr uint32_t LED_STRIP_RMT_RES_HZ  = 10 * 1000 * 1000;
static constexpr uint16_t SENSOR_MEAS_PERIOD_MS = 30000;

static const char firebase_url[] = "https://ficus-base-default-rtdb.europe-west1.firebasedatabase.app/test_node.json";

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
static DS18B20                t_sensor(onewire, DS18B20::resolution_12B);

static ADC                    adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
static AnalogHumiditySensor   h_sensor(adc, 3300);

// ── WiFi ──
static WiFiContext            wifi_context;
static WiFiController         wifi_controller(wifi_context);
static WiFiStation            wifi_station(wifi_context);

EspSntpClient sntp_client_impl = EspSntpClient(wifi_controller);

// ── Channels ──
static value_t<float> t_sensor_output;
static value_t<float> h_sensor_output;
static firebase_channel<int>        firebase_tempertaure("temperature");
static firebase_channel<float>      firebase_tempertaure_2("temperature_2");
static firebase_channel<int>        firebase_humidity("humidity");
static firebase_channel<float>      firebase_humidity_2("humidity_2");

// ── Firebase channels ──
static FirebaseChannelPtr firebase_channel_list[] = {
    &firebase_tempertaure, 
    &firebase_humidity,
    &firebase_tempertaure_2,
    &firebase_humidity_2
};

// ── Routing ──
static Router router{
    ChannelLink<float, int>     {t_sensor_output, firebase_tempertaure.value},
    ChannelLink<float, int>     {h_sensor_output, firebase_humidity.value},
    ChannelLink<float, float>   {t_sensor_output, firebase_tempertaure_2.value},
    ChannelLink<float, float>   {h_sensor_output, firebase_humidity_2.value}
};

// ── Endpoints ──
static AsyncSensorEndpoint<float> t_endpoint(t_sensor_output, t_sensor, SENSOR_MEAS_PERIOD_MS);
static SensorEndpoint<float>      h_endpoint(h_sensor_output, h_sensor, SENSOR_MEAS_PERIOD_MS);

static FakeTLSProvider    tls_provider;
static HttpsClient        http_client(tls_provider);
static FirebaseEncoder    encoder(firebase_url, "id_test", http_client);

static FirebaseEndpoint firebase_endpoint(firebase_channel_list, wifi_controller, sntp_client);

// ── Extern references for main ──
RGBSignaler& rgb_signaler       = rgb_signaler_impl;
EspSntpClient& sntp_client      = sntp_client_impl;

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
    tm.add_task(&t_endpoint);
    tm.add_task(&h_endpoint);
    tm.add_task(&rgb_signaler_impl);
    tm.add_task(&router);
    tm.add_task(&firebase_endpoint);
}

void composition_start_comms() {
    wifi_controller.init();
    wifi_station.sta_connect("XTA_47592", "Mh9gcxu5", 10);
    
    http_client.start();

    sntp_client_impl.add_server("pool.ntp.org");
}
