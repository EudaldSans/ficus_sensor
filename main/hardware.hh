#ifndef HARDWARE_H
#define HARDWARE_H

#include "onewire_bus.hh"
#include "adc.hh"

#include "ds18b20.hh"
#include "analog_humidity_sensor.hh"

#include "led_strip_single.hh"
#include "rgb_signalling.hh"

#include "wifi_context.hh"
#include "wifi_controller.hh"
#include "wifi_station.hh"
#include "wifi_access_point.hh"
#include "wifi_scanner.hh"

#define ONEWIRE_BUS_GPIO        18
#define LED_GPIO                8

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

LEDStripSingle led_strip(LED_GPIO, LED_MODEL_WS2812, LED_STRIP_RMT_RES_HZ);
RGBSignaler rgb_signaler(led_strip);

OnewireBus onewire(ONEWIRE_BUS_GPIO);
DS18B20 t_sensor = DS18B20(onewire, DS18B20::resolution_12B);

const uint32_t h_sensor_max_mv = 3300;
ADC adc(ADC_CHANNEL_2, ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_DEFAULT);
AnalogHumiditySensor h_sensor = AnalogHumiditySensor(adc, h_sensor_max_mv);

WiFiContext wifi_context;
WiFiController wifi(wifi_context);
WiFiStation wifi_station(wifi_context);

#endif