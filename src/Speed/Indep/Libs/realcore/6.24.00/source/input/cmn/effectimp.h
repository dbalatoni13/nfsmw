#ifndef REALCORE_SOURCE_INPUT_CMN_EFFECTIMP_H
#define REALCORE_SOURCE_INPUT_CMN_EFFECTIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/input.h"

namespace RealInput {

struct EffectImp : Effect {
    EffectImp();
    virtual ~EffectImp();

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    virtual void Init(Device *pDevice, Info *pInfo);
    virtual void Release();
    virtual void Start();
    virtual void Stop();
    virtual Status GetStatus();
    virtual Device *GetDevice();
    virtual void GetInfo(Info *pInfo);
    virtual void SetInfo(Info *pInfo);

    inline bool IsInUse() {
        return this->mInUse;
    }

    inline void SetInUse(bool inUse) {
        this->mInUse = inUse;
    }

    Info mInfo;
    Device *mpDevice;
    Status mStatus;
    bool mInUse;
};

} // namespace RealInput

#endif
