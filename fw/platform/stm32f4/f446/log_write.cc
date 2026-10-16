#include <cstdint>
#include <string_view>

#include "fw/log/backend.h"
#include "fw/platform/stm32f4/common/uart.h"
#include "fw/platform/stm32f4/f446/platform.h"

namespace fw::log::backend {

// TODO: This definition should not live here. This is STM32F446 generic code,
// but assumes device specifc. Additional per-device targets should be created
// and maintained.
void Write(std::string_view text) {
  static platform::Uart console(platform::kConsoleUart);
  console.Write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

}  // namespace fw::log::backend
