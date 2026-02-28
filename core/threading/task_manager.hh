#include <string>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

#include <inttypes.h>

#include "task_hal.hh"

#include "task.hh"


#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define MAX_TASKS 32

class TaskManager {
    public: 
        TaskManager(const char* name, std::unique_ptr<ITaskRunner> task_runner);
        ~TaskManager();

        void start();
        void stop();

        void add_task(ITask* task);

    private: 
        static void run(void* instance);
        std::array<ITask*, MAX_TASKS> _tasks;
        size_t _task_count = 0;

        std::atomic_bool _running = false;
        std::atomic_bool _active = false;

        const char* _name;
        std::unique_ptr<ITaskRunner> _task_runner;

        constexpr static char const *TAG = "TaskManager";
};

#endif