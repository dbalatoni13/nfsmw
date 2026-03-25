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
    : mFreq(1.0f),       //
      mOffset(0.0f),     //
      mCycleIdx(-100) {
    mFnAnims[1] = nullptr;
    mType = AnimTypeId::ANIM_TURNBLENDER;
    mAnims = nullptr;
    mWeight = 0.0f;
    mNumAnims = 0;
    mIdx = -100;
    mPrevTime = 0.0f;
    mInit = false;
    mFnAnims[0] = nullptr;
}

FnTurnBlender::~FnTurnBlender() {
    if (mNumAnims) {
        ScratchBuffer::GetScratchBuffer(1).FreeBuffer();
    }
    if (mAnims) {
        MemoryPoolManager::DeleteBlock(mAnims);
    }
}

bool FnTurnBlender::EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) {
    mPrevTime = currTime;

    if (!mFnAnims[0]) {
        SetWeight(0.0f);
    }

    currTime += mOffset;
    int cIdx = ComputeCycleIdx(currTime, 0.0f, 2.0f / mFreq);
    float t0 = mFreq * mCycles[0] * currTime;
    float t1 = mFreq * mCycles[1] * currTime;

    t0 = CycleTime(t0, 0.0f, mCycles[0] + mCycles[0]) - mOffsets[0];
    t1 = CycleTime(t1, 0.0f, mCycles[1] + mCycles[1]) - mOffsets[1];

    mSkeleton->GetStillPose(sqtBuffer, 0);
    if (!mFnAnims[0]->EvalSQT(t0, sqtBuffer, 0)) {
        return false;
    }

    if (mWeight != 0.0f) {
        float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(1).GetBuffer());

        mSkeleton->GetStillPose(buffer, 0);
        if (!mFnAnims[1]->EvalSQT(t1, buffer, 0)) {
            return false;
        }

        FnPoseBlender::Blend(mSkeleton->GetNumBones(), mWeight, sqtBuffer, buffer, sqtBuffer, nullptr);
    }

    AlignCycleBeginEnd(cIdx);
    AlignRootQ(sqtBuffer);
    return true;
}

void FnTurnBlender::Eval(float prevTime, float currTime, float *pose) {
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

    mWeight = w - static_cast<float>(i);
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
        fnA = static_cast<FnRunBlender *>(mAnims[i]);
        mCycles[0] = 1.0f / fnA->GetFrequency();
        mOffsets[0] = fnA->GetOffset();
        fnA = static_cast<FnRunBlender *>(mAnims[mIdx + 1]);
        mCycles[1] = 1.0f / fnA->GetFrequency();
        prevFreq = mFreq;
        mOffsets[1] = fnA->GetOffset();
        mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];
        mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
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

    if (!mFnAnims[0]->EvalVel2D(t0, vel0)) {
        return false;
    }
    if (mWeight != 0.0f) {
        if (!mFnAnims[1]->EvalVel2D(t1, vel1)) {
            return false;
        }

        float weight1 = 1.0f - mWeight;

        vel[0] = mWeight * vel1[0] + weight1 * vel0[0];
        vel[1] = mWeight * vel1[1] + weight1 * vel0[1];

        float velLength = length(vel);

        if (velLength != 0.0f) {
            float vel1Length = length(vel1);
            float vel0Length = length(vel0);
            float scale = (mWeight * vel1Length + weight1 * vel0Length) / velLength;

            vel[0] = vel[0] * scale;
            vel[1] = vel[1] * scale;
        }
    } else {
        vel[1] = vel0[1];
        vel[0] = vel0[0];
    }

    return true;
}

float FnTurnBlender::GetFrequency() const {
    return mFreq;
}

float FnTurnBlender::CycleTime(float t, float startTime, float endTime) const {
    float tmp;
    int n;
    float len = endTime - startTime;

    if (t < startTime) {
        tmp = startTime - t;
        n = FloatToInt(tmp / len);
        return endTime - (tmp - static_cast<float>(n) * len);
    }
    if (t < endTime) {
        return t;
    }
    tmp = t - endTime;
    n = FloatToInt(tmp / len);
    return startTime + (tmp - static_cast<float>(n) * len);
}

int FnTurnBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
    float length = endTime - startTime;

    if (t < startTime) {
        t = startTime - t;
        return static_cast<int>(t / length);
    }
    if (t >= endTime) {
        t -= endTime;
        return static_cast<int>(t / length) + 1;
    }
    return 0;
}

void FnTurnBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {
    float dot = v1[0] * v2[0] + v1[1] * v2[1];
    float v1Length = sqrtf(v1[0] * v1[0] + v1[1] * v1[1]);
    float v2Length = sqrtf(v2[0] * v2[0] + v2[1] * v2[1]);
    float norm = v1Length * v2Length;
    float w = ((dot / norm) + 1.0f) * 0.5f;

    q.x = 0.0f;
    q.y = sqrtf(1.0f - w);
    q.z = 0.0f;
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
        float v0[2];
        float v1[2];
        UMath::Vector4 q;
        UMath::Vector4 resultQ;
        int i;

        BlendBeginFacing(v0);
        BlendEndFacing(v1);
        ComputeAlignQ(v0, v1, q);
        if (mCycleIdx - 1 == cIdx) {
            q.y = -q.y;
        }

        QuatMult(mAlignQ, q, resultQ);
        i = i_6840++;
        mAlignQ = resultQ;
        mCycleIdx = cIdx;
        printf("turn align[%d] Q: %g %g %g %g\n\n", i, mAlignQ.x, mAlignQ.y, mAlignQ.z, mAlignQ.w);
    }
}

void FnTurnBlender::AlignRootQ(float *sqt) const {
    UMath::Vector4 result;
    QuatMult(*reinterpret_cast<UMath::Vector4 *>(&sqt[4]), mAlignQ, result);
    *reinterpret_cast<UMath::Vector4 *>(&sqt[4]) = result;
}

void FnTurnBlender::AlignVel(float *vel) const {
    UMath::Vector4 org;
    UMath::Vector4 result;

    org.x = vel[0];
    org.y = 0.0f;
    org.z = vel[1];
    org.w = 1.0f;
    QuatTransformPoint(mAlignQ, org, result);
    vel[0] = result.x;
    vel[1] = result.z;
}

bool FnTurnBlender::BlendBeginFacing(float *f) const {
    FnRunBlender *fnA;

    if (!f) {
        return false;
    }

    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;
    UMath::Vector4 xAxis;
    UMath::Vector4 xAxis1;

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[0]);
    fnA->ComputeBeginRootQ(q0);
    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[1]);
    fnA->ComputeBeginRootQ(q1);
    FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));
    xAxis = kFacingAxis;
    QuatTransformPoint(q, xAxis, xAxis1);
    f[0] = xAxis1.x;
    f[1] = xAxis1.z;
    printf("Facing: %g %g\n", f[0], f[1]);
    return true;
}

bool FnTurnBlender::BlendEndFacing(float *f) const {
    FnRunBlender *fnA;

    if (!f) {
        return false;
    }

    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;
    UMath::Vector4 xAxis;
    UMath::Vector4 xAxis1;

    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[0]);
    fnA->ComputeEndRootQ(q0);
    fnA = reinterpret_cast<FnRunBlender *>(mFnAnims[1]);
    fnA->ComputeEndRootQ(q1);
    FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));
    xAxis = kFacingAxis;
    QuatTransformPoint(q, xAxis, xAxis1);
    f[0] = xAxis1.x;
    f[1] = xAxis1.z;
    printf("Facing: %g %g\n", f[0], f[1]);
    return true;
}

}; // namespace EAGL4Anim
