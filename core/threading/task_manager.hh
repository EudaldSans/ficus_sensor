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

class TaskManager {
    public: 
        TaskManager(const char* name, std::unique_ptr<ITaskRunner> task_runner);
        ~TaskManager();

        void start();
        void stop();

        void add_task(ITask* task);

    private: 
        static constexpr uint16_t _k_max_tasks = 32;
        std::array<ITask*, _k_max_tasks> _tasks;

        const char* _name;

        std::unique_ptr<ITaskRunner> _task_runner;
        static void run(void* instance);
        
        size_t _task_count = 0;

        std::atomic_bool _running = false;
        std::atomic_bool _active = false;

        constexpr static char const *TAG = "TaskManager";
};

#endif