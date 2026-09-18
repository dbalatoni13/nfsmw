#include "ScratchBuffer.h"
#include "MemoryPoolManager.h"

#include "FnPoseBlender.h"
#include "FnRunBlender.h"
#include "FnTurnBlender.h"
#include "AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

#include <cstdio>

#include <cmath>

extern float length(float *v);

namespace EAGL4Anim {

FnTurnBlender::FnTurnBlender()
    : mAnims(nullptr), mWeight(0.0f), mNumAnims(0), mIdx(-100), mFreq(1.0f), mPrevTime(0.0f), mOffset(0.0f), mCycleIdx(-100), mInit(false) {
    mFnAnims[0] = nullptr;
    mFnAnims[1] = nullptr;
    mType = AnimTypeId::ANIM_TURNBLENDER;
}

FnTurnBlender::~FnTurnBlender() {
    if (mNumAnims != 0) {
        ScratchBuffer::GetScratchBuffer(1).FreeBuffer();
    }
    if (mAnims != nullptr) {
        MemoryPoolManager::DeleteBlock(mAnims);
    }
}

bool FnTurnBlender::EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) {

    mPrevTime = currTime;

    if (mFnAnims[0] == nullptr) {
        SetWeight(0.0f);
    }

    currTime = currTime + mOffset;
    int cIdx = ComputeCycleIdx(currTime, 0.0f, 2.0f / mFreq);

    float t0 = mFreq * mCycles[0] * currTime;
    float t1 = mFreq * mCycles[1] * currTime;

    t0 = CycleTime(t0, 0.0f, mCycles[0] + mCycles[0]) - mOffsets[0];
    t1 = CycleTime(t1, 0.0f, mCycles[1] + mCycles[1]) - mOffsets[1];

    mSkeleton->GetStillPose(sqtBuffer, nullptr);

    if (!mFnAnims[0]->EvalSQT(t0, sqtBuffer, nullptr)) {

        return false;
    }

    if (mWeight != 0.0f) {

        float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(1).GetBuffer());

        mSkeleton->GetStillPose(buffer, nullptr);

        if (!mFnAnims[1]->EvalSQT(t1, buffer, nullptr)) {

            return false;
        }

        FnPoseBlender::Blend(mSkeleton->GetNumBones(), mWeight, sqtBuffer, buffer, sqtBuffer, nullptr);
    }

    AlignCycleBeginEnd(cIdx);
    AlignRootQ(sqtBuffer);

    return true;
}

void FnTurnBlender::Eval(float, float currTime, float *pose) {
    EvalSQT(currTime, pose, nullptr);
}

void FnTurnBlender::SetWeight(float w) {

    float prevFreq;
    int i = FloatToInt(w);
    FnRunBlender *fnA;

    if (i < 0) {
        i = 0;
    }
    if (i >= mNumAnims - 1) {
        i = mNumAnims - 2;
    }

    mWeight = w - i;

    if (i == mIdx) {

        prevFreq = mFreq;
        mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];

        mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
    } else {

        if (i == mIdx + 1) {

            mFnAnims[0] = mFnAnims[1];
            mFnAnims[1] = mAnims[i + 1];
        } else if (i == mIdx - 1) {

            mFnAnims[1] = mFnAnims[0];
            mFnAnims[0] = mAnims[i];
        } else {

            mFnAnims[0] = mAnims[i];
            mFnAnims[1] = mAnims[i + 1];
        }

        mIdx = i;

        fnA = reinterpret_cast<FnRunBlender *>(mAnims[i]);
        mCycles[0] = 1.0f / fnA->GetFrequency();
        mOffsets[0] = fnA->GetOffset();
        fnA = reinterpret_cast<FnRunBlender *>(mAnims[mIdx + 1]);
        mCycles[1] = 1.0f / fnA->GetFrequency();
        mOffsets[1] = fnA->GetOffset();

        prevFreq = mFreq;
        mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];

        mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
    }
}

bool FnTurnBlender::EvalPhase(float, PhaseValue &) {
    return false;
}

bool FnTurnBlender::EvalVel2D(float currTime, float *vel) {

    mPrevTime = currTime;

    if (mFnAnims[0] == nullptr) {
        SetWeight(0.0f);
    }

    currTime = currTime + mOffset;
    int cIdx = ComputeCycleIdx(currTime, 0.0f, 2.0f / mFreq);

    printf("currTime: %g  offset: %g   cycle: %g\n", currTime, mOffset, 1.0f / mFreq);
    printf("offset0: %g  offset1: %g\n", mOffsets[0], mOffsets[1]);
    printf("cycle0: %g  cycle1: %g\n", mCycles[0], mCycles[1]);

    float t0 = mFreq * mCycles[0] * currTime;
    float t1 = mFreq * mCycles[1] * currTime;

    printf("before offset t0: %g  t1: %g\n", t0, t1);

    t0 = CycleTime(t0, 0.0f, mCycles[0] + mCycles[0]) - mOffsets[0];
    t1 = CycleTime(t1, 0.0f, mCycles[1] + mCycles[1]) - mOffsets[1];

    if (!BlendVel(t0, t1, vel)) {
        return false;
    }

    AlignCycleBeginEnd(cIdx);
    AlignVel(vel);

    return true;
}

