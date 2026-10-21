#include "fw/platform/stm32f4/common/gpio_output.h"

#include <cstdint>

#include "fw/platform/stm32f4/common/gpio_output.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform {

GpioOutput::GpioOutput(const OutputPinConfig& config)
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

void GpioOutput::Set(bool high) {
  if (high) {
    LL_GPIO_SetOutputPin(port_, pin_);
  } else {
    LL_GPIO_ResetOutputPin(port_, pin_);
  }
}

void GpioOutput::Toggle() { LL_GPIO_TogglePin(port_, pin_); }

}  // namespace fw::platform
