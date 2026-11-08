#include "fw/platform/common/pin_lock.h"

#include "gtest/gtest.h"

namespace fw::platform {
namespace {

enum class Pin {
  k0,
  k1,
  k2,
};

class PinLockTest : public ::testing::Test {
 protected:
  void SetUp() override { PinLock<Pin>::Reset(); }
};

TEST_F(PinLockTest, AddAndRemovePin) {
  for (Pin pin : magic_enum::enum_values<Pin>()) {
    PinLock<Pin>::AddPin(pin);
  }
  for (Pin pin : magic_enum::enum_values<Pin>()) {
    PinLock<Pin>::RemovePin(pin);
  }
}

TEST_F(PinLockTest, DoubleAdd) {
  PinLock<Pin>::AddPin(Pin::k2);
  EXPECT_DEATH(PinLock<Pin>::AddPin(Pin::k2);, ".*");
}

TEST_F(PinLockTest, RemoveBeforeAdd) {
  EXPECT_DEATH(PinLock<Pin>::RemovePin(Pin::k1);, ".*");

  PinLock<Pin>::AddPin(Pin::k2);
  PinLock<Pin>::RemovePin(Pin::k2);
  EXPECT_DEATH(PinLock<Pin>::RemovePin(Pin::k2);, ".*");
}

}  // namespace
}  // namespace fw::platform