#ifndef WORLD_COMMON_WGRIDNODE_H
#define WORLD_COMMON_WGRIDNODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

struct WGridNodeElemList;

struct WGridNode {
    unsigned int TotalSize() const;

    WGridNodeElemList* fDynElems;       // offset 0x0, size 0x4
    unsigned short fNodeInd;            // offset 0x4, size 0x2
    unsigned short fPad;                // offset 0x6, size 0x2
    unsigned char fElemCounts[4];       // offset 0x8, size 0x4
    unsigned short fElemOffsets[4];     // offset 0xC, size 0x8
};

#endif
