// Stubs for the C runtime hooks that -nostartfiles do not define.
extern "C" {

void _init() {}

void _fini() {}

}  // extern "C"
