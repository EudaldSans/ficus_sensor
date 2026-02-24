#include "task_manager.hh"

#include "fic_log.hh"

TaskManager::TaskManager(std::string name, uint32_t stack_size, BaseType_t core_id): _name(name), _stack_size(stack_size), _core_id(core_id) {

}

void TaskManager::run(void* pvParameters) {
    auto* self = static_cast<TaskManager*>(pvParameters);
    
    FIC_LOGI(TAG, "Setting up tasks for manager %s", self->_name.c_str());
    for (size_t i = 0; i < self->_task_count; i++) self->_tasks[i]->setup();

    FIC_LOGI(TAG, "Manager %s tasks start", self->_name.c_str());
    while (self->_running) {
        uint64_t now = pdTICKS_TO_MS(xTaskGetTickCount());

        for (size_t i = 0; i < self->_task_count; i++) {
            ITask* task = self->_tasks[i];

            if (!task->should_run(now)) continue;
            task->update(now);
        }
        
        vTaskDelay(1);
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
        FIC_LOGE(TAG, "Task limit reached!");
    }
}

void TaskManager::stop() { 
    _running = false; 
}