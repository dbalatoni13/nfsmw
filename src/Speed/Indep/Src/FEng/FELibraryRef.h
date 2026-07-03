#ifndef FELIBRARYREF_H__
#define FELIBRARYREF_H__

#include "types.h"

// File: speed/indep/src/feng/FELibraryRef.h
// total size: 0xC
// Decl: speed/indep/src/feng/FELibraryRef.h:24
class FELibraryRef {
  public:
    u32 ObjGUID;      // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FELibraryRef.h:26
    u32 PackNameHash; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FELibraryRef.h:27
    u32 LibGUID;      // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FELibraryRef.h:28

    FELibraryRef() {} // Decl: speed/indep/src/feng/FELibraryRef.h:30
};

#endif
