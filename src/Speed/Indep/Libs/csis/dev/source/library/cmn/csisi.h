//
//
#ifndef CSISI_H
#define CSISI_H 1

#include <Allocator/iallocator.h>
#include <csis/csis.h>
#include "../../extern/csisdef.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/slinklist.h"

struct OSMutex;

namespace Csis {

extern bool gIsAllocSet;
extern EA::Allocator::IAllocator *gpAllocator;
extern EA::Allocator::ICoreAllocator *gpCoreAllocator;
extern bool gIsAllocatorCreated;
extern bool gIsCoreAllocatorCreated;
extern bool gIsInited;
extern CListDStack gSystems;
extern OSMutex gMutexHandle;

struct ClassData {
    void SendParameters(Parameter *pParameters);

    void AddRefInline() {
        this->mRefCount++;
    }

    void DecRefInline() {
        this->mRefCount--;
        if (this->mRefCount == 0) {
            if (gpCoreAllocator != NULL) {
                gpCoreAllocator->Free(this, 0);
            } else {
                gpAllocator->Free(this, 0);
            }
        }
    }

private:
    friend class Class;

    ClassDesc *mpClassDesc;
    int mRefCount;
    CListDStack mUpdateClients;
    CListDStack mDestroyClients;
};

}

#endif
