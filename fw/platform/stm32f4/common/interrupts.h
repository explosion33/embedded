#pragma once

#include <array>
#include <functional>
#include <optional>

#include "fw/os/mutex.h"
#include "fw/platform/stm32f4/common/pin_map.h"

namespace fw::platform {

// Handles configuring interrupts on the STM32F4 target. Should have one
// isntance constructed per chip.
class Interrupts {
 public:
  using Callback = std::function<void()>;

  Interrupts(const Interrupts&) = delete;
  Interrupts& operator=(const Interrupts&) = delete;

  // An interrupt trigger edge.
  enum class Edge {
    kRising,
    kFalling,
    kBoth,
  };

  // There are 16 external interrupt lanes for the STM32F4 series.
  // https://stm32f4-discovery.net/2014/08/stm32f4-external-interrupts-tutorial/
  static constexpr size_t kNumGpioLanes = 16;

  // Gets the Interrupts for this platform.
  static Interrupts& Get() {
    static Interrupts interrupts;
    return interrupts;
  }

  // Registers `fn` as the ISR for an IRQ on `pin`.
  void AddGpioIrq(pins::Pin pin, Edge edge, Callback fn);

  // Removes the registered ISR on `pin`.
  void RemoveGpioIrq(pins::Pin pin);

  // Public facing function to execute one of the backing callbacks. Should be
  // called from ISR context.
  void Execute(size_t idx);

 private:
  Interrupts();

  // The mutex protecting stored function state.
  os::Mutex mutex_;

  // The stored GPIO ISR functions.
  std::array<std::optional<Callback>, kNumGpioLanes> fns_;
};

}  // namespace fw::platform