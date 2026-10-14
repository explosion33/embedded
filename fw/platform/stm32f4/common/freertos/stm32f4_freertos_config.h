#pragma once

// FreeRTOS configuration shared by every STM32F4 part.
//
// Do not include this directly. Each part provides its own FreeRTOSConfig.h,
// which defines anything it needs to differ and then includes this file. Every
// setting here is guarded by #ifndef so those earlier definitions win.
//
// Settings with no sensible family-wide value (e.g. configTOTAL_HEAP_SIZE,
// which depends on the part's RAM) have no default and must be set per part.
//
// Reference:
// https://www.freertos.org/Documentation/02-Kernel/03-Supported-devices/02-Customization

#include "stm32f4xx.h"

// ---------------------------------------------------------------------------
// Settings each part must provide.

#ifndef configTOTAL_HEAP_SIZE
#error "configTOTAL_HEAP_SIZE must be set by the part's FreeRTOSConfig.h"
#endif

// ---------------------------------------------------------------------------
// Clock and scheduling.

// Tracks the live core clock, so changing the clock tree before starting the
// scheduler needs no config change.
// TODO: Configure the PLL. The F4 currently runs from the 16MHz HSI.
#ifndef configCPU_CLOCK_HZ
#define configCPU_CLOCK_HZ SystemCoreClock
#endif

// Set the granualrity of tick based delays / timeouts to 1ms.
#ifndef configTICK_RATE_HZ
#define configTICK_RATE_HZ 1000
#endif

// Allow higher priority tasks to run ahead of lower priority tasks when both
// are available.
#ifndef configUSE_PREEMPTION
#define configUSE_PREEMPTION 1
#endif

// Allow tasks of equal priority to round robin for CPU access.
#ifndef configUSE_TIME_SLICING
#define configUSE_TIME_SLICING 1
#endif

// Uses the Cortex-M CLZ instruction to pick the next task. Limits
// configMAX_PRIORITIES to 32.
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

// Disables the supression of tick while tasks are idle.
// TODO: This disabled low-power modes, if these are needed this should be
// enabled. This would require enabling the low power timer and time-correction
// on wake.
#ifndef configUSE_TICKLESS_IDLE
#define configUSE_TICKLESS_IDLE 0
#endif

// Configure 8 task priority levels.
#ifndef configMAX_PRIORITIES
#define configMAX_PRIORITIES 8
#endif

// Set the minimal stack size to 128b. Stacks can grow larger as needed.
#ifndef configMINIMAL_STACK_SIZE
#define configMINIMAL_STACK_SIZE 128
#endif

// Maximum of 16 characters (+ null terminator) for task names.
#ifndef configMAX_TASK_NAME_LEN
#define configMAX_TASK_NAME_LEN 16
#endif

// Sets the tick counter bit-width to 32b.
#ifndef configTICK_TYPE_WIDTH_IN_BITS
#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS
#endif

// The idle task will yield to tasks of equal priority.
#ifndef configIDLE_SHOULD_YIELD
#define configIDLE_SHOULD_YIELD 1
#endif

// Disable FreeRTOS APIs.
#ifndef configENABLE_BACKWARD_COMPATIBILITY
#define configENABLE_BACKWARD_COMPATIBILITY 0
#endif

// Sets the variable type used in stack depth related arguments / interfaces.
#ifndef configSTACK_DEPTH_TYPE
#define configSTACK_DEPTH_TYPE size_t
#endif

// Sets the variable type used in message buffer related arguments / interfaces.
#ifndef configMESSAGE_BUFFER_LENGTH_TYPE
#define configMESSAGE_BUFFER_LENGTH_TYPE size_t
#endif

// ---------------------------------------------------------------------------
// Memory.

// Enables tasks / objects to be allocated on the heap.
#ifndef configSUPPORT_DYNAMIC_ALLOCATION
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#endif

// Enables allocating tasks / objects with static application provided storage.
#ifndef configSUPPORT_STATIC_ALLOCATION
#define configSUPPORT_STATIC_ALLOCATION 1
#endif

// Lets the kernel own the idle and timer task storage, rather than requiring
// the application to provide vApplicationGet*TaskMemory().
#ifndef configKERNEL_PROVIDED_STATIC_MEMORY
#define configKERNEL_PROVIDED_STATIC_MEMORY 1
#endif

// Stops memory from being directly cleared once freed.
#ifndef configHEAP_CLEAR_MEMORY_ON_FREE
#define configHEAP_CLEAR_MEMORY_ON_FREE 0
#endif

// newlib's per-task reentrancy structs are not used. Only one task may call
// into stateful newlib functions (stdio, strtok, errno). newlib's malloc is
// disabled entirely, see freertos_heap.cc.
#ifndef configUSE_NEWLIB_REENTRANT
#define configUSE_NEWLIB_REENTRANT 0
#endif

