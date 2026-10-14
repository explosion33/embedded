// Makes the FreeRTOS heap the only heap.
//
// C++ allocation (operator new/delete, and so std::vector etc.) is routed to
// pvPortMalloc/vPortFree, which are thread safe and report exhaustion through
// vApplicationMallocFailedHook.
//
// newlib's own malloc is disabled: its _sbrk halts. Out of the box newlib's
// heap has no lock (so is unsafe across tasks) and libnosys' _sbrk has no
// limit (so it silently grows into the interrupt stack). Anything that still
// calls malloc directly, or allocates inside the C library (e.g. printf to
// stdout, or float formatting), stops here instead of corrupting memory.

#include <cstddef>
#include <new>

#include "FreeRTOS.h"

namespace {

// Allocates `size` bytes of memory on the heap.
void* Allocate(std::size_t size) {
  // heap_4 treats a zero byte request as a failed allocation, but operator new
  // must return a unique pointer for one.
  if (size == 0) {
    size = 1;
  }
  // Never null: on exhaustion pvPortMalloc calls vApplicationMallocFailedHook,
  // which halts.
  return pvPortMalloc(size);
}

// Allocates `size` bytes with the provided `alignment`.
// TODO: This is a dumb wrapper that asserts for any invalid alignment, update
// if differing alignments are needed.
void* AllocateAligned(std::size_t size, std::align_val_t alignment) {
  configASSERT(static_cast<std::size_t>(alignment) <= portBYTE_ALIGNMENT);
  return Allocate(size);
}

}  // namespace

// Every other replaceable form (nothrow new, nothrow delete) forwards to these
// in libsupc++.

void* operator new(std::size_t size) { return Allocate(size); }

void* operator new[](std::size_t size) { return Allocate(size); }

void* operator new(std::size_t size, std::align_val_t alignment) {
  return AllocateAligned(size, alignment);
}

void* operator new[](std::size_t size, std::align_val_t alignment) {
  return AllocateAligned(size, alignment);
}

void operator delete(void* ptr) noexcept { vPortFree(ptr); }

void operator delete[](void* ptr) noexcept { vPortFree(ptr); }

void operator delete(void* ptr, std::size_t) noexcept { vPortFree(ptr); }

void operator delete[](void* ptr, std::size_t) noexcept { vPortFree(ptr); }

void operator delete(void* ptr, std::align_val_t) noexcept { vPortFree(ptr); }

void operator delete[](void* ptr, std::align_val_t) noexcept { vPortFree(ptr); }

void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept {
  vPortFree(ptr);
}

void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept {
  vPortFree(ptr);
}

extern "C" void* _sbrk(ptrdiff_t increment) {
  // Reaching this means something called newlib's malloc. Use new, or
  // pvPortMalloc, instead.
  configASSERT(false);
  return reinterpret_cast<void*>(-1);
}
