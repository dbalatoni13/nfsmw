#include "FnRunBlender.h"

#include "FnPoseBlender.h"
#include "MemoryPoolManager.h"
#include "ScratchBuffer.h"


namespace EAGL4Anim {

FnRunBlender::FnRunBlender()
    : mAnims(nullptr),      //
      mPhases(nullptr),     //
      mVels(nullptr),       //
      mWeight(0.0f),        //
      mNumAnims(0),         //
      mIdx(-100),           //
      mSkeleton(nullptr),   //
      mFreq(1.0f),          //
      mPrevTime(0.0f),      //
      mOffset(0.0f),        //
      mCycleIdx(-100),      //
      mInit(false),         //
      mSpeed(nullptr) {
    mType = AnimTypeId::ANIM_RUNBLENDER;
    mFnAnims[0] = nullptr;
    mFnAnims[1] = nullptr;
    mFnVelAnims[0] = nullptr;
    mFnVelAnims[1] = nullptr;
    mAlignFrame[0] = 0.0f;
    mAlignFrame[1] = 0.0f;
    mCycles[0] = 0.0f;
    mCycles[1] = 0.0f;
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

    float evalTime = currTime + mOffset;
    float t0 = mFreq * mCycles[0] * evalTime + mAlignFrame[0];
    float t1 = mFreq * mCycles[1] * evalTime + mAlignFrame[1];

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

    return true;
}

void FnRunBlender::Eval(float prevTime, float currTime, float *pose) {
    EvalSQT(currTime, pose, nullptr);
}

void FnRunBlender::SetWeight(float w) {
    int idx = static_cast<int>(w);

    if (idx < 0) {
        idx = 0;
    }
    if (idx >= mNumAnims - 1) {
        idx = mNumAnims - 2;
    }

    mWeight = w - static_cast<float>(idx);

    if (idx == mIdx) {
        float denom = mWeight / mCycles[1] + (1.0f - mWeight) / mCycles[0];

        if (denom != 0.0f) {
            float prevFreq = mFreq;

            mFreq = denom;
            mOffset = (prevFreq / mFreq) * (mPrevTime + mOffset) - mPrevTime;
        }
        return;
    }

    if (idx >= 0 && idx < mNumAnims) {
        mFnAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[idx])));
        mFnAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mAnims[idx + 1])));

        if (mVels) {
            mFnVelAnims[0] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[idx])));
            mFnVelAnims[1] = reinterpret_cast<FnAnim *>(MemoryPoolManager::NewFnAnim(const_cast<AnimMemoryMap *>(mVels[idx + 1])));
        }

        mIdx = idx;
        mCycles[0] = 1.0f;
        mCycles[1] = 1.0f;
        mFreq = 1.0f;
        mOffset = 0.0f;
    }
}

}; // namespace EAGL4Anim
