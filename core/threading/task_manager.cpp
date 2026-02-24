#include "task_manager.hh"

#include "fic_log.hh"

TaskManager::TaskManager(const char* name, std::unique_ptr<ITaskRunner> task_runner)
    : _name(name), _task_runner(std::move(task_runner)) {}

TaskManager::~TaskManager() {
    stop();

    while(!_active) {
        _task_runner->delay(10); // FIXME: use clock or delay source, not task runner
    }
}

void TaskManager::run(void* pvParameters) {
    auto* self = static_cast<TaskManager*>(pvParameters);
    self->_active = true;
    
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
        
        self->_task_runner->delay(1);
    }

    self->_task_runner->delete_task();
    self->_active = false;
}

void TaskManager::start() {
    if (_running) return;
    _running = true;

    _task_runner->create_task(run, this);
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