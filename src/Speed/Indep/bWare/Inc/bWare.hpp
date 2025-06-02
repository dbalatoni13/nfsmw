#pragma once

#if DEBUG
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params);
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (debug_text), (debug_line), (allocation_params))
#else
void *bMalloc(int size, int allocation_params);
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (allocation_params))
#endif

extern "C" {
void bMemSet(void *dest, unsigned char pattern, unsigned int size);
}
