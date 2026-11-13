#pragma once

namespace fw::os {

// RAII critical section guarantee.
class CriticalSection {
 public:
  CriticalSection();
  ~CriticalSection();
};

}  // namespace fw::os