#include <cstdio>

#include "fw/platform/stm32f4/common/gpio_input.h"
#include "fw/platform/stm32f4/common/gpio_output.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/testing/test.h"

namespace {

using fw::platform::GpioInput;
using fw::platform::GpioOutput;
using fw::platform::pins::Pin;

// Requires test board with PA_0 and PA_1 bridged together.
TEST(DigitalGpioLoopBack) {
  constexpr Pin kOut = Pin::kPA0;
  constexpr Pin kIn = Pin::kPA1;

  GpioOutput output(kOut);
  GpioInput input(kIn);

  output.Set(true);
  EXPECT_TRUE(input.Read());

  output.Set(false);
  EXPECT_FALSE(input.Read());
}

}  // namespace
