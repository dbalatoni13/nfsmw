#ifndef BWARE_HPP
#define BWARE_HPP

#include "Strings.hpp"
#include "bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "bMemory.hpp"
#include "bSlotPool.hpp"
#include "bTypes.hpp"

// TODO move these to the correct place
// #define PLAT_NEXT_GEN
#define NATIVE_ENDIAN_BIG
#define MEMORY_DUMP
//   #undef  NO_DEBUG_BMEMORY
//   #define   NULL 0
#define MUL 0
#define bPrintf (1) ? ((void)0) : bNullPrintf
#define bMilestonePutString bReleasePutString
#define bMilestonePrintf bReleasePrintf
#define bAssertMsg(exp, msg)
#define bAssertMsg1(exp, msg, arg1)
#define bAssertMsg2(exp, msg, arg1, arg2)
#define bAssertMsg3(exp, msg, arg1, arg2, arg3)
#define bAssertMsg4(exp, msg, arg1, arg2, arg3, arg4)
#define bWarning(exp)
#define bWarningMsg(exp, msg)
#define bValid(exp)
#define bFAssert(exp)
#define ASSERT_NOTRENDERTHREAD()
#define ASSERT_ISMAINTHREAD()
#define ASSERT_ISRENDERTHREAD()
// #define bMemCpy(dest, src, numbytes) memcpy(dest, src, numbytes)
// #define bMemSet(dest, pattern, size) memset(dest, pattern, size)
// #define bMemCmp(s1, s2, numbytes) memcmp(s1, s2, numbytes)
#define BMEMORY_TOP_BIT (1 << 6)
#define BMEMORY_MAX_POOLS 16
#define BMEMORY_POOL_MASK (BMEMORY_MAX_POOLS - 1)
#define BMEMORY_DEFAULT_POOL 0
#define BMEMORY_MAIN_POOL 0
#define BMEMORY_MAX_ALIGNMENT 4096
#define BMEMORY_POOL(n) (n & BMEMORY_POOL_MASK)
#define BMEMORY_ALLOCATE_FROM_TOP (1 << 6)
#define BMEMORY_BEST_FIT (1 << 7)
#define BMEMORY_ALIGNMENT(n) (((n >> 2) & 0x7ff) << 8)
#define BMEMORY_ALIGNMENT_OFFSET(n) (((n >> 2) & 0x7ff) << 19)
#define BMEMORY_DEFAULT_ALLOCATION_PARAMS 0
#define MEMTHROW
#define BDELETE delete
#define BNEW new (__FILE__, __LINE__)
#define BTEMPNEW new (__FILE__, __LINE__, BMEMORY_ALLOCATE_FROM_TOP)
#define USE_SLOTALLOC(POOL)                                                                                                                          \
    void *operator new(size_t size) {                                                                                                                \
        return bOMalloc(POOL);                                                                                                                       \
    }                                                                                                                                                \
    void *operator new(size_t size, const char *name) {                                                                                              \
        return bOMalloc(POOL);                                                                                                                       \
    }                                                                                                                                                \
    void operator delete(void *ptr) {                                                                                                                \
        bFree(POOL, ptr);                                                                                                                            \
    }
#define BEGIN_SOURCELIST(_MODULE)
#define END_SOURCELIST(_MODULE)
#define THIS_SCOPE_EXECUTES_ONLY_ONCE()

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

inline void *bMalloc(int size, int allocation_params) {
    return bMalloc(size, nullptr, 0, allocation_params);
}
#else

inline void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bMalloc(size, allocation_params);
}

#endif

void *bMalloc(SlotPool *slot_pool);
void *bMalloc(SlotPool *slot_pool, int num_slots, void **last_slot);
void bFree(void *ptr);
const char *bGetMallocName(void *ptr);
size_t bGetMallocSize(const void *ptr);
int bGetMallocPool(void *ptr);
int bMemoryGetAllocations(int pool_num, void **allocations, int max_allocations);

extern "C" {
void bMemCpy(void *dest, const void *src, unsigned int numbytes);
void bMemSet(void *dest, unsigned char pattern, unsigned int size);
int bMemCmp(const void *s1, const void *s2, unsigned int numbytes);
void bOverlappedMemCpy(void *dest, const void *src, unsigned int numbytes);
}

bool bSetMemoryPoolDebugFill(int pool_num, bool on_off);
void bSetMemoryPoolTopDirection(int pool_num, bool top_means_larger_address);

// TODO
inline void *Alloc(unsigned int bytes, int memtype, const char *name) {
    int parms;

    return bMalloc(bytes, name, 0, memtype);
}

#ifdef MILESTONE_OPT
void *operator new(size_t size, const char *file, int line);
#else
// TODO move the milestone path into a cpp file?
inline void *operator new(size_t size, const char *file, int line) {
#ifdef EA_BUILD_A124
    return bMalloc(size, 0);
#elif MILESTONE_OPT
    return bWareMalloc(size, file, line, 0);
#else
    return new char[size];
#endif
}
#endif

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
inline void bEndianSwap(short *value) {
    bEndianSwap16(value);
}

void bInitSharedStringPool(int size);
void bCloseSharedStringPool();

inline void bPlatEndianSwap(uint64 *value) {
#ifndef EA_BUILD_A124
    bEndianSwap64(value);
#endif
}

inline void bPlatEndianSwap(int32 *value) {
#ifndef EA_BUILD_A124
    bEndianSwap32(value);
#endif
}

inline void bPlatEndianSwap(uint32 *value) {
#ifndef EA_BUILD_A124
    bEndianSwap32(value);
#endif
}

inline void bPlatEndianSwap(int16 *value) {
#ifndef EA_BUILD_A124
    bEndianSwap16(value);
#endif
}

inline void bPlatEndianSwap(uint16 *value) {
#ifndef EA_BUILD_A124
    bEndianSwap16(value);
#endif
}

inline void bPlatEndianSwap(uint8 *value) {}

inline void bPlatEndianSwap(int8 *value) {}

inline void bPlatEndianSwap(float *value) {
#ifndef EA_BUILD_A124
    bEndianSwap32(value);
#endif
}

inline void bPlatEndianSwap(UCrc32 *c) {
    unsigned int val = c->GetValue();
    bPlatEndianSwap(&val);
    *c = UCrc32(val);
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
    return 0;
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

void bAssertFailMsg(char *fmt, const char *filename, int line_number, ...);

#define bAssert(exp) bAssertFailMsg(exp) // : 427

#endif
