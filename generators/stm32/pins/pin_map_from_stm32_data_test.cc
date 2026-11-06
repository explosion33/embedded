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

TEST(PinMapStm32Data, StableUartValue) {
  EXPECT_EQ(static_cast<size_t>(Uart::kUART4), 4);
  EXPECT_EQ(static_cast<size_t>(Uart::kUSART3), 3);
}

TEST(PinMapStm32Data, Usart3) {
  EXPECT_EQ(GetUartTxAf(Uart::kUSART3, Pin::kPC10), 7);
  EXPECT_EQ(GetUartTxAf(Uart::kUSART3, Pin::kPC11), std::nullopt);

  EXPECT_EQ(GetUartRxAf(Uart::kUSART3, Pin::kPC11), 7);
  EXPECT_EQ(GetUartRxAf(Uart::kUSART3, Pin::kPC10), std::nullopt);
}

TEST(PinMapStm32Data, Uart4) {
  EXPECT_EQ(GetUartTxAf(Uart::kUART4, Pin::kPC10), 8);
  EXPECT_EQ(GetUartTxAf(Uart::kUART4, Pin::kPC11), std::nullopt);

  EXPECT_EQ(GetUartRxAf(Uart::kUART4, Pin::kPC11), 8);
  EXPECT_EQ(GetUartRxAf(Uart::kUART4, Pin::kPC10), std::nullopt);
}

}  // namespace
}  // namespace generators::pins
