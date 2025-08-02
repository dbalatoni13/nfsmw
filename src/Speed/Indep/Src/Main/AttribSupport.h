#ifndef MAIN_ATTRIB_SUPPORT_H
#define MAIN_ATTRIB_SUPPORT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"

namespace Attrib {

const Collection *FindCollection(unsigned int classKey, unsigned int collectionKey);
const Collection *FindCollectionWithDefault(unsigned int classKey, unsigned int collectionKey);
unsigned int GetCollectionKey(const Collection *c);
const Collection *GetCollectionParent(const Collection *c);
unsigned int StringToLowerCaseKey(const char *str);
unsigned int AdjustHashTableSize(unsigned int requiredSize);

} // namespace Attrib

#endif
