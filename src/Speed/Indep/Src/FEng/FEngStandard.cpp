#include <string.h>

#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "types.h"

int FEngMemoryPoolNumber = -1; // size: 0x4, address: 0x8041D140, Decl: speed/indep/src/feng/FEngStandard.cpp:23

void *pFEngMemoryPoolMemory = nullptr; // size: 0x4, address: 0x8041D144, Decl: speed/indep/src/feng/FEngStandard.cpp:26

int FEngMemoryPoolSize = 390000; // size: 0x4, address: 0x8041D148, Decl: speed/indep/src/feng/FEngStandard.cpp:31

int FEngMemoryPoolHighwaterWarning = 9999999; // size: 0x4, address: 0xFFFFFFFF, Decl: speed/indep/src/feng/FEngStandard.cpp:35

int FEngMemoryPoolTracingEnabled = 0; // size: 0x4, address: 0x8041D150, Decl: speed/indep/src/feng/FEngStandard.cpp:42

void InitFEngMemoryPool() {
    if (FEngMemoryPoolNumber != 0) {
        FEngMemoryPoolNumber = bGetFreeMemoryPoolNum();
        if (!pFEngMemoryPoolMemory) {
            pFEngMemoryPoolMemory = bMalloc(FEngMemoryPoolSize, "FEngMemoryPool", 0, 0);
        }
        bInitMemoryPool(FEngMemoryPoolNumber, pFEngMemoryPoolMemory, FEngMemoryPoolSize, "FEngMemoryPool");
        bSetMemoryPoolDebugTracing(FEngMemoryPoolNumber, FEngMemoryPoolTracingEnabled);
    }
}

// STRIPPED
void CloseFEngMemoryPool() {}

// STRIPPED
void *FEngMalloc(unsigned int size) {}

void *FEngMalloc(unsigned int size, const char *pFilename, int Line) {
    int pool_num = 0;
    if (FEngMemoryPoolNumber != -1) {
        int largest = bLargestMalloc(FEngMemoryPoolNumber);
        if (largest > static_cast<int>(size) + 0x40) {
            pool_num = FEngMemoryPoolNumber;
        }
    }
    void *ptr = bMalloc(size, pFilename, Line, (pool_num & 0xf) | 0x100);
    return ptr;
}

// STRIPPED
void FEngFree(void *pMemPtr) {}

void FEngMemCpy(void *pDest, const void *pSrc, int Len) {
    memcpy(pDest, pSrc, Len);
}

void FEngMemSet(void *pDest, int Value, int Len) {
    memset(pDest, Value, Len);
}

// STRIPPED
void FEngMemMove(void *pDest, const void *pSrc, int Len) {
    memmove(pDest, pSrc, Len);
}

void FEngStrCpy(char *pDest, const char *pSrc) {
    strcpy(pDest, pSrc);
}

int FEngStrLen(const char *pSrc) {
    return strlen(pSrc);
}

// STRIPPED
int FEngStrCmp(const char *pStr1, const char *pStr2) {
    return strcmp(pStr1, pStr2);
}

int FEngStrICmp(const char *pStr1, const char *pStr2) {
    return bStrICmp(pStr1, pStr2);
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
