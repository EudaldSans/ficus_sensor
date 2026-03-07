#ifndef FREERTOS_QUEUE_HH
#define FREERTOS_QUEUE_HH

#include "queue_hal.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

template <typename T, size_t N>
class FreeRTOSQueue : public IQueue<T> {
public:
    FreeRTOSQueue() {
        _queue = xQueueCreateStatic(N, sizeof(T), _buffer, &_static_queue);
    }

    ~FreeRTOSQueue() override = default;

    bool push(const T& item) override { 
        if (xPortInIsrContext()) {
            BaseType_t high_priority_task_woken = pdFALSE;
            bool result = xQueueSendFromISR(_queue, &item, &high_priority_task_woken);
            portYIELD_FROM_ISR(high_priority_task_woken);

            return result;
        }

        return xQueueSend(_queue, &item, 0) == pdPASS; 
    }

    bool pop(T& item, bool block, uint16_t timeout_ms) override { 
        if (xPortInIsrContext()) {
            BaseType_t high_priority_task_woken = pdFALSE;
            return xQueueReceiveFromISR(_queue, &item, &high_priority_task_woken) == pdPASS;
        }

        TickType_t ticks = block ? pdMS_TO_TICKS(timeout_ms) : 0;

        return xQueueReceive(_queue, &item, ticks) == pdPASS; 
    }

    size_t size() const override { 
        if (xPortInIsrContext()) {
            return uxQueueMessagesWaitingFromISR(_queue);
        }
        
        return uxQueueMessagesWaiting( _queue ); 
    }

    bool empty() const override { 
        if (xPortInIsrContext()) {
            return uxQueueMessagesWaitingFromISR(_queue) == 0;
        }

        return uxQueueMessagesWaiting( _queue ) == 0; 
    }

private:
    QueueHandle_t _queue = nullptr;
    StaticQueue_t _static_queue;
    uint8_t _buffer[N * sizeof(T)];
};


#endif