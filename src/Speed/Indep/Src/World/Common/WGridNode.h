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

    inline const unsigned int GetElemTypeCount(WGridNode_ElemType type) const {
        return fElemCounts[type];
    }

    inline const unsigned int *GetElemTypePtr(WGridNode_ElemType type) const {
        const char *dataStart = reinterpret_cast<const char *>(this) + sizeof(WGridNode);
        return reinterpret_cast<const unsigned int *>(dataStart + fElemOffsets[type]);
    }

    inline unsigned int GetElemType(unsigned int index, WGridNode_ElemType type) const {
        return GetElemTypePtr(type)[index];
    }

    inline void AddDynamic(unsigned int ind, WGridNode_ElemType type) {
        if (fDynElems == nullptr) {
            fDynElems = new WGridNodeElemList();
        }
        WGridNodeElem elem(ind, type);
        fDynElems->push_back(elem);
    }

    inline void RemoveDynamic(unsigned int ind, WGridNode_ElemType type) {
        if (fDynElems != nullptr) {
            WGridNodeElemList::iterator eIter;
            for (eIter = fDynElems->begin(); eIter != fDynElems->end(); ++eIter) {
                if ((*eIter).fInd == ind && (*eIter).fType == type) {
                    fDynElems->erase(eIter);
                    return;
                }
            }
        }
    }

    WGridNodeElemList* fDynElems;       // offset 0x0, size 0x4
    unsigned short fNodeInd;            // offset 0x4, size 0x2
    unsigned short fPad;                // offset 0x6, size 0x2
    unsigned char fElemCounts[4];       // offset 0x8, size 0x4
    unsigned short fElemOffsets[4];     // offset 0xC, size 0x8
};

#endif
