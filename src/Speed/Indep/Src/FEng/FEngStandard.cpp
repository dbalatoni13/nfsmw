#include <string.h>

#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

int bStrICmp(const char* s1, const char* s2);

void FEngMemCpy(void* pDest, const void* pSrc, int Len) { memcpy(pDest, pSrc, Len); }

void FEngMemSet(void* pDest, int Value, int Len) { memset(pDest, Value, Len); }

void FEngStrCpy(char* pDest, const char* pSrc) { strcpy(pDest, pSrc); }

int FEngStrLen(const char* pSrc) { return strlen(pSrc); }

int FEngStrICmp(const char* pStr1, const char* pStr2) { return bStrICmp(pStr1, pStr2); }

static int FEngMemoryPoolNumber = -1;
static void* pFEngMemoryPoolMemory = nullptr;
static int FEngMemoryPoolSize = 0;
static int FEngMemoryPoolTracingEnabled = 0;

void InitFEngMemoryPool() {
    if (FEngMemoryPoolNumber != 0) {
        FEngMemoryPoolNumber = bGetFreeMemoryPoolNum();
        if (!pFEngMemoryPoolMemory) {
            pFEngMemoryPoolMemory = bMalloc(FEngMemoryPoolSize, __FILE__, 0, 0);
        }
        bInitMemoryPool(FEngMemoryPoolNumber, pFEngMemoryPoolMemory, FEngMemoryPoolSize, __FILE__);
        if (FEngMemoryPoolTracingEnabled) {
            MemoryPools[FEngMemoryPoolNumber]->DebugTracingEnabled = true;
        } else {
            void* dummy = bMalloc(0x10, __FILE__, 0, (FEngMemoryPoolNumber & 0xf) | 0x40);
            MemoryPools[FEngMemoryPoolNumber]->DebugTracingEnabled = false;
            bFree(dummy);
        }
    }
}

void* FEngMalloc(unsigned int size, const char* pFilename, int Line) {
    int pool_num = 0;
    if (FEngMemoryPoolNumber != -1) {
        int largest = bLargestMalloc(FEngMemoryPoolNumber);
        if (static_cast<int>(size) + 0x40 < largest) {
            pool_num = FEngMemoryPoolNumber;
        }
    }
    void* ptr = bMalloc(size, pFilename, Line, (pool_num & 0xf) | 0x400);
    return ptr;
}

float FEngAbs(float x) {
    return bAbs(x);
}

float FEngSqrt(float x) {
    return bSqrt(x);
}

float FEngSin(float x) {
    return bSin(x);
}

float FEngACos(float x) {
    return bAngToRad(bACos(x));
}
