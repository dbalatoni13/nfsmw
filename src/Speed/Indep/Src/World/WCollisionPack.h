#ifndef WORLD_WCOLLISIONPACK_H
#define WORLD_WCOLLISIONPACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

// total size: 0x18
class WCollisionPack {
  public:
    WCollisionPack(bChunk *chunk);
    ~WCollisionPack();

    const WCollisionInstance *Instance(unsigned short index) const;

    unsigned int InstanceCount() {
        return mInstanceNum;
    }

    const WCollisionObject *Object(unsigned short index) const;

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void *operator new(size_t size, const char *name) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

  private:
    void Init(bChunk *chunk);
    void DeInit();
    void Resolve(const UGroup *cGroup, unsigned int deltaAddress);

    unsigned int mSectionNumber;             // offset 0x0, size 0x4
    unsigned int mInstanceNum;               // offset 0x4, size 0x4
    const WCollisionInstance *mInstanceList; // offset 0x8, size 0x4
    unsigned int mObjectNum;                 // offset 0xC, size 0x4
    const WCollisionObject *mObjectList;     // offset 0x10, size 0x4
    bChunkCarpHeader *mCarpChunkHeader;      // offset 0x14, size 0x4
};

#endif