// ---------------------------------------------------------------------------
// Interrupt priorities.
//
// Cortex-M priorities are numerically inverted: 0 is the most urgent. ISRs
// that call FreeRTOS *FromISR() APIs must have a numeric priority at or above
// configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. ISRs more urgent than that are
// never masked by the kernel, but must not call into it.

#ifndef configPRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#endif

// The least urgent priority, used for the kernel's own PendSV and SysTick.
#ifndef configLIBRARY_LOWEST_INTERRUPT_PRIORITY
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY ((1 << configPRIO_BITS) - 1)
#endif

#ifndef configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#endif

// The same values shifted into the implemented (most significant) bits of the
// 8-bit priority registers.
#define configKERNEL_INTERRUPT_PRIORITY \
  (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
  (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

// ---------------------------------------------------------------------------
// Exception handlers.
//
// ST's startup file declares these handlers as weak aliases of
// Default_Handler. Renaming the port's handlers to match makes them the strong
// definitions that land in the vector table.

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

// ---------------------------------------------------------------------------
// Hooks and debugging.
//
// The hooks enabled here are defined in freertos_hooks.cc.

#ifndef configUSE_IDLE_HOOK
#define configUSE_IDLE_HOOK 0
#endif

#ifndef configUSE_TICK_HOOK
#define configUSE_TICK_HOOK 0
#endif

#ifndef configUSE_MALLOC_FAILED_HOOK
#define configUSE_MALLOC_FAILED_HOOK 1
#endif

#ifndef configUSE_DAEMON_TASK_STARTUP_HOOK
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0
#endif

// Method 2 checks both the stack pointer and a fill pattern at the end of the
// stack on every context switch.
#ifndef configCHECK_FOR_STACK_OVERFLOW
#define configCHECK_FOR_STACK_OVERFLOW 2
#endif

#ifndef configQUEUE_REGISTRY_SIZE
#define configQUEUE_REGISTRY_SIZE 8
#endif

#ifndef configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY 0
#endif

#ifndef configGENERATE_RUN_TIME_STATS
#define configGENERATE_RUN_TIME_STATS 0
#endif

#ifndef configUSE_STATS_FORMATTING_FUNCTIONS
#define configUSE_STATS_FORMATTING_FUNCTIONS 0
#endif

#ifndef configASSERT
#ifdef __cplusplus
extern "C" {
#endif
// Halts on a failed kernel assertion. Defined in freertos_hooks.cc.
void vAssertCalled(const char* file, int line);
#ifdef __cplusplus
}  // extern "C"
#endif
#define configASSERT(x)                  \
  do {                                   \
    if ((x) == 0) {                      \
      vAssertCalled(__FILE__, __LINE__); \
    }                                    \
  } while (0)
#endif

// ---------------------------------------------------------------------------
// Kernel features.

#ifndef configUSE_TIMERS
#define configUSE_TIMERS 1
#endif

#ifndef configTIMER_TASK_PRIORITY
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#endif

#ifndef configTIMER_TASK_STACK_DEPTH
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2)
#endif

#ifndef configTIMER_QUEUE_LENGTH
#define configTIMER_QUEUE_LENGTH 10
#endif

#ifndef configUSE_EVENT_GROUPS
#define configUSE_EVENT_GROUPS 1
#endif

#ifndef configUSE_STREAM_BUFFERS
#define configUSE_STREAM_BUFFERS 1
#endif

#ifndef configUSE_TASK_NOTIFICATIONS
#define configUSE_TASK_NOTIFICATIONS 1
#endif

#ifndef configTASK_NOTIFICATION_ARRAY_ENTRIES
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1
#endif

#ifndef configUSE_MUTEXES
#define configUSE_MUTEXES 1
#endif

#ifndef configUSE_RECURSIVE_MUTEXES
#define configUSE_RECURSIVE_MUTEXES 1
#endif

#ifndef configUSE_COUNTING_SEMAPHORES
#define configUSE_COUNTING_SEMAPHORES 1
#endif

#ifndef configUSE_QUEUE_SETS
#define configUSE_QUEUE_SETS 0
#endif

#ifndef configUSE_CO_ROUTINES
#define configUSE_CO_ROUTINES 0
#endif

#ifndef configUSE_APPLICATION_TASK_TAG
#define configUSE_APPLICATION_TASK_TAG 0
#endif

#ifndef configNUM_THREAD_LOCAL_STORAGE_POINTERS
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0
#endif

// ---------------------------------------------------------------------------
// Optional API functions.

#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_xTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetIdleTaskHandle 1
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xTimerPendFunctionCall 1
#define INCLUDE_xTaskAbortDelay 1
#define INCLUDE_xTaskGetHandle 1
#define INCLUDE_xTaskResumeFromISR 1
