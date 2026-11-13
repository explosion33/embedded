#include <cstdio>

#include "fw/os/mutex.h"
#include "fw/os/task_wrapper.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/platform/stm32f4/common/gpio_input.h"
#include "fw/platform/stm32f4/common/gpio_output.h"
#include "fw/platform/stm32f4/common/interrupts.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/testing/test.h"

// Peripheral tests for STM32F4 based boards. Requires the following pin
// connections to function:
// PA_0 -> PA_1.
// PA_4 is floating.
namespace {

using fw::platform::GpioInput;
using fw::platform::GpioOutput;
using fw::platform::Interrupts;
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

TEST(RisingInterrupt) {
  constexpr Pin kPin = Pin::kPA0;

  GpioOutput output(kPin);
  output.Set(true);

  Interrupts& interrupts = Interrupts::Get();

  volatile size_t calls = 0;
  interrupts.AddGpioIrq(kPin, Interrupts::Edge::kRising,
                        [&calls]() { calls = calls + 1; });

  // Interrupt should not fire until the edge occurs.
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);

  // A rising edge interrupt should not fire on a falling edge.
  output.Set(false);
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);

  output.Set(true);
  fw::os::Delay(1);
  EXPECT_EQ(calls, 1);

  // Removing the interrupt should no longer invoke the callback.
  interrupts.RemoveGpioIrq(kPin);
  output.Toggle();
  output.Toggle();
  output.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls, 1);
}

TEST(FallingInterrupt) {
  constexpr Pin kPin = Pin::kPB7;

  GpioOutput output(kPin);
  output.Set(false);

  Interrupts& interrupts = Interrupts::Get();

  volatile size_t calls = 0;
  interrupts.AddGpioIrq(kPin, Interrupts::Edge::kFalling,
                        [&calls]() { calls = calls + 1; });

  // Interrupt should not fire until the edge occurs.
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);

  // A falling edge interrupt should not fire on a rising edge.
  output.Set(true);
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);

  output.Set(false);
  fw::os::Delay(1);
  EXPECT_EQ(calls, 1);

  // Removing the interrupt should no longer invoke the callback.
  interrupts.RemoveGpioIrq(kPin);
  output.Toggle();
  output.Toggle();
  output.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls, 1);
}

TEST(BothInterrupt) {
  constexpr Pin kPin = Pin::kPC14;

  GpioOutput output(kPin);
  output.Set(false);

  Interrupts& interrupts = Interrupts::Get();

  volatile size_t calls = 0;
  interrupts.AddGpioIrq(kPin, Interrupts::Edge::kBoth,
                        [&calls]() { calls = calls + 1; });

  // Interrupt should not fire until the edge occurs.
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);

  for (size_t i = 1; i < 10; i++) {
    output.Toggle();
    fw::os::Delay(1);
    EXPECT_EQ(calls, i);
  }

  calls = 0;

  // Removing the interrupt should no longer invoke the callback.
  interrupts.RemoveGpioIrq(kPin);
  output.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls, 0);
}

TEST(MultiInterruptMultiLine) {
  constexpr Pin kPin0 = Pin::kPC14;
  constexpr Pin kPin1 = Pin::kPB15;

  GpioOutput output_0(kPin0);
  GpioOutput output_1(kPin1);
  output_0.Set(false);
  output_1.Set(false);

  Interrupts& interrupts = Interrupts::Get();

  volatile size_t calls_0 = 0;
  volatile size_t calls_1 = 0;
  interrupts.AddGpioIrq(kPin0, Interrupts::Edge::kBoth,
                        [&calls_0]() { calls_0 = calls_0 + 1; });
  interrupts.AddGpioIrq(kPin1, Interrupts::Edge::kBoth,
                        [&calls_1]() { calls_1 = calls_1 + 1; });

  output_0.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls_0, 1);
  EXPECT_EQ(calls_1, 0);

  output_1.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls_0, 1);
  EXPECT_EQ(calls_1, 1);

  interrupts.RemoveGpioIrq(kPin0);

  output_0.Toggle();
  output_1.Toggle();
  fw::os::Delay(1);
  EXPECT_EQ(calls_0, 1);
  EXPECT_EQ(calls_1, 2);

  interrupts.RemoveGpioIrq(kPin1);
}

TEST(FreeRTOSInterrupt) {
  constexpr Pin kPin = Pin::kPH1;

  GpioOutput output(kPin);
  output.Set(false);

  Interrupts& interrupts = Interrupts::Get();

  fw::os::Thread t;
  size_t calls = 0;
  t.Start([&calls]() {
    fw::os::Thread::WaitForNotification();

    // Locking a mutex is not possible in an ISR context.
    fw::os::Mutex mtx;
    mtx.Lock();
    calls++;
  });

  interrupts.AddGpioIrq(kPin, Interrupts::Edge::kBoth,
                        [&t]() { t.Notify(/*from_isr=*/true); });
  output.Toggle();
  t.Join();
  EXPECT_EQ(calls, 1);
}

}  // namespace
