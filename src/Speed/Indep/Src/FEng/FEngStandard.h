#ifndef FENGSTANDARD_H_
#define FENGSTANDARD_H_

#include <types.h>

#define FEngAssert(x) bAssert(x) // :40

void InitFEngMemoryPool();
void CloseFEngMemoryPool();
void *FEngMalloc(unsigned int size);
void *FEngMalloc(unsigned int size, const char *pFilename, int Line);
void FEngFree(void *ptr);
void FEngMemCpy(void *pDest, const void *pSrc, int Len);
void FEngMemSet(void *pDest, int Value, int Len);
void FEngMemMove(void *pDest, const void *pSrc, int Len);
void FEngStrCpy(char *pDest, const char *pSrc);
int FEngStrLen(const char *pSrc);
int FEngStrCmp(const char *pStr1, const char *pStr2);
int FEngStrICmp(const char *pStr1, const char *pStr2);
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

#ifdef MILESTONE_OPT
#define FNEW new (__FILE__, __LINE__, (DummyFEngNewType *)NULL) // :88
#else
#define FNEW new (0, 0, (DummyFEngNewType *)NULL)
#endif

#define FDELETE delete // :89

#endif
