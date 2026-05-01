#include <inttypes.h>

#ifndef TASK_H
#define TASK_H

enum class TaskType {
    ONE_SHOT,
    INTERVAL,
    CONTINUOUS
};

class ITask {
    public: 
        virtual ~ITask() = default;
        virtual void setup() = 0;
        virtual void update(uint32_t now) = 0;  

        virtual bool should_run(uint32_t now) = 0;
};

class ContinuousTask : public ITask {
    public:
        virtual ~ContinuousTask() = default;
        bool should_run(uint32_t now) override { return true; }
};

class OneShotTask : public ITask {
    public:
        virtual ~OneShotTask() = default;
        virtual bool is_finished() = 0;
        virtual bool reset() = 0; // Resets the task to be run again

        bool should_run(uint32_t now) override { return !is_finished(); }
};


class IntervalTask : public ITask {
    public:
        virtual ~IntervalTask() = default;

        virtual uint32_t get_run_period_ms() const = 0; // Return interval in milliseconds
        bool should_run(uint32_t now) override { return (now - last_run_time_ms) >= get_run_period_ms(); }
    

        uint32_t last_run_time_ms = 0;
};

#endif