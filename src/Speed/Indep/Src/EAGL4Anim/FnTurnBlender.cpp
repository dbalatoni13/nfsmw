#include "FnTurnBlender.h"

#include "AnimUtil.h"
#include "FnPoseBlender.h"
#include "FnRunBlender.h"
#include "ScratchBuffer.h"

#include <stdio.h>


namespace EAGL4Anim {

FnTurnBlender::FnTurnBlender()
    : mAnims(nullptr),     //
      mWeight(0.0f),       //
      mNumAnims(0),        //
      mIdx(-100),          //
      mSkeleton(nullptr),  //
      mFreq(1.0f),         //
      mPrevTime(0.0f),     //
      mOffset(0.0f),       //
      mCycleIdx(-100),     //
      mInit(false) {
    mType = AnimTypeId::ANIM_TURNBLENDER;
    mFnAnims[0] = nullptr;
    mFnAnims[1] = nullptr;
    mCycles[0] = 0.0f;
    mCycles[1] = 0.0f;
    mOffsets[0] = 0.0f;
    mOffsets[1] = 0.0f;
    mAlignQ.x = 0.0f;
    mAlignQ.y = 0.0f;
    mAlignQ.z = 0.0f;
    mAlignQ.w = 1.0f;
}

FnTurnBlender::~FnTurnBlender() {}

bool FnTurnBlender::EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) {
    mPrevTime = currTime;

    if (!mFnAnims[0]) {
        SetWeight(0.0f);
    }

    float evalTime = currTime + mOffset;
    float t0 = CycleTime(mFreq * mCycles[0] * evalTime + mOffsets[0], 0.0f, mCycles[0]);
    float t1 = CycleTime(mFreq * mCycles[1] * evalTime + mOffsets[1], 0.0f, mCycles[1]);

    mSkeleton->GetStillPose(sqtBuffer, 0);
    if (!mFnAnims[0] || !mFnAnims[0]->EvalSQT(t0, sqtBuffer, 0)) {
        return false;
    }

    if (mWeight != 0.0f && mFnAnims[1]) {
        ScratchBuffer &scratch = ScratchBuffer::GetScratchBuffer(0);
        unsigned int bufferSize = static_cast<unsigned int>(mSkeleton->GetNumBones() * 12 * sizeof(float));

        if (scratch.GetSize() < bufferSize) {
            scratch.AllocateBuffer(bufferSize);
        }

        float *blendPose = reinterpret_cast<float *>(scratch.GetBuffer());

        mSkeleton->GetStillPose(blendPose, 0);
        if (!mFnAnims[1]->EvalSQT(t1, blendPose, 0)) {
            return false;
        }

        FnPoseBlender::Blend(mSkeleton->GetNumBones(), mWeight, sqtBuffer, blendPose, sqtBuffer, boneMask);
    }

    AlignRootQ(sqtBuffer);
    return true;
}

void FnTurnBlender::Eval(float prevTime, float currTime, float *pose) {
    EvalSQT(currTime, pose, nullptr);
}

void FnTurnBlender::SetWeight(float w) {
    int idx = static_cast<int>(w);

    if (idx < 0) {
        idx = 0;
    }
    if (idx >= mNumAnims - 1) {
        idx = mNumAnims - 2;
    }

    mWeight = w - static_cast<float>(idx);

    if (idx >= 0 && idx < mNumAnims) {
        mFnAnims[0] = mAnims[idx];
        mFnAnims[1] = mAnims[idx + 1];
        mIdx = idx;
        mCycles[0] = 1.0f;
        mCycles[1] = 1.0f;
        mOffsets[0] = 0.0f;
        mOffsets[1] = 0.0f;
        mFreq = 1.0f;
        mOffset = 0.0f;
    }
}

bool FnTurnBlender::EvalPhase(float currTime, PhaseValue &phase) {
    return false;
}

bool FnTurnBlender::EvalVel2D(float currTime, float *vel) {
    if (!mFnAnims[0]) {
        SetWeight(0.0f);
    }

    float evalTime = currTime + mOffset;
    float t0 = CycleTime(mFreq * mCycles[0] * evalTime + mOffsets[0], 0.0f, mCycles[0]);
    float t1 = CycleTime(mFreq * mCycles[1] * evalTime + mOffsets[1], 0.0f, mCycles[1]);

    if (mWeight != 0.0f && mFnAnims[0] && mFnAnims[1]) {
        if (!BlendVel(t0, t1, vel)) {
            return false;
        }
    } else if (mFnAnims[0]) {
        if (!mFnAnims[0]->EvalVel2D(t0, vel)) {
            return false;
        }
    } else {
        return false;
    }

    AlignVel(vel);
    return true;
}

bool FnTurnBlender::BlendVel(float t0, float t1, float *vel) const {
    float vel0[2];
    float vel1[2];

    if (!mFnAnims[0] || !mFnAnims[1]) {
        return false;
    }
    if (!mFnAnims[0]->EvalVel2D(t0, vel0) || !mFnAnims[1]->EvalVel2D(t1, vel1)) {
        return false;
    }

    vel[0] = vel0[0] + mWeight * (vel1[0] - vel0[0]);
    vel[1] = vel0[1] + mWeight * (vel1[1] - vel0[1]);
    return true;
}

float FnTurnBlender::GetFrequency() const {
    return mFreq;
}

float FnTurnBlender::CycleTime(float t, float startTime, float endTime) const {
    float length = endTime - startTime;

    if (length <= 0.0f) {
        return startTime;
    }
    while (t < startTime) {
        t += length;
    }
    while (t > endTime) {
        t -= length;
    }
    return t;
}

int FnTurnBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
    float length = endTime - startTime;

    if (length <= 0.0f) {
        return 0;
    }

    return static_cast<int>((t - startTime) / length);
}

void FnTurnBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {
    q.x = 0.0f;
    q.y = 0.0f;
    q.z = 0.0f;
    q.w = 1.0f;
}

void FnTurnBlender::AlignCycleBeginEnd(int cIdx) {
    mCycleIdx = cIdx;
    mInit = true;
    mAlignQ.x = 0.0f;
    mAlignQ.y = 0.0f;
    mAlignQ.z = 0.0f;
    mAlignQ.w = 1.0f;
}

void FnTurnBlender::AlignRootQ(float *sqt) const {}

void FnTurnBlender::AlignVel(float *vel) const {}

bool FnTurnBlender::BlendBeginFacing(float *f) const {
    if (!f) {
        return false;
    }

    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;

    reinterpret_cast<const FnRunBlender *>(mFnAnims[0])->ComputeBeginRootQ(q0);
    reinterpret_cast<const FnRunBlender *>(mFnAnims[1])->ComputeBeginRootQ(q1);
    FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));

    f[0] = 2.0f * (q.w * q.z - q.x * q.y);
    f[1] = 2.0f * (q.y * q.z + q.x * q.w);
    printf("Facing: %g %g\n", f[0], f[1]);
    return true;
}

bool FnTurnBlender::BlendEndFacing(float *f) const {
    if (!f) {
        return false;
    }

    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;

    reinterpret_cast<const FnRunBlender *>(mFnAnims[0])->ComputeEndRootQ(q0);
    reinterpret_cast<const FnRunBlender *>(mFnAnims[1])->ComputeEndRootQ(q1);
    FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));

    f[0] = 2.0f * (q.w * q.z - q.x * q.y);
    f[1] = 2.0f * (q.y * q.z + q.x * q.w);
    printf("Facing: %g %g\n", f[0], f[1]);
    return true;
}

}; // namespace EAGL4Anim
