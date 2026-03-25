#include "FnRunBlender.h"

#include "AnimUtil.h"
#include "FnPoseBlender.h"
#include "MemoryPoolManager.h"
#include "PhaseChan.h"
#include "ScratchBuffer.h"

#include <stdio.h>

float length(float *v) {
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}


namespace EAGL4Anim {

static const UMath::Vector4 kFacingAxis = {0.0f, 1.0f, 0.0f, 1.0f};

static inline void QuatTransformPoint(const UMath::Vector4 &q, const UMath::Vector4 &p, UMath::Vector4 &result) {
    float s;
    float xs;
    float ys;
    float zs;
    float wx;
    float wy;
    float wz;
    float xx;
    float xy;
    float xz;
    float yy;
    float yz;
    float zz;

    s = 2.0f;
    xs = q.x * s;
    ys = q.y * s;
    zs = q.z * s;
    wx = q.w * xs;
    wy = q.w * ys;
    wz = q.w * zs;
    xx = q.x * xs;
    xy = q.x * ys;
    xz = q.x * zs;
    yy = q.y * ys;
    yz = q.y * zs;
    zz = q.z * zs;
    result.x = p.x * (p.w - (yy + zz)) + p.y * (xy - wz) + p.z * (xz + wy);
    result.y = p.x * (xy + wz) + p.y * (p.w - (xx + zz)) + p.z * (yz - wx);
    result.z = p.x * (xz - wy) + p.y * (yz + wx) + p.z * (p.w - (xx + yy));
    result.w = p.w;
}

FnRunBlender::FnRunBlender()
    : mFreq(1.0f),       //
      mOffset(0.0f),     //
      mCycleIdx(-100) {
    mFnVelAnims[1] = nullptr;
    mType = AnimTypeId::ANIM_RUNBLENDER;
    mAnims = nullptr;
    mPhases = nullptr;
    mVels = nullptr;
    mWeight = 0.0f;
    mNumAnims = 0;
    mIdx = -100;
    mPrevTime = 0.0f;
    mInit = false;
    mSpeed = nullptr;
    mFnAnims[0] = nullptr;
    mFnAnims[1] = nullptr;
    mFnVelAnims[0] = nullptr;
}

FnRunBlender::~FnRunBlender() {
    if (mFnAnims[0]) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);
    }
    if (mFnAnims[1]) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);
    }
    if (mFnVelAnims[0]) {
        MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);
    }
    if (mFnVelAnims[1]) {
        MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);
    }
    if (mNumAnims) {
        ScratchBuffer::GetScratchBuffer(0).FreeBuffer();
    }
    if (mAnims) {
        MemoryPoolManager::DeleteBlock(mAnims);
    }
    if (mPhases) {
        MemoryPoolManager::DeleteBlock(const_cast<PhaseChan **>(mPhases));
    }
    if (mVels) {
        MemoryPoolManager::DeleteBlock(mVels);
    }
    if (mSpeed) {
        MemoryPoolManager::DeleteBlock(mSpeed);
    }
}

bool FnRunBlender::EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) {
    mPrevTime = currTime;

    if (!mFnAnims[0]) {
        SetWeight(0.0f);
    }

    currTime += mOffset;
    float evalTime0 = mFreq * mCycles[0] * currTime + mAlignFrame[0];
    float evalTime1 = mFreq * mCycles[1] * currTime + mAlignFrame[1];
    int cycleIdx = ComputeCycleIdx(evalTime0, 0.0f, static_cast<float>(mPhases[mIdx]->mNumFrames - 1));
    float t0 = CycleTime(evalTime0, 0.0f, static_cast<float>(mPhases[mIdx]->mNumFrames - 1));
    float t1 = CycleTime(evalTime1, 0.0f, static_cast<float>(mPhases[mIdx + 1]->mNumFrames - 1));

    mSkeleton->GetStillPose(sqtBuffer, 0);
    if (!mFnAnims[0] || !mFnAnims[0]->EvalSQT(t0, sqtBuffer, 0)) {
        return false;
    }

    if (mWeight != 0.0f && mFnAnims[1]) {
        float *blendPose = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(0).GetBuffer());

        mSkeleton->GetStillPose(blendPose, 0);
        if (!mFnAnims[1]->EvalSQT(t1, blendPose, 0)) {
            return false;
        }

        FnPoseBlender::Blend(mSkeleton->GetNumBones(), mWeight, sqtBuffer, blendPose, sqtBuffer, nullptr);
    }

    AlignCycleBeginEnd(cycleIdx);
    AlignRootQ(sqtBuffer);
    return true;
}

void FnRunBlender::Eval(float prevTime, float currTime, float *pose) {
    EvalSQT(currTime, pose, nullptr);
}

