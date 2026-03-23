#ifndef EAGL4ANIM_FNRAWLINEARCHANNEL_H
#define EAGL4ANIM_FNRAWLINEARCHANNEL_H

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnimMemoryMap.h"
#include "RawLinearChannel.h"

namespace EAGL4Anim {

// total size: 0x14
class FnRawLinearChannel : public FnAnimMemoryMap {
  public:
    FnRawLinearChannel() : mInterp(true) {
        mType = AnimTypeId::ANIM_RAWLINEAR;
    }

    // Overrides: FnAnimSuper
    ~FnRawLinearChannel() override {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    RawLinearChannel *GetRawLinearChannel() {
        return reinterpret_cast<RawLinearChannel *>(mpAnim);
    }

    const RawLinearChannel *GetRawLinearChannel() const {
        return reinterpret_cast<RawLinearChannel *>(mpAnim);
    }

    void SetInterp(bool state) {
        mInterp = state;
    }

    bool IsInterp() const {
        return mInterp;
    }

    // Overrides: FnAnim
    void Eval(float, float currentTime, float *output) override {
        GetRawLinearChannel()->Eval(currentTime, output, mInterp);
    }

    // Overrides: FnAnim
    bool GetLength(float &l) const override {
        l = static_cast<float>(GetRawLinearChannel()->GetNumFrames());
        return true;
    }

    int GetNumFrames() const {
        return GetRawLinearChannel()->GetNumFrames();
    }

    int GetNumDOFs() const {
        return GetRawLinearChannel()->GetNumDOFs();
    }

  private:
    bool mInterp; // offset 0x10, size 0x1
};

}; // namespace EAGL4Anim

#endif
