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

#endif
