#pragma once

#include <array>
#include <atomic>
#include <climits>
#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>

#include "fw/log/check.h"

// PinMap currently emits a uint8_t this covers all possible pin amounts.
// TODO: This is fragile, pin_map should instead emit a
// magic_enum::customize::enum_range.
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum/magic_enum.hpp"
#undef MAGIC_ENUM_RANGE_MAX

namespace fw::platform {

// A thread-safe static global map of which pins are currently in use. Ensuring
// each pin is only used once.
template <typename PinEnum>
class PinLock {
 public:
  // RAII Lock wrapper around the PinLock.
  class Lock {
   public:
    Lock(PinEnum pin) : Lock({pin}) {};
    Lock(std::initializer_list<PinEnum> pins) {
      pins_.reserve(pins.size());
      for (PinEnum pin : pins) {
        AddPin(pin);
        pins_.push_back(pin);
      }
    }
    ~Lock() {
      for (PinEnum pin : pins_) {
        RemovePin(pin);
      }
    }

   private:
    // The pins this lock holds.
    std::vector<PinEnum> pins_;
  };

  PinLock(const PinLock&) = delete;
  PinLock& operator=(const PinLock&) = delete;

  // Adds the pin to the pin lock. Erroring if its in use.
  static void AddPin(PinEnum pin) {
    const auto idx = std::to_underlying(pin);
    // TODO: Repalce with GenerateBitMask utility.
    const uint32_t mask = 1u << (idx % kU32Bits);
    const uint32_t prev = GetPinLock().pins_[idx / kU32Bits].fetch_or(mask);
    CHECK(!(prev & mask)) << idx;
  }

  // Removes the pin from the pin lock, Erroring if it was not added.
  static void RemovePin(PinEnum pin) {
    const auto idx = std::to_underlying(pin);
    // TODO: Repalce with GenerateBitMask utility.
    const uint32_t mask = 1u << (idx % kU32Bits);
    const uint32_t prev = GetPinLock().pins_[idx / kU32Bits].fetch_and(~mask);
    CHECK(prev & mask) << idx;
  }

  static void Reset() {
    for (size_t i = 0; i < kNumWords; i++) {
      GetPinLock().pins_[i].store(0);
    }
  }

 private:
  static constexpr size_t kNumPins = magic_enum::enum_count<PinEnum>();
  static constexpr size_t kU32Bits = sizeof(uint32_t) * CHAR_BIT;
  static constexpr size_t kNumWords = (kNumPins + kU32Bits - 1) / kU32Bits;
  static_assert(std::atomic<uint32_t>::is_always_lock_free);

  PinLock() = default;

  static PinLock& GetPinLock() {
    static PinLock lock;
    return lock;
  }

  std::array<std::atomic<uint32_t>, kNumWords> pins_;
};

}  // namespace fw::platform