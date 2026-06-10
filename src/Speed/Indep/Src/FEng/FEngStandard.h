#ifndef FENGSTANDARD_H_
#define FENGSTANDARD_H_

#include <types.h>

#define FEngAssert(x) bAssert(x) // :40

void *FEngMalloc(size_t size);
void *FEngMalloc(size_t size, const char *pFilename, int Line);
void FEngFree(void *ptr);
void FEngMemCpy(void *pDest, const void *pSrc, int Length);
void FEngMemSet(void *pDest, int Value, int Length);
void FEngStrCpy(char *pDest, const char *pSrc);
int FEngStrLen(const char *pSrc);
int FEngStrICmp(const char *pStr1, const char *pStr2);
void InitFEngMemoryPool();
float FEngAbs(float x);
float FEngSqrt(float x);
float FEngSin(float x);
float FEngACos(float x);

struct DummyFEngNewType {};

inline void *operator new(size_t size, const char *file, int line, DummyFEngNewType *dummy) {
    return FEngMalloc(size, file, line);
}

inline void *operator new[](size_t size, const char *file, int line, DummyFEngNewType *dummy) {
    return FEngMalloc(size, file, line);
}

#define FNEW new (__FILE__, __LINE__, (DummyFEngNewType *)NULL) // :88
#define FDELETE delete                                          // :89

#endif
