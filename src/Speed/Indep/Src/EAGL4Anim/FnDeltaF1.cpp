#include "FnDeltaF1.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

FnDeltaF1::FnDeltaF1()
    : mNextKey(-1),         //
      mNextValues(nullptr), //
      mPrevKey(-1),         //
      mPrevVBlock(nullptr), //
      mPrevValues(nullptr), //
      mNextVBlock(nullptr) {
    mType = AnimTypeId::ANIM_DELTAF1;
}

void FnDeltaF1::Eval(float prevTime, float currTime, float *evalBuffer) {
    EvalSQT(currTime, evalBuffer, nullptr);
}

bool FnDeltaF1::EvalSQT(float currTime /* f31 */, float *sqt /* r25 */, const BoneMask *boneMask /* r30 */) {
    if (!mPrevValues) {
        InitBuffersAsRequired();
    }
    if (boneMask) {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    if (mBoneMask) {
        mBoneMask = nullptr;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaF1 *deltaF = reinterpret_cast<DeltaF1 *>(mpAnim); // r30
    int floorTime = FloatToInt(currTime);                  // r26
    int floorKey;                                          // r29
    if (!deltaF->mTimes) {
        if (floorTime >= 0) {
            floorKey = floorTime;
            if (floorTime >= deltaF->mNumFrames) {
                floorKey = deltaF->mNumFrames - 1;
            }
        } else {
            floorKey = 0;
        }
    } else if (floorTime < deltaF->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex = mPrevKey - 1; // r11
        if (mPrevKey <= 0) {
            timeIndex = 0;
        }
        if (deltaF->mTimes[timeIndex] <= floorTime) {
            // TODO
            for (; timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex] <= floorTime; timeIndex++) {
            }
        } else {
            for (; timeIndex > 0 && deltaF->mTimes[timeIndex] > floorTime; timeIndex--) {
            }
        }

        floorKey = timeIndex + 1;
    }
    unsigned int binLenPower = deltaF->GetBinLengthPower();
    unsigned int binLenModMask = deltaF->GetBinLengthModMask(); // r7 // TODO

    int floorBinIdx = floorKey >> binLenPower;    // r21
    int floorDeltaIdx = floorKey & binLenModMask; // r28
    // TODO
    int prevBinIdx; // r8 and r22?
    // TODO
    int prevDeltaIdx; // r10
    // TODO
    unsigned char *binData = deltaF->GetBin(floorBinIdx); // r12
    // TODO
    unsigned short *binPhys = deltaF->GetPhysical(binData);
    unsigned char *binDelta; // r6
    // TODO
    int frameSize = deltaF->GetFrameDeltaSize();
    bool preventReverse = false; // r0
    if (floorKey < mPrevKey) {
        preventReverse = !IsReverseDeltaSumEnabled();
    }
    if (floorKey == mNextKey) {
        float *tempBuf = mNextValues;
        prevDeltaIdx = floorDeltaIdx;
        mNextValues = mPrevValues;
        prevBinIdx = floorKey + 1;
        mPrevValues = tempBuf;
        mNextKey = -1;
        mPrevKey = floorKey;
    } else if (mPrevKey == -1 || floorBinIdx != (mPrevKey >> binLenPower) || floorDeltaIdx == 0 || preventReverse) {
        prevBinIdx = floorKey + 1;
        for (int idof = 0; idof < deltaF->GetNumBones(); idof++) {
            mPrevValues[idof] = deltaF->UnQuantizePhysical(mMinRangesf[idof], binPhys[idof]);
        }
        prevDeltaIdx = 0;
    } else {
        prevBinIdx = floorKey + 1;
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx);
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                mPrevValues[ibone] += deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
            }
            binDelta += frameSize;
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx - 1);
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                mPrevValues[ibone] -= deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
            }
            binDelta -= frameSize;
        }
    }
    mPrevKey = floorKey;

    // TODO
    int ceilKey = prevBinIdx;
    float scale = 1.0f; // f1
    bool lerpReqd;      // r7
    if (!deltaF->mTimes) {
        lerpReqd = currTime != floorTime;
        if (lerpReqd) {
            float ceilKeyTime = currTime - floorTime;
            scale = ceilKeyTime;
        }
    } else if (floorKey == 0) {
        // TODO ceilKeyTime here?
        lerpReqd = currTime != 0.0f;
        if (lerpReqd) {
            scale = currTime / static_cast<float>(deltaF->mTimes[0]);
        }
    } else {
        float floorKeyTime = deltaF->mTimes[floorKey - 1];
        lerpReqd = currTime != floorKeyTime;
        if (lerpReqd) {
            float ceilKeyTime = currTime - floorKeyTime;
            scale = ceilKeyTime / (static_cast<float>(deltaF->mTimes[floorKey]) - floorKeyTime);
        }
    }
    // TODO
    unsigned short *dofIndices = deltaF->GetDofIndices(); // r3

    if (lerpReqd && floorKey < deltaF->mNumFrames - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        // TODO
        float ceil;
        binData = deltaF->GetBin(ceilBinIdx);
        binPhys = deltaF->GetPhysical(binData);
        if (ceilKey == mNextKey) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++, dofIndices++) {
                sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
            }
        } else {
            if (ceilBinIdx != floorBinIdx) {
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++, dofIndices++) {
                    mNextValues[ibone] = deltaF->UnQuantizePhysical(mMinRangesf[ibone], binPhys[ibone]);
                    sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                }
            } else {
                int ceilDeltaIdx = floorDeltaIdx;
                binDelta = deltaF->GetDelta(binData, ceilDeltaIdx);
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++, dofIndices++) {
                    mNextValues[ibone] = deltaF->UnQuantizeDelta(mMinRangesf[ibone], binPhys[ibone]);
                    sqt[*dofIndices] = (mNextValues[ibone] - mPrevValues[ibone]) * scale + mPrevValues[ibone];
                }
            }
            mNextKey = ceilKey;
        }
    } else {
        for (int idof = 0; idof < deltaF->GetNumBones(); idof++) {
            sqt[dofIndices[idof]] = mPrevValues[idof];
        }
    }
    if (deltaF->mNumConstBones != 0) {
        unsigned short *constBoneIdxs = deltaF->GetConstBoneIdx(); // r6
        float *constPhys = deltaF->GetConstPhysical();             // r11

        for (int ibone = 0; ibone < deltaF->mNumConstBones; ibone++) {
            sqt[*constBoneIdxs] = *constPhys;

            constBoneIdxs++;
            constPhys++;
        }
    }

    return true;
}

