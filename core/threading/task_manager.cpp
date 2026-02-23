#include "task_manager.hh"

#include "esp_log.h"

TaskManager::TaskManager(std::string name, uint32_t stack_size, BaseType_t core_id): _name(name), _stack_size(stack_size), _core_id(core_id) {

}

void TaskManager::run(void* pvParameters) {
    auto* self = static_cast<TaskManager*>(pvParameters);
    
    ESP_LOGI(TAG, "Setting up tasks for manager %s", self->_name.c_str());
    for (auto& task : self->_tasks) task->setup();

    ESP_LOGI(TAG, "Manager %s tasks start", self->_name.c_str());
    while (self->_running) {
        uint64_t now = pdTICKS_TO_MS(xTaskGetTickCount());
        uint32_t next_run_in_ms = 100; // Default max sleep time

        for (auto& task : self->_one_shot_tasks) {
            if (task->is_finished()) continue;
            task->update(now);
        }

        for (size_t i = 0; i < self->_task_count; i++) {
            auto& task = self->_tasks[i];

            if (task->get_task_type() == TaskType::ONE_SHOT) {
                if (task->is_finished()) continue;

                task->update(now);

            } else if (task->get_task_type() == TaskType::INTERVAL) {
                task->update(now);
                task->last_run_time_ms = now;

                uint32_t time_to_next = task->get_run_period_ms() - (now - task->last_run_time_ms);
                if (time_to_next < next_run_in_ms) next_run_in_ms = time_to_next;

            } else if (task->get_task_type() == TaskType::CONTINUOUS) {
                task->update(now);
                next_run_in_ms = 0; // Run task continuously

            } else {
                ESP_LOGW(TAG, "Unknown task type for task in manager %s", self->_name.c_str());
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(next_run_in_ms));
    }
    vTaskDelete(NULL);
}

void TaskManager::start() {
    if (_running) return;
    _running = true;

    xTaskCreatePinnedToCore(run, _name.c_str(), _stack_size, this, 10, &_handle, _core_id);
}

void TaskManager::add_task(ITask* task) {
    if (_task_count < _tasks.size()) {
        _tasks[_task_count++] = task;
    } else {
        ESP_LOGE(TAG, "Task limit reached!");
    }
}

void TaskManager::stop() { 
    _running = false; 
}