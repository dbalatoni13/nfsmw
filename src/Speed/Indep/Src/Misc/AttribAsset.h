//
#ifndef _AttribAsset_h_
#define _AttribAsset_h_

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"

bool AddDepFile(const char *filename, void *data, size_t bytes);
bool RemoveDepFile(const char *filename);
Attrib::Vault *AddVault(const char *filename, void *data, size_t bytes);
void RemoveVault(const char *filename);

#endif
