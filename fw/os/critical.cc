#include "fw/os/critical.h"

#include "FreeRTOS.h"
#include "task.h"

namespace fw::os {

CriticalSection::CriticalSection() { vPortEnterCritical(); }
CriticalSection::~CriticalSection() { vPortExitCritical(); }

}  // namespace fw::os