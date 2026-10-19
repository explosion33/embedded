#pragma once

#include <cstddef>
#include <string_view>

namespace fw::os::internal {

// Wrapper around a FreeRTOS task that avoids exposing FreeRTOS code.
class Task {
 public:
  // Configuration for a FreeRTOS task.
  struct Config {
    // Entry point for the task. Must not return; it must end by calling
    // ExitCurrentTask().
    void (*fn)(void*);
    void* arg;
    std::string_view name;
    size_t stack_depth_bytes;
    size_t priority;
  };

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  explicit Task(const Config& task);

  // Pauses the tasks execution.
  void Pause();

  // Resumes a paused tasks execution.
  void Resume();

  // Stops the threads execution, once stopped a task cannot be restarted.
  void Stop();

  // Exits the task. Must be called from within a running task.
  [[noreturn]] static void ExitCurrentTask();

  // Gets the minimal FreeRTOS stack size.
  static size_t GetMinimalStackSize();

 private:
  // Type erased TaskHandle.
  void* handle_ = nullptr;

  // True if the task is currently paused.
  bool paused_ = false;
};

}  // namespace fw::os::internal
