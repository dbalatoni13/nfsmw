#ifndef EAGL4ANIM_FNPOSEBLENDER_H
#define EAGL4ANIM_FNPOSEBLENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"
#include "Skeleton.h"

namespace EAGL4Anim {

// total size: 0xA0
class FnPoseBlender : public FnAnim {
  public:
    FnPoseBlender()
        : mAlignRootBoneIdx(-1), //
          mpSkel(nullptr) {
        mPose[0] = mPose[1] = nullptr;
        mResetBuffers[0] = mResetBuffers[1] = false;
        mType = AnimTypeId::ANIM_POSEBLENDER;
    }

    // Overrides: FnAnimSuper
    ~FnPoseBlender() override;

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size);

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    float GetTimeOffset(int i) const {}

    FnAnim *GetAnim(int i) {}

    float GetStartTransTime() const {}

    float GetTransDuration() const {}

    float GetEndTransTime() const {}

    void SetResetBuffers(int i, bool state) {}

    bool GetResetBuffers(int i) const {}

    void Init(Skeleton *skel, float *poseBuffer0, float *poseBuffer1) {}

    static void BlendQ(int numBones, float w, const float *pose0, const float *pose1, float *result, const BoneMask *boneMask);

    static void BlendQ(int firstBone, int lastBone, float w, const float *pose0, const float *pose1, float *result);

    static void Blend(int numBones, float w, const float *pose0, const float *pose1, float *result, const BoneMask *boneMask);

    static void Blend(int firstBone, int lastBone, float w, const float *pose0, const float *pose1, float *result);

    static void BlendAll(int numBones, float w, const float *pose0, const float *pose1, float *result);

    static void BlendQ(int numBones, float w, const float *pose0, const BoneMask &mask0, const float *pose1, const BoneMask &mask1, float *result);

    static void Blend(int numBones, float w, const float *pose0, const BoneMask &mask0, const float *pose1, const BoneMask &mask1, float *result);

    void Set(FnAnim *anim0, float timeOffset0, FnAnim *anim1, float timeOffset1, int alignRootBoneIdx, float previousTime, float startTransTime,
             float duration);

    void Set(FnAnim *anim0, float timeOffset0, FnAnim *anim1, float timeOffset1, EAGL4::Transform &alignMat, int alignRootBoneIdx,
             float startTransTime, float duration);

    void Set(FnAnim *anim0, float timeOffset0, FnAnim *anim1, float timeOffset1, float startTransTime, float duration);

    // Overrides: FnAnim
    bool EvalSQT(float currentTime, float *sqtBuffer, const BoneMask *boneMask) override;

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *outputPose) override;

    static void XZProjectAlign(const EAGL4::Transform &from, const EAGL4::Transform &to, EAGL4::Transform &alignMat);

  private:
    static void GetMaxXZProjectAxis(const EAGL4::Transform &mat, UMath::Vector4 &axis, int &whichAxis) {}

    static void XZProjectAxis(const EAGL4::Transform &mat, UMath::Vector4 &axis, int whichAxis) {}

    void LinearBlendF3(float w, bool anim0, float *pose0, bool anim1, float *pose1, float *out) {}

    void QuatBlendF4(float w, bool anim0, float *pose0, bool anim1, float *pose1, float *out) {}

    Skeleton *mpSkel;              // offset 0xC, size 0x4
    float *mPose[2];               // offset 0x10, size 0x8
    FnAnim *mAnim[2];              // offset 0x18, size 0x8
    float mTimeOffset[2];          // offset 0x20, size 0x8
    int mAlignRootBoneIdx;         // offset 0x28, size 0x4
    EAGL4::Transform mAlignMatrix; // offset 0x40, size 0x40
    float mStartTransTime;         // offset 0x80, size 0x4
    float mDuration;               // offset 0x84, size 0x4
    float mEndTransTime;           // offset 0x88, size 0x4
    bool mResetBuffers[2];         // offset 0x8C, size 0x2
};

}; // namespace EAGL4Anim

#endif
