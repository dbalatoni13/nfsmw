#include "FnTurnBlender.h"

#include "AnimUtil.h"
#include "FnPoseBlender.h"
#include "FnRunBlender.h"
#include "ScratchBuffer.h"

#include <stdio.h>

float turnLength(float *v) {
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

static int i_6840;


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
    mPrevTime = currTime;
    if (!mFnAnims[0]) {
        SetWeight(0.0f);
    }

    float evalTime = currTime + mOffset;
    int cycleIdx = ComputeCycleIdx(evalTime, 0.0f, 2.0f / mFreq);

    printf("currTime: %g  offset: %g   cycle: %g\n", evalTime, mOffset, 1.0f / mFreq);
    printf("offset0: %g  offset1: %g\n", mOffsets[0], mOffsets[1]);
    printf("cycle0: %g  cycle1: %g\n", mCycles[0], mCycles[1]);

    float t0 = mFreq * mCycles[0] * evalTime;
    float t1 = mFreq * mCycles[1] * evalTime;

    printf("before offset t0: %g  t1: %g\n", t0, t1);
    if (BlendVel(CycleTime(t0, 0.0f, mCycles[0] + mCycles[0]) - mOffsets[0], CycleTime(t1, 0.0f, mCycles[1] + mCycles[1]) - mOffsets[1], vel)) {
        AlignCycleBeginEnd(cycleIdx);
        AlignVel(vel);
        return true;
    }

    return false;
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

    if (t < startTime) {
        return endTime - ((startTime - t) - static_cast<float>(static_cast<int>((startTime - t) / length)) * length);
    }
    if (t < endTime) {
        return t;
    }
    return startTime + ((t - endTime) - static_cast<float>(static_cast<int>((t - endTime) / length)) * length);
}

int FnTurnBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
    float length = endTime - startTime;

    if (length <= 0.0f) {
        return 0;
    }

    return static_cast<int>((t - startTime) / length);
}

void FnTurnBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {
    float dot = v1[0] * v2[0] + v1[1] * v2[1];
    float norm = turnLength(v1) * turnLength(v2);
    float w = ((dot / norm) + 1.0f) * 0.5f;

    q.x = 0.0f;
    q.z = 0.0f;
    q.y = sqrtf(1.0f - w);
    q.w = sqrtf(w);
    if (0.0f < v1[0] * v2[1] - v1[1] * v2[0]) {
        q.y = -q.y;
    }
}

void FnTurnBlender::AlignCycleBeginEnd(int cIdx) {
    if (!mInit) {
        mInit = true;
        mCycleIdx = -1;
        mAlignQ.x = 0.0f;
        mAlignQ.y = 0.0f;
        mAlignQ.z = 0.0f;
        mAlignQ.w = 1.0f;
    } else if (mCycleIdx != cIdx) {
        float beginFacing[2];
        float endFacing[2];
        UMath::Vector4 q;

        BlendBeginFacing(beginFacing);
        BlendEndFacing(endFacing);
        ComputeAlignQ(beginFacing, endFacing, q);
        if (mCycleIdx - 1 == cIdx) {
            q.y = -q.y;
        }

        float x = mAlignQ.x;
        float y = mAlignQ.y;
        float z = mAlignQ.z;
        float w = mAlignQ.w;
        float newX = (x * q.w - y * q.z) + z * q.y + w * q.x;
        float newY = ((x * q.z + y * q.w) - z * q.x) + w * q.y;
        float newZ = -x * q.y + y * q.x + z * q.w + w * q.z;
        float newW = ((-x * q.x - y * q.y) - z * q.z) + w * q.w;

        i_6840++;
        mAlignQ.x = newX;
        mAlignQ.y = newY;
        mAlignQ.z = newZ;
        mAlignQ.w = newW;
        mCycleIdx = cIdx;
        printf("turn align[%d] Q: %g %g %g %g\n\n", i_6840 - 1, mAlignQ.x, mAlignQ.y, mAlignQ.z, mAlignQ.w);
    }
}

void FnTurnBlender::AlignRootQ(float *sqt) const {
    float x = sqt[4];
    float y = sqt[5];
    float z = sqt[6];
    float w = sqt[7];
    float newX = w * mAlignQ.x + z * mAlignQ.y + (x * mAlignQ.w - y * mAlignQ.z);
    float newY = w * mAlignQ.y + (x * mAlignQ.z + y * mAlignQ.w) - z * mAlignQ.x;
    float newZ = w * mAlignQ.z + z * mAlignQ.w - x * mAlignQ.y + y * mAlignQ.x;
    float newW = w * mAlignQ.w - x * mAlignQ.x - y * mAlignQ.y - z * mAlignQ.z;

    sqt[4] = newX;
    sqt[5] = newY;
    sqt[6] = newZ;
    sqt[7] = newW;
}

void FnTurnBlender::AlignVel(float *vel) const {
    float x = mAlignQ.x;
    float y = mAlignQ.y;
    float z = mAlignQ.z;
    float w = mAlignQ.w;
    float doubleY = y + y;
    float doubleZ = z + z;
    float oldX = vel[0];
    float oldY = vel[1];
    float y2 = y * doubleY;
    float xz = x * doubleZ;
    float wy = w * doubleY;

    vel[0] = oldX * (1.0f - (z * doubleZ + y2)) + oldY * (xz + wy);
    vel[1] = oldX * (xz - wy) + oldY * (1.0f - (x * (x + x) + y2));
}

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
