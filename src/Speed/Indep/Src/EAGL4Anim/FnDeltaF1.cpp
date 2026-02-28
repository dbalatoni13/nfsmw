#include "FnDeltaF1.h"
#include "AnimTypeId.h"
#include "DeltaF1.h"
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

bool FnDeltaF1::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!mPrevValues) {
        InitBuffersAsRequired();
    }
    if (boneMask) {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    if (mBoneMask) {
        mPrevKey = -1;
        mNextKey = -1;
        mBoneMask = nullptr;
    }

    DeltaF1 *deltaF = reinterpret_cast<DeltaF1 *>(mpAnim);
    int floorTime = FloatToInt(currTime);
    int floorKey;
    if (!deltaF->mTimes) {
        if (floorTime < 0) {
            floorKey = 0;
        } else {
            if (floorTime >= deltaF->mNumFrames) {
                floorKey = deltaF->mNumFrames - 1;
            } else {
                floorKey = floorTime;
            }
        }
    } else if (floorTime < deltaF->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex;
        if (mPrevKey < 1) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaF->mTimes[timeIndex] <= floorTime) {
            while (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                timeIndex++;
            }
        } else {
            while (timeIndex > 0 && deltaF->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }
    unsigned int binLenPower = deltaF->GetBinLengthPower();
    unsigned int binLenModMask = deltaF->GetBinLengthModMask();

    int floorBinIdx = floorKey >> binLenPower;
    int floorDeltaIdx = floorKey & binLenModMask;
    int prevBinIdx = mPrevKey >> binLenPower;
    int prevDeltaIdx;
    unsigned char *binData = deltaF->GetBin(floorBinIdx);
    unsigned short *binPhys = deltaF->GetPhysical(binData);
    unsigned char *binDelta;
    int frameSize = deltaF->GetFrameDeltaSize();

    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();
    if (floorKey == mNextKey) {
        float *tempBuf = mPrevValues;
        mPrevValues = mNextValues;
        mNextValues = tempBuf;
        prevDeltaIdx = floorDeltaIdx;
        mPrevKey = mNextKey;
        mNextKey = -1;
    } else if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int idof = 0; idof < deltaF->GetNumBones(); idof++) {
            mPrevValues[idof] = deltaF->UnQuantizePhysical(mMinRangesf[idof], binPhys[idof]);
        }
        prevDeltaIdx = 0;
    } else {
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

    int ceilKey = floorKey + 1;
    float scale = 1.0f;
    bool lerpReqd;

    if (!deltaF->mTimes) {
        lerpReqd = currTime != floorTime;
        if (lerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (floorKey == 0) {
        lerpReqd = currTime != 0.0f;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        float floorKeyTime = deltaF->mTimes[floorKey - 1];
        lerpReqd = currTime != floorKeyTime;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    unsigned short *dofIndices = deltaF->GetDofIndices();

    if (lerpReqd && floorKey < deltaF->mNumFrames - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        float ceil;
        binData = deltaF->GetBin(ceilBinIdx);
        binPhys = deltaF->GetPhysical(binData);
        if (ceilKey == mNextKey) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                dofIndices++;
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
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                    ceil = deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
                    mNextValues[ibone] = mPrevValues[ibone] + ceil;
                    sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                    dofIndices++;
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
        unsigned short *constBoneIdxs = deltaF->GetConstBoneIdx();
        float *constPhys = deltaF->GetConstPhysical();

        for (int ibone = 0; ibone < deltaF->mNumConstBones; ibone++, constPhys++, constBoneIdxs++) {
            sqt[*constBoneIdxs] = *constPhys;
        }
    }

    return true;
}

bool FnDeltaF1::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mPrevKey = -1;
        mNextKey = -1;
        mBoneMask = boneMask;
    }

    DeltaF1 *deltaF = reinterpret_cast<DeltaF1 *>(mpAnim);
    int floorTime = FloatToInt(currTime);
    int floorKey;
    if (!deltaF->mTimes) {
        if (floorTime < 0) {
            floorKey = 0;
        } else {
            if (floorTime >= deltaF->mNumFrames) {
                floorKey = deltaF->mNumFrames - 1;
            } else {
                floorKey = floorTime;
            }
        }
    } else if (floorTime < deltaF->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex;
        if (mPrevKey < 1) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaF->mTimes[timeIndex] <= floorTime) {
            while (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                timeIndex++;
            }
        } else {
            while (timeIndex > 0 && deltaF->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }
    unsigned int binLenPower = deltaF->GetBinLengthPower();
    unsigned int binLenModMask = deltaF->GetBinLengthModMask();

    int floorBinIdx = floorKey >> binLenPower;
    int floorDeltaIdx = floorKey & binLenModMask;
    int prevBinIdx = mPrevKey >> binLenPower;
    int prevDeltaIdx;
    unsigned char *binData = deltaF->GetBin(floorBinIdx);
    unsigned short *binPhys = deltaF->GetPhysical(binData);
    unsigned char *binDelta;
    int frameSize = deltaF->GetFrameDeltaSize();

    unsigned short *dofIdxs = deltaF->GetDofIndices();
    unsigned char boneIdxs[80];
    int idof;
    for (idof = 0; idof < deltaF->GetNumBones(); idof++) {
        boneIdxs[idof] = static_cast<unsigned char>(dofIdxs[idof] / 0xCu);
    }

    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();
    if (floorKey == mNextKey) {
        float *tempBuf = mPrevValues;
        mPrevValues = mNextValues;
        mNextValues = tempBuf;
        prevDeltaIdx = floorDeltaIdx;
        mPrevKey = mNextKey;
        mNextKey = -1;
    } else if (mPrevKey == -1 || floorBinIdx != prevBinIdx || preventReverse) {
        for (idof = 0; idof < deltaF->GetNumBones(); idof++) {
            if (boneMask->GetBone(boneIdxs[idof])) {
                mPrevValues[idof] = deltaF->UnQuantizePhysical(mMinRangesf[idof], binPhys[idof]);
            }
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx);
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    mPrevValues[ibone] += deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
                }
            }
            binDelta += frameSize;
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx - 1);
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    mPrevValues[ibone] -= deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
                }
            }
            binDelta -= frameSize;
        }
    }
    mPrevKey = floorKey;

    int ceilKey = floorKey + 1;
    float scale = 1.0f;
    bool lerpReqd;
    if (!deltaF->mTimes) {
        lerpReqd = currTime != floorTime;
        if (lerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (floorKey == 0) {
        lerpReqd = currTime != 0.0f;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        float floorKeyTime = deltaF->mTimes[floorKey - 1];
        lerpReqd = currTime != floorKeyTime;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    unsigned short *dofIndices = deltaF->GetDofIndices();

    if (lerpReqd && floorKey < deltaF->mNumFrames - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        float ceil;
        binData = deltaF->GetBin(ceilBinIdx);
        binPhys = deltaF->GetPhysical(binData);
        if (ceilKey == mNextKey) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                }
                dofIndices++;
            }
        } else {
            if (ceilBinIdx != floorBinIdx) {
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                    if (boneMask->GetBone(boneIdxs[ibone])) {
                        mNextValues[ibone] = deltaF->UnQuantizePhysical(mMinRangesf[ibone], binPhys[ibone]);
                        sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                    }
                    dofIndices++;
                }
            } else {
                int ceilDeltaIdx = floorDeltaIdx;
                binDelta = deltaF->GetDelta(binData, ceilDeltaIdx);
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                    if (boneMask->GetBone(boneIdxs[ibone])) {
                        ceil = deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta[ibone]);
                        mNextValues[ibone] = mPrevValues[ibone] + ceil;
                        sqt[*dofIndices] = mPrevValues[ibone] + (mNextValues[ibone] - mPrevValues[ibone]) * scale;
                    }
                    dofIndices++;
                }
            }
            mNextKey = ceilKey;
        }
    } else {
        for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                sqt[dofIndices[ibone]] = mPrevValues[ibone];
            }
        }
    }
    if (deltaF->mNumConstBones != 0) {
        unsigned short *constBoneIdxs = deltaF->GetConstBoneIdx();
        float *constPhys = deltaF->GetConstPhysical();
        unsigned short dofIndex;
        unsigned short boneIndex;

        for (int ibone = 0; ibone < deltaF->mNumConstBones; ibone++) {
            dofIndex = *constBoneIdxs++;
            boneIndex = dofIndex / 0xC;
            if (boneMask->GetBone(boneIndex)) {
                sqt[dofIndex] = *constPhys;
                constPhys++;
            } else {
                constPhys++;
            }
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
