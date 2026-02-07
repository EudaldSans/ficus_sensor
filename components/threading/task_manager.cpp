#include "task_manager.hh"

TaskManager::TaskManager(std::string name, uint32_t stack_size): _name(name), _stack_size(stack_size) {

}

void TaskManager::run(void* pvParameters) {
    auto* self = static_cast<TaskManager*>(pvParameters);
        
    for (auto& task : self->_tasks) task->setup();

    while (self->_running) {
        uint32_t now = pdTICKS_TO_MS(xTaskGetTickCount());

        for (auto& task : self->_tasks) {
            if (now - task->last_run_time_ms >= task->get_run_period_ms()) {
                task->update();
                task->last_run_time_ms = now;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    vTaskDelete(NULL);
}

void TaskManager::start() {
    if (_running) return;
    _running = true;

    xTaskCreate(run, _name.c_str(), _stack_size, this, 10, &_handle);
}

void TaskManager::add_task(std::shared_ptr<ITask> task) {
    _tasks.push_back(task);
}

void TaskManager::stop() { 
    _running = false; 
}