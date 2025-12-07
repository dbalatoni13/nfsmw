#ifndef BWARE_BWARE_H
#define BWARE_BWARE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bMath.hpp"
#include "bSlotPool.hpp"

#if DEBUG
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params);
// this macro might not be a thing
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (debug_text), (debug_line), (allocation_params))
#else
void *bMalloc(int size, int allocation_params);
// this macro might not be a thing
#define bMALLOC(size, debug_text, debug_line, allocation_params) bMalloc((size), (allocation_params))
#endif

void *bMalloc(SlotPool *slot_pool);
void *bMalloc(SlotPool *slot_pool, int num_slots, void **last_slot);
void bFree(void *ptr);

extern "C" {
void bMemCpy(void *dest, const void *src, unsigned int numbytes);
void bMemSet(void *dest, unsigned char pattern, unsigned int size);
int bMemCmp(const void *s1, const void *s2, unsigned int numbytes);
}

inline void *operator new(size_t size, const char *file, int line) {
    return new char[size];
}

inline void *operator new[](size_t size, const char *file, int line) {
    return new char[size];
}

void bEndianSwap64(void *value);
void bEndianSwap32(void *value);
void bEndianSwap16(void *value);
void bPlatEndianSwap(bVector2 *value);
void bPlatEndianSwap(bVector3 *value);
void bPlatEndianSwap(bVector4 *value);
void bPlatEndianSwap(bMatrix4 *value);

void bInitSharedStringPool(int size);
void bCloseSharedStringPool();

inline void bPlatEndianSwap(int *value) {
    bEndianSwap32(value);
}

inline void bPlatEndianSwap(unsigned int *value) {
    bEndianSwap32(value);
}

inline void bPlatEndianSwap(short *value) {
    bEndianSwap16(value);
}

inline void bPlatEndianSwap(unsigned short *value) {
    bEndianSwap16(value);
}

inline void bPlatEndianSwap(unsigned char *value) {}

inline void bPlatEndianSwap(float *value) {
    bEndianSwap32(value);
}

inline bool bIsDigit(char c) {
    return c >= '0' && c <= '9';
}

extern BOOL bRumbleEnabled;

#endif
