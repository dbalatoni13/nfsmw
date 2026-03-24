#ifndef BWARE_BWARE_H
#define BWARE_BWARE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Strings.hpp"
#include "bMath.hpp"
#include "bMemory.hpp"
#include "bSlotPool.hpp"

#ifdef DEBUG_OPT
#define ENABLE_IN_DEBUG true
#else
#define ENABLE_IN_DEBUG false
#endif

#ifdef MILESTONE_OPT
#define ENABLE_IN_MILESTONE true
#else
#define ENABLE_IN_MILESTONE false
#endif

void *bMalloc(int size, int allocation_params);
#ifdef MILESTONE_OPT
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params);
#else

inline void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bMalloc(size, allocation_params);
}

#endif

#ifdef EA_PLATFORM_PLAYSTATION2
inline void *bMalloc(unsigned int size, int allocation_params) {
    return bMalloc(static_cast<int>(size), allocation_params);
}
#endif

void *bMalloc(SlotPool *slot_pool);
void *bMalloc(SlotPool *slot_pool, int num_slots, void **last_slot);
void bFree(void *ptr);

extern "C" {
void bMemCpy(void *dest, const void *src, unsigned int numbytes);
void bMemSet(void *dest, unsigned char pattern, unsigned int size);
int bMemCmp(const void *s1, const void *s2, unsigned int numbytes);
void bOverlappedMemCpy(void *dest, const void *src, unsigned int numbytes);
}

inline void *operator new(size_t size, const char *file, int line) {
#if MILESTONE_OPT
    return bWareMalloc(size, file, line, 0);
#else
    return new char[size];
#endif
}

inline void *operator new[](size_t size, const char *file, int line) {
#if MILESTONE_OPT
    return bWareMalloc(size, file, line, 0);
#else
    return new char[size];
#endif
}

inline char *bGetPlatformName() {
#ifdef EA_PLATFORM_GAMECUBE
    return "GAMECUBE";
#elif defined(EA_PLATFORM_PLAYSTATION2)
    return "PSX2";
#elif defined(EA_PLATFORM_XENON)
    return "XENON";
#else
#error "Platform not specified";
#endif
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

inline void bPlatEndianSwap(signed char *value) {}

inline void bPlatEndianSwap(float *value) {
    bEndianSwap32(value);
}

inline bool bIsDigit(char c) {
    return c >= '0' && c <= '9';
}

inline bool bStrEqual(const char *s1, const char *s2) {
    return bStrICmp(s1, s2) == 0;
}

#ifdef MILESTONE_OPT
extern float bCodeineVersion;
#endif

inline int bIsCodeineConnected() {
#ifdef MILESTONE_OPT
    return bCodeineVersion > 0.0f;
#else
    return false;
#endif
}

inline int bIsBFunkAvailable() {
    return bIsCodeineConnected();
}

unsigned int bCalculateCrc32(const void *data, int size, unsigned int prev_crc32);

inline int bMemoryGetPoolNum(int allocation_params) {
    return allocation_params & 0xf;
}

inline int bMemoryGetAlignment(int allocation_params) {
    int alignment = allocation_params >> 6 & 0x1ffc;

    return alignment;
}

inline int bMemoryGetBestFit(int allocation_params) {
    return allocation_params & 0x80;
}

inline int bMemoryGetTopBit(int allocation_params) {
    return allocation_params & 0x40;
}

inline int bMemoryGetAlignmentOffset(int allocation_params) {
    return (allocation_params >> 17) & 0x1ffc;
}

#endif
