/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "fic_log.hh"

#include "nvs_flash.h"

#include "sdkconfig.h"
#include "fic_log.hh"
#include "esp_fic_log.hh"

#include "task_manager.hh"

#include "router.hh"
#include "endpoint.hh"
#include "conversions.hh"

#include "sensor_endpoints.hh"
#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"

#include "esp_time.hh"
#include "onewire_bus.hh"
#include "adc.hh"

#include "led_strip_single.hh"
#include "rgb_signalling.hh"

#include "wifi_context.hh"
#include "wifi_controller.hh"
#include "wifi_station.hh"
#include "wifi_access_point.hh"
#include "wifi_scanner.hh"

#include "https_client.hh"
#include "credentials_provider.hh"

#include "freertos_task.hh"
#include "freertos_queue.hh"

#include "firebase_encoder.hh"

#include "routing.hh"

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

extern const char root_cert_pem_start[] asm("_binary_root_cert_pem_start");
extern const char root_cert_pem_end[]   asm("_binary_root_cert_pem_end");

const char firebase_url[] = "https://ficus-base-default-rtdb.europe-west1.firebasedatabase.app/test_node.json";

LEDStripSingle led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);
RGBSignaler rgb_signaler(led_strip);

const uint16_t sensor_meas_period_ms = 30000;

OnewireBus onewire(ONEWIRE_BUS_GPIO);
DS18B20 t_sensor = DS18B20(onewire, DS18B20::resolution_12B);
AsyncSensorEndpoint<float> t_endpoint(
    t_sensor_output,
    t_sensor,
    sensor_meas_period_ms
);

const uint32_t h_sensor_max_mv = 3300;
ADC adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
AnalogHumiditySensor h_sensor = AnalogHumiditySensor(adc, h_sensor_max_mv);
SensorEndpoint<float> h_endpoint(
    h_sensor_output,
    h_sensor,
    sensor_meas_period_ms
);

WiFiContext wifi_context;
WiFiController wifi(wifi_context);
WiFiStation wifi_station(wifi_context);

static const char *TAG = "main";

class FakeListener : public IHTTPListener {
public:
    FakeListener(RGBSignaler& signaler) : _signaler(signaler) {}
    ~FakeListener() = default;

    virtual void on_success(const Response& resp) override {
        if (resp.status_code < 300) _signaler.set_solid(LED_GREEN);
        else _signaler.set_solid(LED_YELLOW);

        if (resp.payload && resp.payload_size > 0) {
            text.assign(resp.payload, resp.payload_size);            
        } else {
            text = "null";
        }

        FIC_LOGI("FakeListener", "%s", text.c_str());
    }
    virtual void on_failure(fic_error_t error) override {
        _signaler.set_solid(LED_RED);
    }

private:
    RGBSignaler& _signaler;

    std::string text;
};

class FakeTLSProvider : public ICredentialsProvider {
public: 
    std::string_view get_client_cert() const override {return "";}
    std::string_view get_client_key() const override {return "";}
    std::string_view get_ca_cert() const override {
        size_t size_with_null = (root_cert_pem_end - root_cert_pem_start); 
        return std::string_view(root_cert_pem_start, size_with_null);
    }
};

extern "C" void app_main(void) {  
    fic_log_set_backend(esp32_backend);

    ITimeSource::set_instance(&EspTimeSource::instance());
    ITimeDelay::set_instance(&EspTimeDelay::instance());

    FIC_LOGI(TAG, "Initializing hardware");
    led_strip.init();

    nvs_flash_init();

    FIC_LOGI(TAG, "Setting up task manager");
    TaskManager task_manager = TaskManager(
        "main_task_manager", 
        std::make_unique<FreeRTOS_TaskRunner>("main_task_manager", 4096, tskNO_AFFINITY)
    );

    task_manager.add_task(&t_endpoint);
    task_manager.add_task(&h_endpoint);
    task_manager.add_task(&rgb_signaler);
    task_manager.add_task(&router);

    FakeTLSProvider tls_provider = FakeTLSProvider();
    HttpsClient http_client = HttpsClient(tls_provider);

    auto encoder = std::make_unique<FirebaseEncoder>(firebase_url, "id_test", http_client);
    
    std::vector<std::shared_ptr<IConversion<float>>> conversions;
    // conversions.push_back(std::make_shared<ToFahrenheitConversion<float>>());

    FIC_LOGI(TAG, "Size of conversions: %d", conversions.size());

    http_client.start();
    task_manager.start();

    wifi.init();
    wifi_station.sta_connect("XTA_47592", "Mh9gcxu5", 10);

    while (1) {
        uint32_t blink_time = 500;
        uint16_t cycles = 2;
        size_t free_mem = esp_get_free_heap_size();

        FIC_LOGI(TAG, "free heap size %d", free_mem);

        if (wifi.get_state() != WiFiState::STA_CONNECTED) {
            rgb_signaler.set_blink(LED_BLUE, blink_time, LED_OFF, blink_time, cycles);
        } else {
            rgb_signaler.set_blink(LED_BLUE, blink_time / 5, LED_OFF, blink_time / 5, cycles * 5);
        }

        if (firebase_h_input.is_new()) {
            int humidity = firebase_h_input.consume();
            FIC_LOGI(TAG, "h_consumer got %d", humidity);
        }

        if (firebase_t_input.is_new()) {
            int temperature = firebase_t_input.consume();
            FIC_LOGI(TAG, "t_consumer got %d", temperature);
        }

        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}