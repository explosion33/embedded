#include "fw/os/semaphore.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "fw/log/check.h"

namespace fw::os {

Semaphore::Semaphore(size_t max_count) {
  CHECK(max_count > 0);
  if (max_count == 1) {
    handle_ = xSemaphoreCreateBinary();
  } else {
    handle_ = xSemaphoreCreateCounting(max_count, 0);
  }
  CHECK(handle_ != nullptr);
}

Semaphore::~Semaphore() { vSemaphoreDelete(handle_); }

bool Semaphore::Give() {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  return xSemaphoreGive(handle) == pdTRUE;
}

void Semaphore::Take() {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  CHECK(xSemaphoreTake(handle, portMAX_DELAY));
}

bool Semaphore::TryTakeFor(size_t millis) {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  return xSemaphoreTake(handle, pdMS_TO_TICKS(millis)) == pdTRUE;
}

size_t Semaphore::Count() {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  return uxSemaphoreGetCount(handle);
}

}  // namespace fw::os