#include "fw/os/constants.h"

#include "FreeRTOS.h"

namespace fw::os {
size_t GetMaxSysCallInterruptPriority() {
  return configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
}
}  // namespace fw::os