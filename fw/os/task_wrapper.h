#pragma once

#include <cstddef>
#include <string_view>

namespace fw::os::internal {

// Configuration for a FreeRTOS task.
struct TaskConfig {
  // Entry point for the task. Must not return; it must end by calling
  // ExitCurrentTask().
  void (*fn)(void*);
  void* arg;
  std::string_view name;
  size_t stack_depth_bytes;
  size_t priority;
};

// Spawns a FreeRTOS task with the provided configuration.
void CreateTask(const TaskConfig& task);

// Exits function called with a task.
[[noreturn]] void ExitCurrentTask();

// Gets the minimal FreeRTOs stack size.
size_t GetMinimalStackSize();

}  // namespace fw::os::internal
