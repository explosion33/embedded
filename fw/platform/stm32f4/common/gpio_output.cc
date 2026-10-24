#include "fw/platform/stm32f4/common/gpio_output.h"

#include <cstdint>
#include <optional>

#include "fw/log/check.h"
#include "fw/platform/peripherals/pin_map.h"
#include "fw/platform/stm32f4/common/gpio_output.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform {

namespace {

// Unwraps a std::optional with FatalError.
template <typename T>
constexpr T Unwrap(std::optional<T> val) {
  CHECK(val.has_value());
  return val.value();
}

// Get the LL Port def pointer for this port.
GPIO_TypeDef* GetLLPort(Port port) {
  GPIO_TypeDef* base = GPIOA;
  const size_t stride = GPIOB - GPIOA;

  return static_cast<GPIO_TypeDef*>(static_cast<size_t>(port) * stride + base);
}

// Gets the LL port clock mask for the provided port.
constexpr uint32_t GetLLPortClock(Port port) {
  return LL_AHB1_GRP1_PERIPH_GPIOA << static_cast<size_t>(port);
}

// Gets the LL pin mask for the provided pin.
constexpr uint32_t GetLLPin(uint8_t pin_idx) {
  return LL_GPIO_PIN_0 << pin_idx;
}

}  // namespace

GpioOutput::GpioOutput(Pin pin)
    : port_(Unwrap(GetPort(pin))), pin_(Unwrap(GetPinIndex(pin))) {
  CHECK(Unwrap(SupportsOutput(pin)));
  const auto ll_port = GetLLPort(port_);
  const auto ll_pin = GetLLPin(pin_);

  LL_AHB1_GRP1_EnableClock(GetLLPortClock(port_));
  // Set the level before switching the pin to output, so it never glitches
  // high.
  LL_GPIO_ResetOutputPin(ll_port, ll_pin);

  LL_GPIO_InitTypeDef gpio_init = {
      .Pin = ll_pin,
      .Mode = LL_GPIO_MODE_OUTPUT,
      .Speed = LL_GPIO_SPEED_FREQ_LOW,
      .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
      .Pull = LL_GPIO_PULL_NO,
      .Alternate = LL_GPIO_AF_0,
  };
  LL_GPIO_Init(ll_port, &gpio_init);
}

void GpioOutput::Set(bool high) {
  const auto ll_port = GetLLPort(port_);
  const auto ll_pin = GetLLPin(pin_);

  if (high) {
    LL_GPIO_SetOutputPin(ll_port, ll_pin);
  } else {
    LL_GPIO_ResetOutputPin(ll_port, ll_pin);
  }
}

void GpioOutput::Toggle() {
  const auto ll_port = GetLLPort(port_);
  const auto ll_pin = GetLLPin(pin_);
  LL_GPIO_TogglePin(ll_port, ll_pin);
}

}  // namespace fw::platform
