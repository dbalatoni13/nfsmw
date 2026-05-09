#ifndef FENG_FELIBRARYREF_H
#define FENG_FELIBRARYREF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

class FELibraryRef { // 0xc
  public:
    /* 0x0 */ u32 ObjGUID;
    /* 0x4 */ u32 PackNameHash;
    /* 0x8 */ u32 LibGUID;
};

#endif
