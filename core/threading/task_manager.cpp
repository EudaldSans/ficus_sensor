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

        for (auto& task : self->_tasks) {
            if (now - task->last_run_time_ms >= task->get_run_period_ms()) {
                task->update();
                task->last_run_time_ms = now;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

void TaskManager::start() {
    if (_running) return;
    _running = true;

    xTaskCreatePinnedToCore(run, _name.c_str(), _stack_size, this, 10, &_handle, _core_id);
}

void TaskManager::add_task(std::shared_ptr<ITask> task) {
    _tasks.push_back(task);
}

void TaskManager::stop() { 
    _running = false; 
}