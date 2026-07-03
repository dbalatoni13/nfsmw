#ifndef FENGINTERFACEFESTRINGS_H
#define FENGINTERFACEFESTRINGS_H

#include "Speed/Indep/Src/FEng/FEString.h"

FEString *FEngFindString(const char *pkg_name /* r3 */, int name_hash /* r4 */);

void FEngSetLanguageHash(FEString *pkg_name, uint32 hash);

void FEngSetLanguageHash(const char *pkg_name, uint32 obj_hash, uint32 language);
unsigned int FEngHashString(const char *fmt /* r4 */, ...);
int FEngSNPrintf(char *buffer /* r3 */, int buf_size /* r4 */, const char *fmt /* r5 */, ...);

void FESetString(FEString *text /* r31 */, const short *string /* r4 */);

static int DoFEngPrintf(FEString *text, char *string, int len);

#endif
