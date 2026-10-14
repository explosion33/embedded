// Application hooks the FreeRTOS kernel calls when it detects a fatal error.
//
// Each one masks interrupts, stops at a breakpoint if a debugger is attached,
// and otherwise spins forever so the failure is not silently ignored.
// TODO: Report the failure over the console, or reset, once there is a
// logging story that is safe to call from a broken system.

#include "FreeRTOS.h"
#include "task.h"

namespace {

// Halts the current program and spins forever.
[[noreturn]] void Halt() {
  taskDISABLE_INTERRUPTS();
  // Without a debugger attached, bkpt escalates to a HardFault, whose default
  // handler also spins forever.
  __asm__ volatile("bkpt #0");
  while (true) {
  }
}

}  // namespace

extern "C" {

void vAssertCalled(const char* file, int line) {
  // Kept volatile so a debugger can read where the assertion fired.
  [[maybe_unused]] const char* volatile assert_file = file;
  [[maybe_unused]] volatile int assert_line = line;
  Halt();
}

void vApplicationStackOverflowHook(TaskHandle_t task, char* task_name) {
  [[maybe_unused]] char* volatile overflowed_task_name = task_name;
  Halt();
}

void vApplicationMallocFailedHook() { Halt(); }

}  // extern "C"
