#ifndef REALCORE_SOURCE_INPUT_GC_EFFECT_H
#define REALCORE_SOURCE_INPUT_GC_EFFECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../cmn/effectimp.h"

namespace RealInput {

struct GcEffect : EffectImp {
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    static inline void *operator new[](unsigned int size) {
        return AllocateMemSize(nullptr, static_cast<int>(size), 0, 4, 0);
    }

    static inline void operator delete[](void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    GcEffect();
    virtual ~GcEffect();

    virtual void Init(Device *pDevice, Info *pInfo);
    virtual void Release();
    virtual void Start();
    virtual void Stop();
    virtual Status GetStatus();
    virtual void SetInfo(Info *pInfo);

    bool mPlaying;
    int mPort;
    int mUseFullStop;
};

} // namespace RealInput

#endif
