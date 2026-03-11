#ifndef WORLD_COMMON_WGRIDNODE_H
#define WORLD_COMMON_WGRIDNODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"

struct WGridNodeElemList : public std::list<WGridNodeElem, UTL::Std::Allocator<WGridNodeElem, _type_list> > {
    static void *operator new(unsigned int size) { return gFastMem.Alloc(size, nullptr); }
    static void operator delete(void *mem, unsigned int size) { gFastMem.Free(mem, size, nullptr); }
};

struct WGridNode {
    unsigned int TotalSize() const;

    void ShutDown() {
        if (fDynElems != nullptr) {
            delete fDynElems;
        }
        fDynElems = nullptr;
    }

    unsigned int GetNodeInd() const { return fNodeInd; }

    WGridNodeElemList* fDynElems;       // offset 0x0, size 0x4
    unsigned short fNodeInd;            // offset 0x4, size 0x2
    unsigned short fPad;                // offset 0x6, size 0x2
    unsigned char fElemCounts[4];       // offset 0x8, size 0x4
    unsigned short fElemOffsets[4];     // offset 0xC, size 0x8
};

#endif
