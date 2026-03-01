#ifndef EAGL4ANIM_FNPOSEMIRROR_H
#define EAGL4ANIM_FNPOSEMIRROR_H

#include "BoneMask.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"
#include "Skeleton.h"
#include "eagl4supportdef.h"

namespace EAGL4Anim {

// total size: 0x20
class FnPoseMirror : public FnAnim {
  public:
    FnPoseMirror()
        : mMirror(true),       //
          mpAnim(nullptr),     //
          mpSkeleton(nullptr), //
          mMirrorLocal(false) {
        mType = AnimTypeId::ANIM_POSEMIRROR;
    }

    // Overrides: FnAnimSuper
    ~FnPoseMirror() override {}

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

    void Init(Skeleton *skel, float *poseBuffer, bool doGetStillPose) {}

    void Set(FnAnim *anim, bool mirrorLocal) {}

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *outputPose) override {
        if (mMirror) {
            mpAnim->Eval(previousTime, currentTime, mpPoseBuffer);
            mpSkeleton->MirrorPose(mpPoseBuffer, outputPose, mMirrorLocal, nullptr);
        } else {
            mpAnim->Eval(previousTime, currentTime, outputPose);
        }
    }

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *sqt, const BoneMask *boneMask) override {
        bool result;
        if (mMirror) {
            result = mpAnim->EvalSQT(currentTime, mpPoseBuffer, boneMask);
            if (result) {
                mpSkeleton->MirrorPose(mpPoseBuffer, sqt, mMirrorLocal, boneMask);
            }
        } else {
            result = mpAnim->EvalSQT(currentTime, sqt, boneMask);
        }
        return result;
    }

    void SetMirror(bool state) {}

    bool IsMirror() const {}

    FnAnim *GetAnim() {
        return mpAnim;
    }

    const FnAnim *GetAnim() const {
        return mpAnim;
    }

  private:
    FnAnim *mpAnim;       // offset 0xC, size 0x4
    Skeleton *mpSkeleton; // offset 0x10, size 0x4
    float *mpPoseBuffer;  // offset 0x14, size 0x4
    bool mMirrorLocal;    // offset 0x18, size 0x1
    bool mMirror;         // offset 0x1C, size 0x1
};

}; // namespace EAGL4Anim

#endif
