#ifndef ESP_TIME_HH
#define ESP_TIME_HH

#include "fic_time.hh"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class EspTimeSource : public ITimeSource {
public:
    static EspTimeSource& instance() {
        static EspTimeSource inst;  // Thread-safe in C++11+
        return inst;
    }

    uint64_t _get_time_ms() override {
        return pdTICKS_TO_MS(xTaskGetTickCount());
    }

    // Delete copy/move to enforce singleton
    EspTimeSource(const EspTimeSource&) = delete;
    EspTimeSource& operator=(const EspTimeSource&) = delete;

private:
    EspTimeSource() = default;
};

class EspTimeDelay : public ITimeDelay {
public:
    static EspTimeDelay& instance() {
        static EspTimeDelay inst;  // Thread-safe in C++11+
        return inst;
    }

    void _delay_ms(uint32_t ms) override {
        BaseType_t ticks_to_delay = pdMS_TO_TICKS(ms);

        if (ticks_to_delay == 0) {
            vTaskDelay(1); // Minimum delay of 1 tick to yield to other tasks
        } else {
            vTaskDelay(ticks_to_delay);
        }
    }

    // Delete copy/move to enforce singleton
    EspTimeDelay(const EspTimeDelay&) = delete;
    EspTimeDelay& operator=(const EspTimeDelay&) = delete;
    
private:
    EspTimeDelay() = default;
};

#endif