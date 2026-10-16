// Application hooks the FreeRTOS kernel calls when it detects a fatal error.

#include "FreeRTOS.h"
#include "task.h"

#include "fw/log/log.h"

extern "C" {

void vAssertCalled(const char* file, int line) {
  ::fw::log::internal::Fatal(file, line, "FreeRTOS assertion failed");
}

void vApplicationStackOverflowHook(TaskHandle_t task, char* task_name) {
  LOG_FATAL("Stack overflow in task '%s'", task_name);
}

void vApplicationMallocFailedHook() { LOG_FATAL("FreeRTOS heap exhausted"); }

}  // extern "C"
