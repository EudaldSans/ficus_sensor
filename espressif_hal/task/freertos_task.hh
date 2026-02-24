#ifndef TASK_HH
#define TASK_HH

#include "task_hal.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class FreeRTOS_TaskRunner : public ITaskRunner {
    public:
        FreeRTOS_TaskRunner(const char* name, uint32_t stack_size, BaseType_t core_id = tskNO_AFFINITY);

        ~FreeRTOS_TaskRunner() = default;
        void create_task(void (*run)(void*), void* params) override;

        void delete_task() override;
        void delay(uint32_t ms) override;

    private: 
        const char* _name;
        const uint32_t _stack_size;
        const BaseType_t _core_id;

        TaskHandle_t _handle = nullptr;
};



#endif