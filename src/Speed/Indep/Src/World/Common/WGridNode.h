#ifndef _WGRIDNODE_H_
#define _WGRIDNODE_H_

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

typedef uintptr_t WGridNodeElemTag;

enum WGridNode_ElemType {
    WGrid_kInstance = 0,
    WGrid_kTrigger = 1,
    WGrid_kObject = 2,
    WGrid_kRoadSegment = 3,
    WGrid_kElemTypeCount = 4,
};

// total size: 0x8
struct WGridNodeElem {
    WGridNodeElem() {}
    WGridNodeElem(WGridNodeElemTag ind, WGridNode_ElemType type)
        : fInd(ind), //
          fType(type) {}

    WGridNodeElemTag fInd;    // offset 0x0, size 0x4
    WGridNode_ElemType fType; // offset 0x4, size 0x4
};

struct WGridNodeElemList : public UTL::Std::list<WGridNodeElem, _type_list> {
    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }
};

// total size: 0x1C
struct WGridNode {
    unsigned int TotalSize() const;

    void ShutDown() {
        delete fDynElems;
        fDynElems = nullptr;
    }

    unsigned int GetNodeInd() const {
        return fNodeInd;
    }

    const unsigned int GetElemTypeCount(WGridNode_ElemType type) const {
        return fElemCounts[type];
    }

    const unsigned int *GetElemTypePtr(WGridNode_ElemType type) const {
        // TODO
        const char *dataStart = reinterpret_cast<const char *>(&this[1]) + fElemOffsets[type];
        return reinterpret_cast<const unsigned int *>(dataStart);
    }

    const unsigned int GetElemType(unsigned int ind, WGridNode_ElemType type) const {
        return GetElemTypePtr(type)[ind];
    }

    void AddDynamic(unsigned int ind, WGridNode_ElemType type) {
        if (fDynElems == nullptr) {
            fDynElems = new WGridNodeElemList();
        }
        WGridNodeElem elem(ind, type);
        fDynElems->push_back(elem);
    }

    void RemoveDynamic(uintptr_t ind, WGridNode_ElemType type) {
        if (fDynElems != nullptr) {
            for (WGridNodeElemList::iterator eIter = fDynElems->begin(); eIter != fDynElems->end(); ++eIter) {
                if ((*eIter).fInd == ind && (*eIter).fType == type) {
                    fDynElems->erase(eIter);
                    return;
                }
            }
        }
    }

    class iterator {
      public:
        iterator(const WGridNode *node, WGridNode_ElemType type)
            : fType(type),             //
              fNode(node),             //
              fNumEntriesRemaining(0), //
              fElemInd(nullptr),       //
              fValid(false),           //
              fDynamic(false) {
            fNumEntriesRemaining = fNode->GetElemTypeCount(type);
            if (fNumEntriesRemaining > 0) {
                fValid = true;
                fElemInd = fNode->GetElemTypePtr(type);
            }
            if (node->fDynElems != nullptr) {
                fIter = fNode->fDynElems->begin();
                fValid = true;
            }
        }

        void Invalidate() {
            fElemInd = nullptr;
            fValid = false;
        }

        const uintptr_t *GetIndPtr() {
            if (!fValid) {
                return nullptr;
            }
            const uintptr_t *retInd = nullptr;
            if (!fDynamic && fNumEntriesRemaining > 0) {
                fValid = true;
                retInd = fElemInd++;
                fNumEntriesRemaining--;
            } else if (fNode->fDynElems != nullptr) {
                fDynamic = true;
                while (fIter != fNode->fDynElems->end() && (*fIter).fType != fType) {
                    ++fIter;
                }
                if (fIter != fNode->fDynElems->end()) {
                    fElemInd = &(*fIter).fInd;
                    retInd = fElemInd;
                    ++fIter;
                } else {
                    Invalidate();
                }
            } else {
                Invalidate();
            }
            return retInd;
        }

      private:
        WGridNode_ElemType fType;          // offset 0x0, size 0x4
        const WGridNode *fNode;            // offset 0x4, size 0x4
        int fNumEntriesRemaining;          // offset 0x8, size 0x4
        const uintptr_t *fElemInd;         // offset 0xC, size 0x4
        bool fValid;                       // offset 0x10, size 0x1
        bool fDynamic;                     // offset 0x14, size 0x1
        WGridNodeElemList::iterator fIter; // offset 0x18, size 0x4
    };

    WGridNodeElemList *fDynElems;   // offset 0x0, size 0x4
    unsigned short fNodeInd;        // offset 0x4, size 0x2
    unsigned short fPad;            // offset 0x6, size 0x2
    unsigned char fElemCounts[4];   // offset 0x8, size 0x4
    unsigned short fElemOffsets[4]; // offset 0xC, size 0x8
};

#endif
