#include "fw/log/backend.h"
#include "stm32f4xx.h"

namespace fw::log::backend {

[[noreturn]] void Halt() {
  __disable_irq();

  // Stop in the debugger, if one is attached. Without a debugger, BKPT
  // escalates to a HardFault, so it must be guarded.
  if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0) {
    __BKPT(0);
  }

  while (true) {
    __NOP();
  }
}

}  // namespace fw::log::backend
