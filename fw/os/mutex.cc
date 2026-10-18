#include "fw/os/mutex.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "fw/log/check.h"

namespace fw::os {

Mutex::Mutex() { handle_ = xSemaphoreCreateMutex(); }

Mutex::~Mutex() {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  if (locked_) {
    Unlock();
  }
  vSemaphoreDelete(handle);
}

void Mutex::Lock() {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  CHECK(xSemaphoreTake(handle, portMAX_DELAY) == pdTRUE);
  locked_ = true;
}

bool Mutex::TryLockFor(size_t millis) {
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  if (xSemaphoreTake(handle, pdMS_TO_TICKS(millis)) == pdTRUE) {
    locked_ = true;
    return true;
  };
  return false;
}

void Mutex::Unlock() {
  CHECK(locked_);
  SemaphoreHandle_t handle = static_cast<SemaphoreHandle_t>(handle_);
  CHECK(xSemaphoreGive(handle) == pdTRUE);
  locked_ = false;
}

}  // namespace fw::os