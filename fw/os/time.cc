#include "fw/os/time.h"

#include "FreeRTOS.h"
#include "task.h"

namespace fw::os {

namespace {
constexpr size_t kMsPerSeconds = 1000;
}

void Delay(size_t millis) {
  const size_t ticks = millis * configTICK_RATE_HZ / kMsPerSeconds;
  configASSERT(ticks > 0);
  vTaskDelay(ticks);
}

}  // namespace fw::os