#include "fw/os/main.h"

#include "FreeRTOS.h"
#include "task.h"

namespace {
// Arbitrarily sized, increase or decrease as needed. Sized to fit LOG_FATAL
// error messages.
constexpr size_t kStackDepth = configMINIMAL_STACK_SIZE * 5;

void AppMainTask(void*) {
  fw::os::AppMain();
  vTaskDelete(nullptr);
}
}  // namespace

int main() {
  configASSERT(xTaskCreate(&AppMainTask, "", kStackDepth, nullptr, 0,
                           nullptr) == pdPASS);
  vTaskStartScheduler();
  configASSERT(false);
}
