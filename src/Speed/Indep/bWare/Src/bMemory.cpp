#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "types.h"

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params);

#if DEBUG
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
  return bWareMalloc(size, debug_text, debug_line, allocation_params);
}
#else
void *bMalloc(int size, int allocation_params) { return bWareMalloc(size, nullptr, 0, allocation_params); }
#endif

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params) { return nullptr; }
