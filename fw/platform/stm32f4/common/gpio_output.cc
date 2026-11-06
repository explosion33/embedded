#include "fw/platform/stm32f4/common/gpio_output.h"

#include <cstdint>
#include <optional>

#include "fw/log/check.h"
#include "fw/platform/stm32f4/common/gpio_output.h"
#include "fw/platform/stm32f4/common/ll_helpers.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform {

namespace {}  // namespace

GpioOutput::GpioOutput(pins::Pin pin)
    : port_(GetPort(pin)), pin_(GetPinIndex(pin)) {
  const auto ll_port = internal::GetLLPort(port_);
  const auto ll_pin = internal::GetLLPin(pin_);

  LL_AHB1_GRP1_EnableClock(internal::GetLLPortClock(port_));
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
  const auto ll_port = internal::GetLLPort(port_);
  const auto ll_pin = internal::GetLLPin(pin_);

  if (high) {
    LL_GPIO_SetOutputPin(ll_port, ll_pin);
  } else {
    LL_GPIO_ResetOutputPin(ll_port, ll_pin);
  }
}

void GpioOutput::Toggle() {
  const auto ll_port = internal::GetLLPort(port_);
  const auto ll_pin = internal::GetLLPin(pin_);
  LL_GPIO_TogglePin(ll_port, ll_pin);
}

}  // namespace fw::platform
