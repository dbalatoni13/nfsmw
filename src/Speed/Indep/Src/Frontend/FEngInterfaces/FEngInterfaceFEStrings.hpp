#ifndef FRONTEND_FENGINTERFACES_FENGINTERFACEFESTRINGS_H
#define FRONTEND_FENGINTERFACES_FENGINTERFACEFESTRINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEString.h"

unsigned int FEngHashString(const char *fmt, ...);
FEString *FEngFindString(const char *pkg_name, int name_hash);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
void FESetString(FEString *text, const short *string);
int FEPrintf(FEString *text, const char *fmt, ...);
int FEPrintf(const char *pkg_name, FEObject *obj, const char *fmt, ...);
int FEngSNPrintf(char *buffer, int buf_size, const char *fmt, ...);

#endif
