#include <string.h>

#include "Speed/Indep/Src/FEng/FEngStandard.h"

int bStrICmp(const char* s1, const char* s2);

void FEngMemCpy(void* pDest, const void* pSrc, int Len) { memcpy(pDest, pSrc, Len); }

void FEngMemSet(void* pDest, int Value, int Len) { memset(pDest, Value, Len); }

void FEngStrCpy(char* pDest, const char* pSrc) { strcpy(pDest, pSrc); }

int FEngStrLen(const char* pSrc) { return strlen(pSrc); }

int FEngStrICmp(const char* pStr1, const char* pStr2) { return bStrICmp(pStr1, pStr2); }
