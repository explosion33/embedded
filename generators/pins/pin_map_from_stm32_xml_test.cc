#include "generators/pins/sample_stm32_pin_map.h"
#include "gtest/gtest.h"
#include "magic_enum/magic_enum.hpp"

namespace generators::pins {
namespace {
TEST(PinMapStm32Xml, PinOutputEnabled) {
  constexpr Pin kFalsePin = Pin::kPC14;

  for (Pin pin : magic_enum::enum_values<Pin>()) {
    const auto maybe_output = SupportsOutput(pin);
    EXPECT_TRUE(maybe_output.has_value());
    if (pin == kFalsePin) {
      EXPECT_FALSE(maybe_output.value());
    } else {
      EXPECT_TRUE(maybe_output.value());
    }
  }
}

TEST(PinMapStm32Xml, GetPort) {
  EXPECT_EQ(GetPort(Pin::kPC0), Port::kC);
  EXPECT_EQ(GetPort(Pin::kPC1), Port::kC);
  EXPECT_EQ(GetPort(Pin::kPH1), Port::kH);
}

TEST(PinMapStm32Xml, GetPinIndex) {
  EXPECT_EQ(GetPinIndex(Pin::kPC0), 0);
  EXPECT_EQ(GetPinIndex(Pin::kPC1), 1);
  EXPECT_EQ(GetPinIndex(Pin::kPH1), 1);
}

}  // namespace
}  // namespace generators::pins
