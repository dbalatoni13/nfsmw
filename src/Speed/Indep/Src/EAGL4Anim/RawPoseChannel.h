#ifndef EAGL4ANIM_RAWPOSECHANNEL_H
#define EAGL4ANIM_RAWPOSECHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "BoneMask.h"

namespace EAGL4Anim {

// total size: 0x10
class RawPoseChannel : public AnimMemoryMap {
  public:
    enum ChannelType {
        QUAT = 0,
        EUL = 1,
        TRAN = 2,
    };

    int GetSigSize() const {
        return mSigSize;
    }

    void SetSigSize(int s) {}

    int GetFrameSize() const {
        return mFrameSize;
    }

    void SetFrameSize(int s) {}

    int GetNumFrames() const {
        return mNumFrames;
    }

    void SetNumFrames(int n) {}

    int *GetNonInterpSig() {
        return reinterpret_cast<int *>(&this[1]);
    }

    const int *GetNonInterpSig() const {
        return reinterpret_cast<const int *>(&this[1]);
    }

    int *GetInterpSig() {
        return GetNonInterpSig() + mSigSize;
    }

    const int *GetInterpSig() const {
        return GetNonInterpSig() + mSigSize;
    }

    float *GetAnimData() {
        return reinterpret_cast<float *>(GetInterpSig() + mSigSize);
    }

    const float *GetAnimData() const {
        return reinterpret_cast<const float *>(GetInterpSig() + mSigSize);
    }

    float *GetFrame(int i) {
        return &GetAnimData()[i * mFrameSize];
    }

    const float *GetFrame(int i) const {
        return &GetAnimData()[i * mFrameSize];
    }

    int GetSize() const {}

    static int ComputeSize(int sigSize, int poseSize, int numFrames) {}

    int GetNumBones() const {}

    void EvalInterpFrame(float t, int frame0, int frame1, float *outputPose, const BoneMask *boneMask) {}

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    void UnInit();

    void Eval(float frameTime, float *outputPose, bool interp, const BoneMask *boneMask);

    void EvalFrame(int frame, float *outputPose, const BoneMask *boneMask);

  private:
    int mSigSize;   // offset 0x4, size 0x4
    int mFrameSize; // offset 0x8, size 0x4
    int mNumFrames; // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
