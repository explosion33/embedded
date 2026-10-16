#pragma once

// Part configuration for the STM32F446RE.

#include <cstdint>

namespace platform {

// By default the MCU boots on the 16MHz HSI, with no PLLs configured.
constexpr uint32_t kDefaultSysClkHz = 16'000'000;
}  // namespace platform
