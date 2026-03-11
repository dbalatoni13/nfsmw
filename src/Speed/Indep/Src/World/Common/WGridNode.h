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
    // total size: 0x1C
    struct iterator {
        WGridNode_ElemType fType;                        // offset 0x0, size 0x4
        const WGridNode *fNode;                          // offset 0x4, size 0x4
        int fNumEntriesRemaining;                        // offset 0x8, size 0x4
        const unsigned int *fElemInd;                    // offset 0xC, size 0x4
        bool fValid;                                     // offset 0x10, size 0x1
        bool fDynamic;                                   // offset 0x14, size 0x1
        WGridNodeElemList::iterator fIter;               // offset 0x18, size 0x4

        inline iterator(const WGridNode *node, WGridNode_ElemType type);
        inline void Invalidate();
        inline const unsigned int *GetIndPtr();
    };

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
        return reinterpret_cast<const unsigned int *>(fElemOffsets[type] + sizeof(WGridNode) + reinterpret_cast<int>(this));
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
            for (WGridNodeElemList::iterator eIter = fDynElems->begin(); eIter != fDynElems->end(); ++eIter) {
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

inline WGridNode::iterator::iterator(const WGridNode *node, WGridNode_ElemType type)
    : fType(type), //
      fNode(node), //
      fNumEntriesRemaining(0), //
      fElemInd(nullptr), //
      fValid(false), //
      fDynamic(false) {
    fNumEntriesRemaining = node->GetElemTypeCount(type);
    if (fNumEntriesRemaining > 0) {
        fElemInd = node->GetElemTypePtr(type);
        fValid = true;
    }
    if (node->fDynElems != nullptr) {
        fIter = node->fDynElems->begin();
        fValid = true;
    }
}

inline void WGridNode::iterator::Invalidate() {
    fElemInd = nullptr;
    fValid = false;
}

inline const unsigned int *WGridNode::iterator::GetIndPtr() {
    if (!fValid) {
        return nullptr;
    }
    const unsigned int *retInd = nullptr;
    if (!fDynamic && fNumEntriesRemaining > 0) {
        fNumEntriesRemaining--;
        fValid = true;
        retInd = fElemInd;
        fElemInd++;
    } else if (fNode->fDynElems == nullptr) {
        Invalidate();
    } else {
        fDynamic = true;
        while (fIter != fNode->fDynElems->end() && (*fIter).fType != fType) {
            ++fIter;
        }
        if (!(fIter != fNode->fDynElems->end())) {
            Invalidate();
        } else {
            retInd = &(*fIter).fInd;
            ++fIter;
            fElemInd = retInd;
        }
    }
    return retInd;
}

#endif
