#ifndef FENG_FENGSTANDARD_H
#define FENG_FENGSTANDARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void* FEngMalloc(unsigned int size, const char* pFilename, int Line);
void FEngStrCpy(char* pDest, const char* pSrc);
int FEngStrLen(const char* pSrc);

#endif
