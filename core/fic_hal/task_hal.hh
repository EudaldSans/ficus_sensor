#ifndef TASK_HAL_HH
#define TASK_HAL_HH

#include <inttypes.h>

class ITaskRunner {
    public:
        virtual ~ITaskRunner() = default;
        virtual void create_task(void (*run)(void*), void* params) = 0;

        virtual void delete_task() = 0;
        virtual void delay(uint32_t ms) = 0;
};

#endif 
