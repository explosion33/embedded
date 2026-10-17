#pragma once

// FreeRTOS configuration for the STM32F446.
//
// Do not include this directly. Each STM32F446 board provides its own
// FreeRTOSConfig.h, which defines anything it needs to differ and then includes
// this file. Every setting here is guarded by #ifndef so those earlier
// definitions win.

// Allocate 64K of the 128K SRAM to the heap.
#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE (64 * 1024)
#endif

// Pull in STM32F4 Defaults.
#include "fw/platform/stm32f4/common/freertos/stm32f4_freertos_config.h"
