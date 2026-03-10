#ifndef ANIMATION_ANIMSKELETON_H
#define ANIMATION_ANIMSKELETON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/Skeleton.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x28
class CAnimSkeleton : public bTNode<CAnimSkeleton> {
  public:
    CAnimSkeleton();

    ~CAnimSkeleton();

    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    int Initialize(char *data, int size);

    void Cleanup();

    void PrintfSkeletonBoneData();

    void SetAssociatedChunk(bChunk *chunk) {
        mAssocChunk = chunk;
    }

    bChunk *GetAssociatedChunk() {
        return mAssocChunk;
    }

    const char *GetSkeletonName() {
        return m_pName;
    }

    unsigned int GetSkeletonNameHash() {
        return m_NameHash;
    }

    EAGL4Anim::Skeleton *GetEAGLSkeleton() {
        return m_pSkeleton;
    }

  private:
    void DynamicLoadResolve();

    int m_loaded;                       // offset 0x8, size 0x4
    bChunk *mAssocChunk;                // offset 0xC, size 0x4
    const char *m_pName;                // offset 0x10, size 0x4
    unsigned int m_NameHash;            // offset 0x14, size 0x4
    int m_internalDynLoader;            // offset 0x18, size 0x4
    EAGL4::DynamicLoader *m_pDynLoader; // offset 0x1C, size 0x4
    int m_DynLoaderSize;                // offset 0x20, size 0x4
    EAGL4Anim::Skeleton *m_pSkeleton;   // offset 0x24, size 0x4
};

CAnimSkeleton *GetSkeletonFromList(unsigned int namehash);

#endif
