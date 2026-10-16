#include "fw/os/main.h"

#include "FreeRTOS.h"
#include "task.h"

namespace {
void AppMainTask(void*) {
  fw::os::AppMain();
  vTaskDelete(nullptr);
}
}  // namespace

int main() {
  configASSERT(xTaskCreate(&AppMainTask, "", configMINIMAL_STACK_SIZE, nullptr,
                           0, nullptr) == pdPASS);
  vTaskStartScheduler();
  configASSERT(false);
}
