#ifndef EAGL4ANIM_FNRAWPOSECHANNEL_H
#define EAGL4ANIM_FNRAWPOSECHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportdef.h"
#include "FnAnimMemoryMap.h"
#include "RawPoseChannel.h"

namespace EAGL4Anim {

// total size: 0x14
class FnRawPoseChannel : public FnAnimMemoryMap {
  public:
    FnRawPoseChannel() : mInterp(true) {
        mType = AnimTypeId::ANIM_RAWPOSE;
    }

    // Overrides: FnAnimSuper
    ~FnRawPoseChannel() override {}

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

    RawPoseChannel *GetRawPoseChannel() {
        return reinterpret_cast<RawPoseChannel *>(mpAnim);
    }

    const RawPoseChannel *GetRawPoseChannel() const {
        return reinterpret_cast<const RawPoseChannel *>(mpAnim);
    }

    void SetInterp(bool state) {
        mInterp = state;
    }

    bool IsInterp() const {
        return mInterp;
    }

    // Overrides: FnAnim
    bool GetLength(float &l) const override {
        l = static_cast<float>(GetRawPoseChannel()->GetNumFrames());
        return true;
    }

    int GetNumFrames() const {
        return GetRawPoseChannel()->GetNumFrames();
    }

    int GetNumBones() const {
        return GetRawPoseChannel()->GetNumBones();
    }

    // Overrides: FnAnim
    void Eval(float, float currentTime, float *outputPose) override;

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *outputPose, const BoneMask *boneMask) override;

  private:
    bool mInterp; // offset 0x10, size 0x1
};

}; // namespace EAGL4Anim

#endif