bool FnDeltaF1::EvalWeights(float currTime, float *weights) {
    Eval(currTime, currTime, weights);
    return true;
}

bool FnDeltaF1::EvalVel2D(float currTime, float *vel) {
    Eval(currTime, currTime, vel);
    return true;
}

void FnDeltaF1::InitBuffersAsRequired() {
    DeltaF1 *deltaF = reinterpret_cast<DeltaF1 *>(mpAnim);

    if (deltaF->GetNumBones() != 0) {
        mPrevValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mPrevValues)));
        mPrevVBlock = mPrevValues;

        mNextValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mNextValues)));
        mNextVBlock = mNextValues;

        mMinRangesf = reinterpret_cast<DeltaF1MinRange *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mMinRangesf)));

        DeltaF1::DofInfo *dofInfo = deltaF->GetDofInfo();

        for (int b = 0; b < deltaF->GetNumBones(); b++) {
            mMinRangesf[b].mPhysMin = dofInfo->mPhysMin;

            mMinRangesf[b].mPhysRange = dofInfo->mPhysRange;
            // TODO magic
            mMinRangesf[b].mPhysRange *= 1.5259022e-5f;

            // TODO magic
            mMinRangesf[b].mDeltaMin = 2 * (dofInfo->mQuantMin * 1.5259022e-5f) * dofInfo->mPhysRange - dofInfo->mPhysRange;

            mMinRangesf[b].mDeltaRange = 2 * (dofInfo->mQuantRange * 1.5259022e-5f) * dofInfo->mPhysRange;
            mMinRangesf[b].mDeltaRange *= 0.003921569f;

            dofInfo++;
        }
    }
}

}; // namespace EAGL4Anim
