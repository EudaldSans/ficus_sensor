#include <inttypes.h>

#ifndef TASK_H
#define TASK_H

class ITask {
    public: 
        virtual ~ITask() = default;
        virtual void setup() = 0;
        virtual void update() = 0;
        virtual uint32_t get_run_period_ms() = 0; // Return interval in milliseconds
        
        uint64_t last_run_time_ms = 0;
};

#endif