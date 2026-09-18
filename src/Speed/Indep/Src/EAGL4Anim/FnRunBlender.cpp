#include "FnPoseBlender.h"
#include "FnRunBlender.h"
#include "ScratchBuffer.h"
#include "MemoryPoolManager.h"
#include "AnimTypeId.h"
#include "PhaseChan.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

#include <cmath>

static float length(float *v) {
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

namespace EAGL4Anim {

FnRunBlender::FnRunBlender()
    : mAnims(nullptr), mPhases(nullptr), mVels(nullptr), mWeight(0.0f), mNumAnims(0), mIdx(-100), mFreq(1.0f), mPrevTime(0.0f), mOffset(0.0f), mCycleIdx(-100), mInit(false), mSpeed(nullptr) {
    mFnAnims[0] = nullptr;
    mFnAnims[1] = nullptr;
    mFnVelAnims[0] = nullptr;
    mFnVelAnims[1] = nullptr;
    mType = AnimTypeId::ANIM_RUNBLENDER;
}

FnRunBlender::~FnRunBlender() {
    if (mFnAnims[0] != nullptr) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);
    }
    if (mFnAnims[1] != nullptr) {
        MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);
    }
    if (mFnVelAnims[0] != nullptr) {
        MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);
    }
    if (mFnVelAnims[1] != nullptr) {
        MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);
    }
    if (mNumAnims != 0) {
        ScratchBuffer::GetScratchBuffer(0).FreeBuffer();
    }
    if (mAnims != nullptr) {
        MemoryPoolManager::DeleteBlock(const_cast<AnimMemoryMap **>(mAnims));
    }
    if (mPhases != nullptr) {
        MemoryPoolManager::DeleteBlock(const_cast<PhaseChan **>(mPhases));
    }
    if (mVels != nullptr) {
        MemoryPoolManager::DeleteBlock(const_cast<AnimMemoryMap **>(mVels));
    }
    if (mSpeed != nullptr) {
        MemoryPoolManager::DeleteBlock(mSpeed);
    }
}

bool FnRunBlender::EvalSQT(float currTime, float *sqtBuffer, const BoneMask *boneMask) {

    mPrevTime = currTime;

    if (mFnAnims[0] == nullptr) {
        SetWeight(0.0f);
    }

    currTime = currTime + mOffset;

    float t0 = mFreq * mCycles[0] * currTime + mAlignFrame[0];
    float t1 = mFreq * mCycles[1] * currTime + mAlignFrame[1];
    int cIdx = ComputeCycleIdx(t0, 0.0f, mPhases[mIdx]->mNumFrames - 1);
    t0 = CycleTime(t0, 0.0f, mPhases[mIdx]->mNumFrames - 1);
    t1 = CycleTime(t1, 0.0f, mPhases[mIdx + 1]->mNumFrames - 1);

    mSkeleton->GetStillPose(sqtBuffer, nullptr);

    if (!mFnAnims[0]->EvalSQT(t0, sqtBuffer, nullptr)) {

        return false;
    }

    if (mWeight != 0.0f) {

        float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(0).GetBuffer());

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

void FnRunBlender::Eval(float, float currTime, float *pose) {
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

    mWeight = w - i;

    if (i == mIdx) {

        prevFreq = mFreq;
        mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];

        mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
    } else {

        if (i == mIdx + 1) {

            MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);

            mFnAnims[0] = mFnAnims[1];
            mFnAnims[1] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i + 1]));
        } else if (i == mIdx - 1) {

            MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);

            mFnAnims[1] = mFnAnims[0];
            mFnAnims[0] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i]));
        } else {

            if (mFnAnims[0] != nullptr) {
                MemoryPoolManager::DeleteFnAnim(mFnAnims[0]);
            }
            if (mFnAnims[1] != nullptr) {
                MemoryPoolManager::DeleteFnAnim(mFnAnims[1]);
            }

            mFnAnims[0] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i]));
            mFnAnims[1] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[i + 1]));
        }

        if (mVels) {

            if (i == mIdx + 1) {

                MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);

                mFnVelAnims[0] = mFnVelAnims[1];
                mFnVelAnims[1] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i + 1]));
            } else if (i == mIdx - 1) {

                MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);

                mFnVelAnims[1] = mFnVelAnims[0];
                mFnVelAnims[0] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i]));
            } else {

                if (mFnVelAnims[0] != nullptr) {
                    MemoryPoolManager::DeleteFnAnim(mFnVelAnims[0]);
                }
                if (mFnVelAnims[1] != nullptr) {
                    MemoryPoolManager::DeleteFnAnim(mFnVelAnims[1]);
                }

                mFnVelAnims[0] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i]));
                mFnVelAnims[1] = MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[i + 1]));
            }
        }

        if (mPhases[i]->StartWithRight()) {
            mAlignFrame[0] = mPhases[i]->mStartTime;
        } else {
            mAlignFrame[0] = mPhases[i]->mStartTime + mPhases[i]->mCycles[0];
        }
        mAlignFrame[1] = mPhases[i + 1]->mStartTime;
        if (!mPhases[i + 1]->StartWithRight()) {
            mAlignFrame[1] = mAlignFrame[1] + mPhases[i + 1]->mCycles[0];
        }

        mIdx = i;

        mCycles[0] = (mPhases[i]->mCycles[0] + mPhases[i]->mCycles[1]) * 0.5f;
        mCycles[1] = (mPhases[i + 1]->mCycles[0] + mPhases[i + 1]->mCycles[1]) * 0.5f;

        prevFreq = mFreq;
        mFreq = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];

        mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
    }
}

