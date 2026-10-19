#include "fw/os/task_wrapper.h"

#include <cstring>
#include <memory>

#include "FreeRTOS.h"
#include "task.h"

#include "fw/log/check.h"

namespace fw::os::internal {

Task::Task(const Config& task) {
  CHECK(task.fn != nullptr);
  CHECK(task.priority < configMAX_PRIORITIES);
  // configMAX_TASK_NAME_LEN includes the null terminator.
  CHECK(task.name.size() < configMAX_TASK_NAME_LEN);
  CHECK(task.stack_depth_bytes % sizeof(StackType_t) == 0);
  const configSTACK_DEPTH_TYPE depth =
      task.stack_depth_bytes / sizeof(StackType_t);
  CHECK(depth > 0);

  // string_view isn't guaranteed to be null terminated. FreeRTOS copies the
  // name into the TCB, so a local buffer is fine.
  char name[configMAX_TASK_NAME_LEN] = {};
  memcpy(name, task.name.data(), task.name.size());

  TaskHandle_t handle;
  CHECK(xTaskCreate(task.fn, name, depth, task.arg, task.priority, &handle) ==
        pdPASS)
      << "Spawning task " << task.name;
  handle_ = handle;
  CHECK(handle_ != nullptr);
}

void Task::Pause() {
  CHECK(!paused_);
  CHECK(handle_ != nullptr);
  vTaskSuspend(static_cast<TaskHandle_t>(handle_));
  paused_ = true;
}

void Task::Resume() {
  CHECK(paused_);
  CHECK(handle_ != nullptr);
  vTaskResume(static_cast<TaskHandle_t>(handle_));
  paused_ = false;
}

void Task::Stop() {
  CHECK(handle_ != nullptr);
  vTaskDelete(static_cast<TaskHandle_t>(handle_));
  handle_ = nullptr;
}

void Task::ExitCurrentTask() {
  vTaskDelete(nullptr);
  // vTaskDelete(nullptr) does not return, but isn't marked [[noreturn]].
  while (true) {
  }
}

size_t Task::GetMinimalStackSize() {
  return configMINIMAL_STACK_SIZE * sizeof(StackType_t);
}

}  // namespace fw::os::internal
