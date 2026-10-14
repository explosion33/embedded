#include "fw/platform/stm32f4/common/gpio.h"

#include <cstdint>

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace platform {

OutputPin::OutputPin(const OutputPinConfig& config)
    : port_(config.port), pin_(config.pin) {
  LL_AHB1_GRP1_EnableClock(config.port_clock);

  // Set the level before switching the pin to output, so it never glitches
  // high.
  LL_GPIO_ResetOutputPin(port_, pin_);

  LL_GPIO_InitTypeDef gpio_init = {
      .Pin = pin_,
      .Mode = LL_GPIO_MODE_OUTPUT,
      .Speed = LL_GPIO_SPEED_FREQ_LOW,
      .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
      .Pull = LL_GPIO_PULL_NO,
      .Alternate = LL_GPIO_AF_0,
  };
  LL_GPIO_Init(port_, &gpio_init);
}

void OutputPin::Set(bool high) {
  if (high) {
    LL_GPIO_SetOutputPin(port_, pin_);
  } else {
    LL_GPIO_ResetOutputPin(port_, pin_);
  }
}

void OutputPin::Toggle() { LL_GPIO_TogglePin(port_, pin_); }

}  // namespace platform
