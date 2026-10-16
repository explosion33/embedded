#include "fw/os/task_wrapper.h"

#include <cstring>

#include "FreeRTOS.h"
#include "task.h"

#include "fw/log/check.h"

namespace fw::os::internal {

void CreateTask(const TaskConfig& task) {
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

  CHECK(xTaskCreate(task.fn, name, depth, task.arg, task.priority, nullptr) ==
        pdPASS)
      << "Spawning task " << task.name;
}

void ExitCurrentTask() {
  vTaskDelete(nullptr);
  // vTaskDelete(nullptr) does not return, but isn't marked [[noreturn]].
  while (true) {
  }
}

size_t GetMinimalStackSize() {
  return configMINIMAL_STACK_SIZE * sizeof(StackType_t);
}

}  // namespace fw::os::internal
