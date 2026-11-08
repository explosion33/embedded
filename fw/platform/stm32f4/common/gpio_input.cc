#include "fw/platform/stm32f4/common/gpio_input.h"

#include "fw/platform/stm32f4/common/ll_helpers.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform {

GpioInput::GpioInput(pins::Pin pin)
    : port_(GetPort(pin)), pin_(GetPinIndex(pin)) {
  const auto ll_port = internal::GetLLPort(port_);
  const auto ll_pin = internal::GetLLPin(pin_);

  LL_AHB1_GRP1_EnableClock(internal::GetLLPortClock(port_));

  LL_GPIO_InitTypeDef gpio_init = {
      .Pin = ll_pin,
      .Mode = LL_GPIO_MODE_INPUT,
      .Speed = LL_GPIO_SPEED_FREQ_LOW,
      .Pull = LL_GPIO_PULL_NO,
  };
  LL_GPIO_Init(ll_port, &gpio_init);
}

bool GpioInput::Read() const {
  return LL_GPIO_IsInputPinSet(internal::GetLLPort(port_),
                               internal::GetLLPin(pin_)) == 1;
}
}  // namespace fw::platform
