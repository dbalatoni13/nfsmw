#ifndef ANIMATION_ANIMPART_H
#define ANIMATION_ANIMPART_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimSkeleton.hpp"

// total size: 0x14
class CAnimPart {
  public:
    CAnimSkeleton *GetSkeleton() {
        return m_pSkeleton;
    }

    float *GetSQTptr() {
        return m_pSQTs;
    }

    int GetSQTsize() {
        return m_SQTsize;
    }

    EAGL4::Transform *GetGlobalMatrices() {
        return m_pGlobalMatrices;
    }

    int GetNumGlobalMatrices() {
        return m_numGlobalMatrices;
    }

    CAnimPart();

    ~CAnimPart();

    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    int Init(CAnimSkeleton *skeleton);

    void Purge();

  private:
    CAnimSkeleton *m_pSkeleton;          // offset 0x0, size 0x4
    float *m_pSQTs;                      // offset 0x4, size 0x4
    int m_SQTsize;                       // offset 0x8, size 0x4
    EAGL4::Transform *m_pGlobalMatrices; // offset 0xC, size 0x4
    int m_numGlobalMatrices;             // offset 0x10, size 0x4
};

#endif
