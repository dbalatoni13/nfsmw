#ifndef REALCORE_SOURCE_INPUT_GC_PAD_H
#define REALCORE_SOURCE_INPUT_GC_PAD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "gc_device.h"
#include "gc_effect.h"

#include <dolphin/pad.h>

namespace RealInput {

struct GcPad : GcDevice {
    static inline void *operator new(unsigned int size) {
        return AllocateMemSize(nullptr, static_cast<int>(size), 0, 0, 0);
    }

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    Effect *GetEffect() {
        return this->mpEffect;
    }

    GcPad(GcInterface *pInterface, unsigned int port, volatile PADStatus *PStat);
    virtual ~GcPad();
    virtual RiResult Update();
    virtual Effect *CreateEffect(Effect::Info *pInfo);

    volatile PADStatus *mPadStat;
    unsigned int mResetBit;
    GcEffect *mpEffect;
    bool mInitialized;
};

} // namespace RealInput

#endif
