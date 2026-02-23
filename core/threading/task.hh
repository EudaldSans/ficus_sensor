#include <inttypes.h>

#include "endpoint.hh"

#ifndef TASK_H
#define TASK_H

class ITask {
    public: 
        virtual ~ITask() = default;
        virtual void setup() = 0;
        virtual void update(uint64_t now) = 0;  
};

class IOneShotTask : public ITask {
    public:
        virtual ~IOneShotTask() = default;
        virtual bool is_finished() = 0;
        virtual bool reset() = 0; // Resets the task to be run again
};


class IntervalTask : public ITask {
    public:
        virtual ~IntervalTask() = default;

        virtual uint32_t get_run_period_ms() = 0; // Return interval in milliseconds

        uint64_t last_run_time_ms = 0;
};

#endif