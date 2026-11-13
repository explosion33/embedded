#include "fw/platform/stm32f4/common/interrupts.h"

#include <cstdint>
#include <utility>

#include "fw/log/check.h"
#include "fw/os/constants.h"
#include "fw/os/critical.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_system.h"

namespace fw::platform {

namespace {

// The LL EXTI Interrupt handles.
constexpr IRQn_Type kExtiIrqs[] = {
    EXTI0_IRQn, EXTI1_IRQn,   EXTI2_IRQn,     EXTI3_IRQn,
    EXTI4_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn,
};

// Gets the LL exti line mask for a given index.
constexpr uint32_t GetLLExtiLine(size_t idx) {
#define CASE(idx) \
  case idx:       \
    return LL_SYSCFG_EXTI_LINE##idx;

  switch (idx) {
    CASE(0)
    CASE(1)
    CASE(2)
    CASE(3)
    CASE(4)
    CASE(5)
    CASE(6)
    CASE(7)
    CASE(8)
    CASE(9)
    CASE(10)
    CASE(11)
    CASE(12)
    CASE(13)
    CASE(14)
    CASE(15)
  }
#undef CASE
  CHECK(false);
}

}  // namespace

Interrupts::Interrupts() {
  os::CriticalSection _;
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

  LL_EXTI_ClearFlag_0_31(0xFFFF);
  for (IRQn_Type irq : kExtiIrqs) {
    NVIC_ClearPendingIRQ(irq);
    NVIC_SetPriority(irq, os::GetMaxSysCallInterruptPriority());
    NVIC_EnableIRQ(irq);
  }
}

void Interrupts::AddGpioIrq(pins::Pin pin, Edge edge, Callback fn) {
  CHECK(fn != nullptr);
  const uint8_t idx = pins::GetPinIndex(pin);
  const uint32_t port_idx = std::to_underlying(pins::GetPort(pin));
  CHECK(idx < kNumGpioLanes);

  mutex_.Lock();
  CHECK(!fns_[idx].has_value());
  fns_[idx].emplace(std::move(fn));
  mutex_.Unlock();

  // LL Register reads and writes are critical and must not invoke a interrupt
  // based context switch.
  os::CriticalSection _;

  LL_SYSCFG_SetEXTISource(port_idx, GetLLExtiLine(idx));

  const uint32_t mask = 1U << idx;

  if (edge == Edge::kBoth || edge == Edge::kFalling) {
    LL_EXTI_EnableFallingTrig_0_31(mask);
  } else {
    LL_EXTI_DisableFallingTrig_0_31(mask);
  }

  if (edge == Edge::kBoth || edge == Edge::kRising) {
    LL_EXTI_EnableRisingTrig_0_31(mask);
  } else {
    LL_EXTI_DisableRisingTrig_0_31(mask);
  }

  LL_EXTI_ClearFlag_0_31(mask);
  LL_EXTI_EnableIT_0_31(mask);
}

void Interrupts::RemoveGpioIrq(pins::Pin pin) {
  const uint8_t idx = pins::GetPinIndex(pin);
  const uint32_t mask = 1U << idx;
  CHECK(idx < kNumGpioLanes);

  {
    os::CriticalSection _;
    LL_EXTI_DisableIT_0_31(mask);
  }

  mutex_.Lock();
  CHECK(fns_[idx].has_value());
  fns_[idx].reset();
  mutex_.Unlock();
}

void Interrupts::Execute(size_t idx) {
  CHECK(idx < kNumGpioLanes);
  CHECK(fns_[idx].has_value());
  fns_[idx].value()();
}

}  // namespace fw::platform

#define SINGLE_EXTI_HANDLER(idx)                                             \
  extern "C" void EXTI##idx##_IRQHandler(void) {                             \
    constexpr uint32_t kLine = EXTI_PR_PR##idx;                              \
                                                                             \
    /*If the pending bit for this line is empty, do nothing. */              \
    if ((EXTI->PR & kLine) == 0) {                                           \
      return;                                                                \
    }                                                                        \
    /*The pending bit is W1C.*/                                              \
    EXTI->PR = kLine;                                                        \
                                                                             \
    /*Call to Execute() is thread-safe since we are within an ISR context.*/ \
    fw::platform::Interrupts::Get().Execute(idx);                            \
  }

SINGLE_EXTI_HANDLER(0)
SINGLE_EXTI_HANDLER(1)
SINGLE_EXTI_HANDLER(2)
SINGLE_EXTI_HANDLER(3)
SINGLE_EXTI_HANDLER(4)

#undef SINGLE_EXTI_HANDLER

#define MULTI_EXTI_HANDLER(start_idx, end_idx)                                 \
  extern "C" void EXTI##end_idx##_##start_idx##_IRQHandler(void) {             \
    const uint32_t pending = EXTI->PR & EXTI->IMR;                             \
    for (size_t i = start_idx; i <= end_idx; i++) {                            \
      const uint32_t line = 1UL << i;                                          \
                                                                               \
      /*If the pending bit for this line is empty, do nothing. */              \
      if ((pending & line) == 0) {                                             \
        continue;                                                              \
      }                                                                        \
      /*The pending bit is W1C.*/                                              \
      EXTI->PR = line;                                                         \
                                                                               \
      /*Call to Execute() is thread-safe since we are within an ISR context.*/ \
      fw::platform::Interrupts::Get().Execute(i);                              \
    }                                                                          \
  }

MULTI_EXTI_HANDLER(5, 9)
MULTI_EXTI_HANDLER(10, 15)

#undef MULTI_EXTI_HANDLER