void FnRunBlender::SetWeight(float w) {
    int i = FloatToInt(w);
    float prevFreq;

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
        return;
    }

    if (i == mIdx + 1) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);
        mFnAnims[0] = mFnAnims[1];
        mFnAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i + 1])));
    } else if (i == mIdx - 1) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);
        mFnAnims[1] = mFnAnims[0];
        mFnAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i])));
    } else {
        if (mFnAnims[0]) {
            MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);
        }
        if (mFnAnims[1]) {
            MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);
        }

        mFnAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i])));
        mFnAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i + 1])));
    }

    if (mVels) {
        if (i == mIdx + 1) {
            MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);
            mFnVelAnims[0] = mFnVelAnims[1];
            mFnVelAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i + 1])));
        } else if (i == mIdx - 1) {
            MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);
            mFnVelAnims[1] = mFnVelAnims[0];
            mFnVelAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i])));
        } else {
            if (mFnVelAnims[0]) {
                MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);
            }
            if (mFnVelAnims[1]) {
                MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);
            }

            mFnVelAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i])));
            mFnVelAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i + 1])));
        }
    }

    if (mPhases[i]->StartWithRight()) {
        mAlignFrame[0] = static_cast<float>(mPhases[i]->mStartTime);
    } else {
        mAlignFrame[0] = static_cast<float>(mPhases[i]->mStartTime + mPhases[i]->mCycles[0]);
    }

    mAlignFrame[1] = static_cast<float>(mPhases[i + 1]->mStartTime);
    if (!mPhases[i + 1]->StartWithRight()) {
        mAlignFrame[1] += static_cast<float>(static_cast<int>(mPhases[i + 1]->mCycles[0]));
    }

    mIdx = i;
    prevFreq = mFreq;
    mCycles[0] = static_cast<float>(mPhases[i]->mCycles[0] + mPhases[i]->mCycles[1]) * 0.5f;
    mCycles[1] = static_cast<float>(mPhases[i + 1]->mCycles[0] + mPhases[i + 1]->mCycles[1]) * 0.5f;
    mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];
    mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
}

bool FnRunBlender::EvalPhase(float currTime, PhaseValue &phase) {
    return false;
}

bool FnRunBlender::EvalVel2D(float currTime, float *vel) {
    mPrevTime = currTime;
    if (!mVels) {
        return false;
    }
    if (!mFnVelAnims[0]) {
        SetWeight(0.0f);
    }

    float evalTime = currTime + mOffset;
    float cycleTime0 = mFreq * mCycles[0] * evalTime + mAlignFrame[0];
    int cycleIdx = ComputeCycleIdx(cycleTime0, 0.0f, static_cast<float>(mPhases[mIdx]->mNumFrames - 1));
    float t0 = CycleTime(cycleTime0, 0.0f, static_cast<float>(mPhases[mIdx]->mNumFrames - 1));
    float t1 = CycleTime(
        mFreq * mCycles[1] * evalTime + mAlignFrame[1],
        0.0f,
        static_cast<float>(mPhases[mIdx + 1]->mNumFrames - 1)
    );

    if (BlendVel(t0, t1, vel)) {
        AlignCycleBeginEnd(cycleIdx);
        AlignVel(vel);
        return true;
    }

    return false;
}

bool FnRunBlender::BlendVel(float t0, float t1, float *vel) const {
    float vel0[2];
    float vel1[2];

    if (!mFnVelAnims[0]->EvalVel2D(t0, vel0)) {
        return false;
    }
    if (mWeight != 0.0f) {
        if (!mFnVelAnims[1]->EvalVel2D(t1, vel1)) {
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

bool FnRunBlender::BlendFacing(float t0, float t1, float *f) const {
    if (!f) {
        return false;
    }

    float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(0).GetBuffer());
    UMath::Vector4 q0;
    UMath::Vector4 q;
    UMath::Vector4 xAxis;
    UMath::Vector4 xAxis1;

    if (!mFnAnims[0] || !mFnAnims[0]->EvalSQT(t0, buffer, 0)) {
        return false;
    }

    q0 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);

    if (mWeight != 0.0f) {
        mSkeleton->GetStillPose(buffer, 0);
        if (!mFnAnims[1] || !mFnAnims[1]->EvalSQT(t1, buffer, 0)) {
            return false;
        }

        UMath::Vector4 q1 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);
        FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));
    } else {
        q = q0;
    }

    xAxis = kFacingAxis;
    QuatTransformPoint(q, xAxis, xAxis1);
    f[0] = xAxis1.x;
    f[1] = xAxis1.z;
    printf("Facing: %g %g\n", f[0], f[1]);
    return true;
}

float FnRunBlender::GetFrequency() const {
    return mFreq;
}

void FnRunBlender::ComputeBeginRootQ(UMath::Vector4 &q) const {
    ComputeRootQ(0.0f, 0.0f, q);
}

void FnRunBlender::ComputeEndRootQ(UMath::Vector4 &q) const {
    float t0 = static_cast<float>(mPhases[mIdx]->mNumFrames - 1);
    float t1 = static_cast<float>(mPhases[mIdx + 1]->mNumFrames - 1);

    ComputeRootQ(t0, t1, q);
}

