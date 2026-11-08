#include <cstdlib>
#include <iostream>
#include <string_view>

#include "fw/log/backend.h"

namespace fw::log::backend {

void Write(std::string_view text) { std::cerr << text << std::endl; }

[[noreturn]] void Halt() { std::abort(); }

}  // namespace fw::log::backend
