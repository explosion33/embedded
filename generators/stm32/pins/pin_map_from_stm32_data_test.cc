#include <optional>

#include "generators/stm32/pins/sample_stm32_data_pin_map.h"
#include "gtest/gtest.h"
#include "magic_enum/magic_enum.hpp"

namespace generators::pins {
namespace {

TEST(PinMapStm32Data, GetPort) {
  EXPECT_EQ(GetPort(Pin::kPC0), Port::kC);
  EXPECT_EQ(GetPort(Pin::kPC1), Port::kC);
  EXPECT_EQ(GetPort(Pin::kPH1), Port::kH);
}

TEST(PinMapStm32Data, GetPinIndex) {
  EXPECT_EQ(GetPinIndex(Pin::kPC0), 0);
  EXPECT_EQ(GetPinIndex(Pin::kPC1), 1);
  EXPECT_EQ(GetPinIndex(Pin::kPH1), 1);
}

TEST(PinMapStm32Data, StablePortValue) {
  EXPECT_EQ(static_cast<size_t>(GetPort(Pin::kPC0)), 'C' - 'A');
  EXPECT_EQ(static_cast<size_t>(GetPort(Pin::kPH1)), 'H' - 'A');
}

}  // namespace
}  // namespace generators::pins