float FnRunBlender::CycleTime(float t, float startTime, float endTime) const {
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

int FnRunBlender::ComputeCycleIdx(float t, float startTime, float endTime) const {
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

bool FnRunBlender::EvalPhase(float, PhaseValue &) {
    return false;
}

bool FnRunBlender::EvalVel2D(float currTime, float *vel) {

    mPrevTime = currTime;

    if (mVels) {

        if (mFnVelAnims[0] == nullptr) {
            SetWeight(0.0f);
        }

        currTime = currTime + mOffset;

        float t0 = mFreq * mCycles[0] * currTime + mAlignFrame[0];
        float t1 = mFreq * mCycles[1] * currTime + mAlignFrame[1];
        int cIdx = ComputeCycleIdx(t0, 0.0f, mPhases[mIdx]->mNumFrames - 1);
        t0 = CycleTime(t0, 0.0f, mPhases[mIdx]->mNumFrames - 1);
        t1 = CycleTime(t1, 0.0f, mPhases[mIdx + 1]->mNumFrames - 1);

        if (!BlendVel(t0, t1, vel)) {
            return false;
        }

        AlignCycleBeginEnd(cIdx);
        AlignVel(vel);

        return true;
    }

    return false;
}

bool FnRunBlender::FindMatchTime(const MatchPhaseInput &input, float &time) const {

    PhaseValue phase;
    float a;
    float da;
    float na;
    int n;
    int s = 1;
    int i;
    int minIdx;
    float diffAngle;
    float minAngle;
    float angle;
    float dAngle;
    FnRunBlender *nonConstThis = const_cast<FnRunBlender *>(this);

    n = FloatToInt((mCycles[0] + mWeight * (mCycles[1] - mCycles[0])) * 2.0f * s);

    angle = input.mAngle;
    dAngle = input.mDAngle;

    /* r36e/r36f: 720/720 B y 8 filas, todas cascada de UNA: el objetivo carga
     * la constante magica de int->float ($LC326, 0x4330000080000000) en el
     * bloque de ANTES del `bso` --el mapa de lineas se la imputa a la linea 432
     * del original, o sea a la sentencia `n = FloatToInt(...)` de arriba-- y
     * nosotros dentro del `if`. Es un hundimiento entre bloques de una cantidad
     * que el compilador genera solo, sin variable C a la que atar la barrera.
     * Negativos de la r36f, encima de los seis de la r36e:
     *   `(float)n` en la comparacion ............... 97,361 % (igual)
     *   sin llaves ................................. 97,361 % (igual)
     *   `float sl = input.mSearchLength` local ..... 97,361 % (igual)
     *   `asm("" : "+f"(angle))` delante del `if` ... 97,361 % (igual)
     *   `angle`/`dAngle` DESPUES del `if` .......... 92,244 % / 43 filas / 724
     *   `asm("" : "+r"(n))` delante del `if` ....... 96,833 % / 27 filas
     *   `n > input.mSearchLength` .................. 97,278 % / 10 filas
     *   `0.0f < ... && (float)n > ...` ............. 97,278 % / 10 filas */
    if (input.mSearchLength > 0.0f && input.mSearchLength < n) {

        n = FloatToInt(input.mSearchLength) + 1;
    }

    nonConstThis->EvalPhase(0.0f, phase);
    a = phase.mAngle;
    minAngle = angle - a;

    if (minAngle < 0.0f) {
        minAngle = -minAngle;
    }
    minIdx = 0;

    for (i = 1; i < n; i++) {

        nonConstThis->EvalPhase(i, phase);
        na = phase.mAngle;

        if (a <= angle && angle <= na) {

            da = na - a;

            if (da * dAngle >= 0.0f) {

                if (da != 0.0f) {

                    time = ((angle - a) / da + (i - 1)) * s;

                } else {

                    time = ((i - 1) + 0.5f) * s;
                }

                return true;
            }
        }

        diffAngle = angle - phase.mAngle;

        if (diffAngle < 0.0f) {
            diffAngle = -diffAngle;
        }

        if (diffAngle < minAngle) {

            minAngle = diffAngle;
            minIdx = i;
        }

        a = na;
    }

    time = minIdx;

    return true;
}

void FnRunBlender::ComputeAlignQ(float *v1, float *v2, UMath::Vector4 &q) const {

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

void FnRunBlender::AlignCycleBeginEnd(int cIdx) {

    float v0[2];
    float v1[2];

    if (!mInit) {

        mCycleIdx = -1;
        mAlignQ.x = 0.0f;
        mAlignQ.y = 0.0f;
        mAlignQ.z = 0.0f;
        mAlignQ.w = 1.0f;
        mInit = true;
    } else if (mCycleIdx != cIdx) {

        UMath::Vector4 q;
        UMath::Vector4 resultQ;

        BlendFacing(0.0f, 0.0f, v0);

        BlendFacing(mPhases[mIdx]->mNumFrames - 1, mPhases[mIdx + 1]->mNumFrames - 1, v1);

        ComputeAlignQ(v0, v1, q);
        if (mCycleIdx - 1 == cIdx) {

            q.y = -q.y;
        }

        QuatMult(q, mAlignQ, resultQ);
        mAlignQ = resultQ;
        mCycleIdx = cIdx;
    }
}

void FnRunBlender::AlignRootQ(float *sqt) const {
    UMath::Vector4 q;

    QuatMult(mAlignQ, *reinterpret_cast<const UMath::Vector4 *>(&sqt[4]), q);

    *reinterpret_cast<UMath::Vector4 *>(&sqt[4]) = q;
}

void FnRunBlender::AlignVel(float *vel) const {
    UMath::Vector4 v, r;

    v.x = vel[0];
    v.y = 0.0f;
    v.z = vel[1];
    v.w = 1.0f;
    QuatRotate(mAlignQ, v, r);
    vel[0] = r.x;
    vel[1] = r.z;
}

bool FnRunBlender::BlendVel(float t0, float t1, float *vel) const {
    float v0[2], v1[2];
    if (!mFnVelAnims[0]->EvalVel2D(t0, v0)) {
        return false;
    }

    if (mWeight != 0.0f) {

        if (!mFnVelAnims[1]->EvalVel2D(t1, v1)) {

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

bool FnRunBlender::BlendFacing(float t0, float t1, float *f) const {

    float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(0).GetBuffer());
    UMath::Vector4 q0;
    UMath::Vector4 q1;
    UMath::Vector4 q;

    if (!mFnAnims[0]->EvalSQT(t0, buffer, nullptr)) {

        return false;
    }

    q0 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);

    if (mWeight != 0.0f) {

        mSkeleton->GetStillPose(buffer, nullptr);

        if (!mFnAnims[1]->EvalSQT(t1, buffer, nullptr)) {

            return false;
        }

        q1 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);

        FastQuatBlendF4(mWeight, &q0.x, &q1.x, &q.x);

    } else {

        q = q0;
    }

    UMath::Vector4 xAxis = {0.0f, 1.0f, 0.0f, 1.0f};
    UMath::Vector4 xAxis1;

    QuatTransformPoint(q, xAxis, xAxis1);

    f[0] = xAxis1.x;
    f[1] = xAxis1.z;

    return true;
}

float FnRunBlender::GetFrequency() const {
    return mFreq;
}

void FnRunBlender::ComputeRootQ(float t0, float t1, UMath::Vector4 &q) const {

    float *buffer = reinterpret_cast<float *>(ScratchBuffer::GetScratchBuffer(0).GetBuffer());
    UMath::Vector4 q0;
    UMath::Vector4 q1;

    if (!mFnAnims[0]->EvalSQT(t0, buffer, nullptr)) {

        return;
    }

    q0 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);

    if (mWeight != 0.0f) {

        mSkeleton->GetStillPose(buffer, nullptr);

        if (!mFnAnims[1]->EvalSQT(t1, buffer, nullptr)) {

            return;
        }

        q1 = *reinterpret_cast<UMath::Vector4 *>(&buffer[4]);

        FastQuatBlendF4(mWeight, &q0.x, &q1.x, &q.x);

    } else {

        q = q0;
    }
}

void FnRunBlender::ComputeBeginRootQ(UMath::Vector4 &q) const {
    ComputeRootQ(0.0f, 0.0f, q);
}

void FnRunBlender::ComputeEndRootQ(UMath::Vector4 &q) const {

    ComputeRootQ(static_cast<float>(mPhases[mIdx]->mNumFrames - 1), static_cast<float>(mPhases[mIdx + 1]->mNumFrames - 1), q);
}

}; // namespace EAGL4Anim
