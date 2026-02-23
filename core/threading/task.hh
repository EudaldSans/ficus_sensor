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
        virtual void update(uint64_t now) = 0;  

        virtual TaskType get_task_type() = 0;
};

class IContinuousTask : public ITask {
    public:
        virtual ~IContinuousTask() = default;
        TaskType get_task_type() { return TaskType::CONTINUOUS; }
};

class IOneShotTask : public ITask {
    public:
        virtual ~IOneShotTask() = default;
        virtual bool is_finished() = 0;
        virtual bool reset() = 0; // Resets the task to be run again

        TaskType get_task_type() { return TaskType::ONE_SHOT; }
};


class IIntervalTask : public ITask {
    public:
        virtual ~IIntervalTask() = default;

        virtual uint32_t get_run_period_ms() = 0; // Return interval in milliseconds
        TaskType get_task_type() { return TaskType::INTERVAL; }


        uint64_t last_run_time_ms = 0;
};

#endif