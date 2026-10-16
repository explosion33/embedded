#include <cstdint>
#include <string_view>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/log/backend.h"

namespace fw::log::backend {

void Write(std::string_view text) {
  fw::board::Console().Write(reinterpret_cast<const uint8_t*>(text.data()),
                             text.size());
}

}  // namespace fw::log::backend
