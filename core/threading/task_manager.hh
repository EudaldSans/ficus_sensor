#include <string>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "task.hh"


#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

class TaskManager {
    public: 
        TaskManager(std::string name, uint32_t stack_size, BaseType_t _core_id = tskNO_AFFINITY);
        ~TaskManager();

        void start();
        void stop();

        void add_task(std::shared_ptr<ITask> task);
        void add_interval_task(std::shared_ptr<IntervalTask> task);
        void add_one_shot_task(std::shared_ptr<IOneShotTask> task);

    private: 
        static void run(void* pvParameters);
        std::vector<std::shared_ptr<ITask>> _tasks;
        std::vector<std::shared_ptr<IntervalTask>> _interval_tasks;
        std::vector<std::shared_ptr<IOneShotTask>> _one_shot_tasks;

        std::atomic_bool _running = false;

        const std::string _name;
        const uint32_t _stack_size;
        const BaseType_t _core_id;

        TaskHandle_t _handle = nullptr;

        constexpr static char const *TAG = "TaskManager";
};

#endif