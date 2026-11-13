#pragma once

// Board configuration for the Nucleo-F446RE.

#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/platform/stm32f4/common/uart.h"

namespace fw::board {

// The user LED, LD2 (PA5).
constexpr platform::pins::Pin kUserLed = platform::pins::Pin::kPC13;

// Returns the console UART, configured from `kConsoleUart` on first use. This
// is the only instance of the console, shared by the log backend and apps.
platform::Uart& Console();

}  // namespace fw::board
