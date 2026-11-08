#include <cstdio>

#include "fw/platform/stm32f4/common/gpio_input.h"
#include "fw/platform/stm32f4/common/gpio_output.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/testing/test.h"

// Peripheral tests for STM32F4 based boards. Requires the following pin
// connections to function:
// PA_0 -> PA_1.
// PA_4 is floating.
namespace {

using fw::platform::GpioInput;
using fw::platform::GpioOutput;
using fw::platform::pins::Pin;

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

TEST(InputPinPull) {
  constexpr Pin kOut = Pin::kPA0;
  constexpr Pin kInFloating = Pin::kPA4;
  constexpr Pin kIn = Pin::kPA1;

  // With no output driving it, the input should follow its pull.
  {
    GpioInput input(kInFloating);

    input.SetPull(GpioInput::Pull::kDown);
    EXPECT_FALSE(input.Read());

    input.SetPull(GpioInput::Pull::kUp);
    EXPECT_TRUE(input.Read());
  }

  // A driven input should read the input regardless of the pull state.
  {
    GpioInput input(kIn);
    GpioOutput output(kOut);

    output.Set(true);
    input.SetPull(GpioInput::Pull::kUp);
    EXPECT_TRUE(input.Read());
    input.SetPull(GpioInput::Pull::kDown);
    EXPECT_TRUE(input.Read());

    output.Set(false);
    input.SetPull(GpioInput::Pull::kUp);
    EXPECT_FALSE(input.Read());
    input.SetPull(GpioInput::Pull::kDown);
    EXPECT_FALSE(input.Read());
  }
}

}  // namespace
