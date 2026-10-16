#pragma once

// The functions a logging backend must provide. log.cc is written against
// these; each target links an implementation selected by the
// `//fw/log:backend` flag.

#include <string_view>

namespace fw::log::backend {

// Writes `text` to the log output, blocking until it has been sent. Must not
// rely on interrupts, as it is called from the fatal path.
void Write(std::string_view text);

// Stops the system. Never returns.
[[noreturn]] void Halt();

}  // namespace fw::log::backend
