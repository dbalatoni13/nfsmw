#ifndef FENG_FENGSTANDARD_H
#define FENG_FENGSTANDARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void* FEngMalloc(unsigned int size, const char* pFilename, int Line);
void FEngMemCpy(void* pDest, const void* pSrc, int Length);
void FEngMemSet(void* pDest, int Value, int Length);
void FEngStrCpy(char* pDest, const char* pSrc);
int FEngStrLen(const char* pSrc);
int FEngStrICmp(const char* pStr1, const char* pStr2);
void InitFEngMemoryPool();
float FEngAbs(float x);
float FEngSqrt(float x);
float FEngSin(float x);
float FEngACos(float x);

struct DummyFEngNewType {};

inline void* operator new(unsigned int size, const char* file, int line, DummyFEngNewType* dummy) {
    return FEngMalloc(size, file, line);
}

inline void* operator new[](unsigned int size, const char* file, int line, DummyFEngNewType* dummy) {
    return FEngMalloc(size, file, line);
}

#define FENG_NEW new(nullptr, 0, static_cast<DummyFEngNewType*>(nullptr))

#endif
