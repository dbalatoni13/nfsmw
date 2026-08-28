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
        delete this->fDynElems;
        this->fDynElems = nullptr;
    }

    unsigned int GetNodeInd() const {
        return this->fNodeInd;
    }

    const unsigned int GetElemTypeCount(WGridNode_ElemType type) const {
        return this->fElemCounts[type];
    }

    const unsigned int *GetElemTypePtr(WGridNode_ElemType type) const {
        // TODO
        const char *dataStart = reinterpret_cast<const char *>(&this[1]) + this->fElemOffsets[type];
        return reinterpret_cast<const unsigned int *>(dataStart);
    }

    const unsigned int GetElemType(unsigned int ind, WGridNode_ElemType type) const {
        return this->GetElemTypePtr(type)[ind];
    }

    void AddDynamic(unsigned int ind, WGridNode_ElemType type) {
        if (this->fDynElems == nullptr) {
            this->fDynElems = new WGridNodeElemList();
        }
        WGridNodeElem elem(ind, type);
        this->fDynElems->push_back(elem);
    }

    void RemoveDynamic(uintptr_t ind, WGridNode_ElemType type) {
        if (this->fDynElems != nullptr) {
            for (WGridNodeElemList::iterator eIter = this->fDynElems->begin(); eIter != this->fDynElems->end(); ++eIter) {
                if ((*eIter).fInd == ind && (*eIter).fType == type) {
                    this->fDynElems->erase(eIter);
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
            this->fNumEntriesRemaining = this->fNode->GetElemTypeCount(type);
            if (this->fNumEntriesRemaining > 0) {
                this->fValid = true;
                this->fElemInd = this->fNode->GetElemTypePtr(type);
            }
            if (node->fDynElems != nullptr) {
                this->fIter = this->fNode->fDynElems->begin();
                this->fValid = true;
            }
        }

        void Invalidate() {
            this->fElemInd = nullptr;
            this->fValid = false;
        }

        const uintptr_t *GetIndPtr() {
            if (!this->fValid) {
                return nullptr;
            }
            const uintptr_t *retInd = nullptr;
            if (!this->fDynamic && this->fNumEntriesRemaining > 0) {
                this->fValid = true;
                retInd = this->fElemInd++;
                this->fNumEntriesRemaining--;
            } else if (this->fNode->fDynElems != nullptr) {
                this->fDynamic = true;
                while (this->fIter != this->fNode->fDynElems->end() && (*this->fIter).fType != this->fType) {
                    ++this->fIter;
                }
                if (this->fIter != this->fNode->fDynElems->end()) {
                    this->fElemInd = &(*this->fIter).fInd;
                    retInd = this->fElemInd;
                    ++this->fIter;
                } else {
                    this->Invalidate();
                }
            } else {
                this->Invalidate();
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
