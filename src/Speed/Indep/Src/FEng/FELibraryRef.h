#ifndef FELIBRARYREF_H__
#define FELIBRARYREF_H__

#include "Speed/Indep/Src/FEng/FETypes.h"

// total size: 0xC
// Decl: 24
class FELibraryRef {
  public:
    u32 ObjGUID;      // offset 0x0, size 0x4, Decl: 26
    u32 PackNameHash; // offset 0x4, size 0x4, Decl: 27
    u32 LibGUID;      // offset 0x8, size 0x4, Decl: 28

    FELibraryRef() : ObjGUID(0), PackNameHash(-1), LibGUID(0) {} // Decl: 30
};

#endif