void FnRunBlender::ComputeRootQ(float t0, float t1, UMath::Vector4 &q) const {
    ScratchBuffer &scratch = ScratchBuffer::GetScratchBuffer(0);
    float *pose = reinterpret_cast<float *>(scratch.GetBuffer());

    if (!mFnAnims[0] || !mFnAnims[0]->EvalSQT(t0, pose, 0)) {
        return;
    }

    UMath::Vector4 q0 = *reinterpret_cast<UMath::Vector4 *>(&pose[4]);

    if (mWeight == 0.0f) {
        q = q0;
        return;
    }

    mSkeleton->GetStillPose(pose, 0);
    if (!mFnAnims[1] || !mFnAnims[1]->EvalSQT(t1, pose, 0)) {
        return;
    }

    UMath::Vector4 q1 = *reinterpret_cast<UMath::Vector4 *>(&pose[4]);
    FastQuatBlendF4(mWeight, reinterpret_cast<float *>(&q0), reinterpret_cast<float *>(&q1), reinterpret_cast<float *>(&q));
}

float FnRunBlender::CycleTime(float t, float startTime, float endTime) const {
    float len = endTime - startTime;
    float tmp;
    int n;

    if (t < startTime) {
        tmp = startTime - t;
        n = FloatToInt(tmp / len);
        tmp = tmp - static_cast<float>(n) * len;
        return endTime - tmp;
    }
    if (t < endTime) {
        return t;
    }
    tmp = t - endTime;
    n = FloatToInt(tmp / len);
    tmp = tmp - static_cast<float>(n) * len;
    return startTime + tmp;
}

int FnRunBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
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

void FnRunBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {
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

void FnRunBlender::AlignCycleBeginEnd(int cIdx) {
    if (!mInit) {
        mInit = true;
        mCycleIdx = -1;
        mAlignQ.z = 0.0f;
        mAlignQ.w = 1.0f;
        mAlignQ.x = 0.0f;
        mAlignQ.y = 0.0f;
    } else if (mCycleIdx != cIdx) {
        float v0[2];
        float v1[2];
        UMath::Vector4 q;
        UMath::Vector4 resultQ;

        BlendFacing(0.0f, 0.0f, v0);
        BlendFacing(static_cast<float>(const_cast<PhaseChan *>(mPhases[mIdx])->mNumFrames - 1),
                    static_cast<float>(const_cast<PhaseChan *>(mPhases[mIdx + 1])->mNumFrames - 1), v1);
        ComputeAlignQ(v0, v1, q);
        if (mCycleIdx - 1 == cIdx) {
            q.y = -q.y;
        }

        QuatMult(mAlignQ, q, resultQ);
        mAlignQ = resultQ;
        mCycleIdx = cIdx;
    }
}

void FnRunBlender::AlignRootQ(float *sqt) const {
    UMath::Vector4 result;
    QuatMult(*reinterpret_cast<UMath::Vector4 *>(&sqt[4]), mAlignQ, result);
    *reinterpret_cast<UMath::Vector4 *>(&sqt[4]) = result;
}

void FnRunBlender::AlignVel(float *vel) const {
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

bool FnRunBlender::FindMatchTime(const MatchPhaseInput &input, float &time) const {
    PhaseValue phase;
    float a;
    float da;
    float na;
    int n;
    int s;
    int i;
    int minIdx;
    float diffAngle;
    float minAngle;
    float angle = input.mAngle;
    float dAngle = input.mDAngle;
    FnRunBlender *nonConstThis = const_cast<FnRunBlender *>(this);

    n = FloatToInt((mWeight * (mCycles[1] - mCycles[0]) + mCycles[0]) * 2.0f);
    s = 1;

    phase.mAngle = 0.0f;
    if (0.0f < input.mSearchLength && input.mSearchLength < static_cast<float>(n)) {
        n = FloatToInt(input.mSearchLength) + s;
    }

    nonConstThis->EvalPhase(0.0f, phase);

    a = phase.mAngle;
    minAngle = angle - a;
    if (minAngle < 0.0f) {
        minAngle = -minAngle;
    }

    minIdx = 0;
    for (i = s; i < n; i++) {
        nonConstThis->EvalPhase(static_cast<float>(i), phase);
        na = phase.mAngle;

        if (a <= angle && angle <= na) {
            da = na - a;

            if (0.0f <= da * dAngle) {
                if (da != 0.0f) {
                    time = ((angle - a) / da + static_cast<float>(i - s)) * static_cast<float>(s);
                } else {
                    time = (static_cast<float>(i - s) + 0.5f) * static_cast<float>(s);
                }
                return true;
            }
        }

        diffAngle = angle - na;
        if (diffAngle < 0.0f) {
            diffAngle = -diffAngle;
        }
        if (diffAngle < minAngle) {
            minIdx = i;
            minAngle = diffAngle;
        }

        a = na;
    }

    time = static_cast<float>(minIdx);
    return true;
}

}; // namespace EAGL4Anim
