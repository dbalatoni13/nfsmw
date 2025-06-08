
#pragma once

#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#if DEBUG
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params);
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (debug_text), (debug_line), (allocation_params))
#else
void *bMalloc(int size, int allocation_params);
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (allocation_params))
#endif

void *bMalloc(SlotPool *slot_pool, int num_slots, void **last_slot);
void bFree(void *ptr);
void bFree(SlotPool *slot_pool, void *p);
void bFree(SlotPool *slot_pool, void *first_slot, void *last_slot);

extern "C" {
void bMemSet(void *dest, unsigned char pattern, unsigned int size);
}
