#pragma once

#include <string>

#include "fw/log/stream.h"

namespace fw::log {

// A Streaming interface that captures the location of a failure, as well as the
// asserted condition.
class FailureStream : public ::fw::log::Stream<FailureStream> {
 public:
  FailureStream(const char* file, int line, std::string_view cond) {
    *this << file << ":" << line << ": Expected: " << cond << " | ";
  }

  // Gets the captured text of the stream.
  std::string_view Text() const { return text_; }

 private:
  // Stream needs to acces the private Append().
  friend class ::fw::log::Stream<FailureStream>;

  // Appends captured text to the backing storage.
  void Append(std::string_view msg) { text_ += msg; };

  // The backing text storage.
  std::string text_;
};
}  // namespace fw::log