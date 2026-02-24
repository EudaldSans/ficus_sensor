
# Ficus Sensor

Personal project measuring my beloved ficus' health parameters.

Built on **ESP-IDF** for the **ESP32** (Xtensa), using C++17.

## Raison d'être

This is the project I work on when either:
- I want to learn new concepts
- I am fed up with the issues on my job's codebase and decide to try doing it my way

## Project Structure

The project follows a **Ports & Adapters** (hexagonal) architecture. Platform-independent business logic lives in `core/`, while ESP-IDF-specific implementations live in `espressif_hal/`. `main/` acts as the composition root that wires everything together.

### Data Flow

```
Sensor ──► SensorEndpoint ──► OutputChannel ──► Router ──► InputChannel ──► Consumer callback
              (ITask)            (emit)         (link)       (receive)
```

Sensors are wrapped in `SensorEndpoint` (sync) or `AsyncSensorEndpoint` (async), which produce values on output channels. The `Router` links output channels to input channels, optionally applying a pipeline of `IConversion<T>` stages (e.g. unit conversion).

## Hardware

| Component | Role | Interface |
|-----------|------|-----------|
| ESP32 DevKit | MCU | — |
| DS18B20 | Temperature sensor | 1-Wire (GPIO 18) |
| Analog soil moisture probe | Humidity sensor | ADC1 Channel 2 |
| WS2812 addressable LED | Status signalling | RMT (GPIO 8) |

## Configuration

Hardware pin assignments are defined in [main/ficus_sensor_main.cpp](main/ficus_sensor_main.cpp):

| Define | Default | Description |
|--------|---------|-------------|
| `ONEWIRE_BUS_GPIO` | 18 | GPIO for the 1-Wire bus (DS18B20) |
| `LED_GPIO` | 8 | GPIO for the WS2812 LED |
| `LED_STRIP_RMT_RES_HZ` | 10 MHz | RMT peripheral resolution |

Sensor measurement period and ADC channel/attenuation are configured in the same file as runtime constants.

