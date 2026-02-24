#include "freertos_task.hh"


FreeRTOS_TaskRunner::FreeRTOS_TaskRunner(const char* name, uint32_t stack_size, BaseType_t core_id)
    : _name(name), _stack_size(stack_size), _core_id(core_id) {}


FreeRTOS_TaskRunner::~FreeRTOS_TaskRunner() = default;
void FreeRTOS_TaskRunner::create_task(void (*run)(void*), void* params) {
    xTaskCreatePinnedToCore(run, _name, _stack_size, params, 10, &_handle, _core_id);
}

void FreeRTOS_TaskRunner::delete_task() {
    if (_handle != nullptr) {
        vTaskDelete(_handle);
        _handle = nullptr;
    }
}

