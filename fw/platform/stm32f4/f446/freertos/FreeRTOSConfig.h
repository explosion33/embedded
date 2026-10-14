#pragma once

// FreeRTOS configuration for the STM32F446RE.

// Allocate 64K of the 128K SRAM to the heap.
#define configTOTAL_HEAP_SIZE (64 * 1024)

// Pull in STM32F4 Defaults.
#include "fw/platform/stm32f4/common/freertos/stm32f4_freertos_config.h"
