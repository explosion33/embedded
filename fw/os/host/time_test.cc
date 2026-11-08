#include "fw/os/time.h"

#include "gtest/gtest.h"

namespace fw::os {
namespace {

TEST(TimeTest, TestDelayAndMillis) {
  constexpr size_t kMaxError = 1;

  for (size_t millis : {10, 100, 500, 1000}) {
    const size_t start = Millis();
    Delay(millis);

    const size_t elapsed = Millis() - start;
    const size_t diff = millis > elapsed ? millis - elapsed : elapsed - millis;
    EXPECT_LE(diff, kMaxError) << millis;
  }
}

}  // namespace
}  // namespace fw::os