bool FnTurnBlender::BlendVel(float t0, float t1, float *vel) const {
    float v0[2], v1[2];
    if (!mFnAnims[0]->EvalVel2D(t0, v0)) {
        return false;
    }

    if (mWeight != 0.0f) {

        if (!mFnAnims[1]->EvalVel2D(t1, v1)) {

            return false;
        }

        float len;
        float w = 1.0f - mWeight;
        vel[0] = mWeight * v1[0] + w * v0[0];
        vel[1] = mWeight * v1[1] + w * v0[1];
        len = length(vel);
        if (len != 0.0f) {

            len = (mWeight * length(v1) + w * length(v0)) / len;
            vel[0] *= len;
            vel[1] *= len;
        }
    } else {

        vel[0] = v0[0];
        vel[1] = v0[1];
    }

    return true;
}

int FnTurnBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
    float tmp;
    float len = endTime - startTime;

    if (t < startTime) {

        tmp = startTime - t;
        return FloatToInt(tmp / len);

    } else if (!(t < endTime)) {

        tmp = t - endTime;
        return FloatToInt(tmp / len) + 1;
    }

    return 0;
}

void FnTurnBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {

    float dot = v1[0] * v2[0] + v1[1] * v2[1];
    float len1 = sqrtf(v1[0] * v1[0] + v1[1] * v1[1]);
    float len2 = sqrtf(v2[0] * v2[0] + v2[1] * v2[1]);
    float cosA = dot / (len1 * len2);

    float t = (cosA + 1.0f) * 0.5f;
    q.x = 0.0f;
    q.y = sqrtf(1.0f - t);
    q.z = 0.0f;
    q.w = sqrtf(t);

    if (v1[0] * v2[1] - v1[1] * v2[0] > 0.0f) {
        q.y = -q.y;
    }
}

void FnTurnBlender::AlignCycleBeginEnd(int cIdx) {

    float v0[2];
    float v1[2];

    if (!mInit) {

        mCycleIdx = -1;
        mAlignQ.x = 0.0f; mAlignQ.y = 0.0f; mAlignQ.z = 0.0f; mAlignQ.w = 1.0f;
        mInit = true;
    } else if (mCycleIdx != cIdx) {

        UMath::Vector4 q;
        UMath::Vector4 resultQ;

        BlendBeginFacing(v0);

        BlendEndFacing(v1);

        ComputeAlignQ(v0, v1, q);
        if (mCycleIdx - 1 == cIdx) {

            q.y = -q.y;
        }

        QuatMult(q, mAlignQ, resultQ);
        mAlignQ = resultQ;
        mCycleIdx = cIdx;

        static int i = 0;

        printf("turn align[%d] Q: %g %g %g %g\n\n", i++, mAlignQ.x, mAlignQ.y, mAlignQ.z, mAlignQ.w);
    }
}

void FnTurnBlender::AlignRootQ(float *sqt) const {
    UMath::Vector4 q;

    QuatMult(mAlignQ, *reinterpret_cast<const UMath::Vector4 *>(&sqt[4]), q);

    *reinterpret_cast<UMath::Vector4 *>(&sqt[4]) = q;
}

void FnTurnBlender::AlignVel(float *vel) const {
    UMath::Vector4 v, r;

    v.x = vel[0];
    v.y = 0.0f;
    v.z = vel[1];
    v.w = 1.0f;
    QuatRotate(mAlignQ, v, r);
    vel[0] = r.x;
    vel[1] = r.z;
}

bool FnTurnBlender::BlendBeginFacing(float *f) const {

    FnRunBlender *fnA;
    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[0]);
    fnA->ComputeBeginRootQ(q0);

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[1]);
    fnA->ComputeBeginRootQ(q1);

    FastQuatBlendF4(mWeight, &q0.x, &q1.x, &q.x);

    UMath::Vector4 xAxis = {0.0f, 1.0f, 0.0f, 1.0f};
    UMath::Vector4 xAxis1;

    QuatRotate(q, xAxis, xAxis1);

    f[0] = xAxis1.x;
    f[1] = xAxis1.z;

    printf("Facing: %g %g\n", xAxis1.x, xAxis1.z);

    return true;
}

float FnTurnBlender::CycleTime(float t, float startTime, float endTime) const {
    float tmp;
    int n;
    float len = endTime - startTime;

    if (t < startTime) {

        tmp = startTime - t;
        n = FloatToInt(tmp / len);
        return endTime - (tmp - n * len);

    } else if (t >= endTime) {

        tmp = t - endTime;
        n = FloatToInt(tmp / len);
        return startTime + (tmp - n * len);
    }

    return t;
}

bool FnTurnBlender::BlendEndFacing(float *f) const {

    FnRunBlender *fnA;
    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[0]);
    fnA->ComputeEndRootQ(q0);

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[1]);
    fnA->ComputeEndRootQ(q1);

    FastQuatBlendF4(mWeight, &q0.x, &q1.x, &q.x);

    UMath::Vector4 xAxis = {0.0f, 1.0f, 0.0f, 1.0f};
    UMath::Vector4 xAxis1;

    QuatRotate(q, xAxis, xAxis1);

    f[0] = xAxis1.x;
    f[1] = xAxis1.z;

    printf("Facing: %g %g\n", xAxis1.x, xAxis1.z);

    return true;
}

}; // namespace EAGL4Anim
