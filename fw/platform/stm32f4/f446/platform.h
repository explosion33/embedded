#pragma once

// Part configuration for the STM32F446RE.

#include <cstdint>

// Unused here, but included so target-dependent includes get the full pin-map.
#include "fw/platform/stm32f4/f446/stm32f446re_pin_map.h"

namespace fw::platform {

// By default the MCU boots on the 16MHz HSI, with no PLLs configured.
constexpr uint32_t kDefaultSysClkHz = 16'000'000;
}  // namespace fw::platform
