#include "FnDeltaQ.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

unsigned char *DeltaQ::GetConstBoneIdx() {
    unsigned int binLen = GetBinLength();
    const int binSize = GetBinSize();
    unsigned char *s = GetBin(0);
    int r = mNumKeys % binLen;

    s += binSize * (mNumKeys / binLen);

    if (r != 0) {
        s += mNumBones * (((r - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical));
    }

    return s;
}

DeltaQPhysical *DeltaQ::GetConstPhysical() {
    unsigned int numBones = mNumBones;
    unsigned int binLength = 1u << mBinLengthPower;
    unsigned int numBins = mNumKeys / binLength;
    unsigned int remainder = mNumKeys - numBins * binLength;
    int s = reinterpret_cast<int>(this) + 0x12;

    s += numBones * sizeof(DeltaQMinRange);
    s += AlignSize2(numBones * (((binLength - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical))) * numBins;

    if (remainder != 0) {
        s += numBones * (((remainder - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical));
    }

    return reinterpret_cast<DeltaQPhysical *>(AlignSize2(s + mNumConstBones));
}

namespace {

static const float kFloatZero = 0.0f;
static const float kFloatOne = 1.0f;

static inline DeltaQMinRange *GetMinRanges(DeltaQ *deltaQ) {
    return deltaQ->GetMinRange();
}

static inline unsigned char *GetBinStart(DeltaQ *deltaQ) {
    return &reinterpret_cast<unsigned char *>(GetMinRanges(deltaQ))[deltaQ->mNumBones * sizeof(DeltaQMinRange)];
}

static inline unsigned char *GetBin(DeltaQ *deltaQ, int binIdx) {
    return &GetBinStart(deltaQ)[binIdx * deltaQ->GetBinSize()];
}

static inline DeltaQPhysical *GetPhysical(unsigned char *binData) {
    return reinterpret_cast<DeltaQPhysical *>(binData);
}

static inline DeltaQDelta *GetDelta(DeltaQ *deltaQ, unsigned char *binData, int deltaIdx) {
    return deltaQ->GetDelta(binData, deltaIdx);
}

static inline float *GetOutputQuat(float *sqt, unsigned char boneIdx) {
    return &sqt[boneIdx * 12 + 4];
}

} // namespace

FnDeltaQ::FnDeltaQ()
    : mPrevKey(-1),            //
      mConstPhysical(nullptr), //
      mMinRanges(nullptr),     //
      mBins(nullptr),          //
      mBinSize(-1),            //
      mPrevQBlock(nullptr),    //
      mPrevQs(nullptr),        //
      mConstBoneIdxs(nullptr) {
    mType = AnimTypeId::ANIM_DELTAQ;
}

void FnDeltaQ::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQTMasked(currTime, nullptr, sqt);
}

bool FnDeltaQ::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask) {
        return EvalSQTMasked(currTime, boneMask, sqt);
    }

    if (!mBins) {
        InitBuffersAsRequired();
    }
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    if (!deltaQ->mNumBones) {
        return true;
    }
    int floorTime = FloatToInt(currTime);
    int floorKey;

    if (!deltaQ->mTimes) {
        if (floorTime < 0) {
            floorKey = 0;
        } else if (floorTime >= deltaQ->mNumKeys) {
            floorKey = deltaQ->mNumKeys - 1;
        } else {
            floorKey = floorTime;
        }
    } else if (floorTime < deltaQ->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex;

        if (mPrevKey < 1) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaQ->mTimes[timeIndex] <= floorTime) {
            while (timeIndex < deltaQ->mNumKeys - 2 && deltaQ->mTimes[timeIndex + 1] <= floorTime) {
                timeIndex++;
            }
        } else {
            while (timeIndex > 0 && deltaQ->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }

    unsigned int binLenPower = deltaQ->GetBinLengthPower();
    unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

    int floorBinIdx = floorKey >> binLenPower;
    int floorDeltaIdx = floorKey & binLenModMask;
    int prevBinIdx = mPrevKey >> binLenPower;
    int prevDeltaIdx;
    unsigned char *binData = &mBins[floorBinIdx * mBinSize];
    DeltaQPhysical *floorPhys = GetPhysical(binData);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    bool preventReverse = false;

    if (floorKey < mPrevKey) {
        preventReverse = !IsReverseDeltaSumEnabled();
    }

    if (mPrevKey != -1 && floorBinIdx == prevBinIdx && floorDeltaIdx != 0 && !preventReverse) {
        prevDeltaIdx = mPrevKey & binLenModMask;
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            floorPhys[ibone].UnQuantize(mPrevQs[ibone]);
        }
        prevDeltaIdx = 0;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 deltaf;
                DeltaQMinRangef minRangef;

                mMinRanges[ibone].UnQuantize(minRangef);
                floorDelta->UnQuantize(minRangef, deltaf);
                mPrevQs[ibone].x += deltaf.x;
                mPrevQs[ibone].y += deltaf.y;
                mPrevQs[ibone].z += deltaf.z;
                floorDelta++;
            }
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 deltaf;
                DeltaQMinRangef minRangef;

                mMinRanges[ibone].UnQuantize(minRangef);
                floorDelta->UnQuantize(minRangef, deltaf);
                mPrevQs[ibone].x -= deltaf.x;
                mPrevQs[ibone].y -= deltaf.y;
                mPrevQs[ibone].z -= deltaf.z;
                floorDelta++;
            }
        }
    }

    if (floorDeltaIdx == 0) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DeltaQRecoverW(floorPhys[ibone].mW, mPrevQs[ibone]);
        }
    } else {
        DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, floorDeltaIdx - 1);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);
            floorDelta++;
        }
    }
    mPrevKey = floorKey;

    int ceilKey = floorKey + 1;
    float scale = 1.0f;
    bool slerpReqd;

    if (!deltaQ->mTimes) {
        slerpReqd = currTime != floorTime;
        if (slerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (floorKey == 0) {
        slerpReqd = currTime != 0.0f;
        if (slerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        float floorKeyTime = deltaQ->mTimes[floorKey - 1];
        slerpReqd = currTime != floorKeyTime;
        if (slerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    if (slerpReqd && floorKey < deltaQ->mNumKeys - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        DeltaQPhysical *ceilPhys = GetPhysical(&mBins[ceilBinIdx * mBinSize]);

        if (ceilBinIdx != floorBinIdx) {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 ceilq;
                UMath::Vector4 &out = *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone]));

                ceilPhys[ibone].UnQuantize(ceilq);
                FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, out);
            }
        } else {
            DeltaQDelta *ceilDelta = GetDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 ceilq;
                UMath::Vector4 &out = *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone]));
                DeltaQMinRangef minRangef;

                mMinRanges[ibone].UnQuantize(minRangef);
                ceilDelta->UnQuantize(minRangef, ceilq);
                ceilq.x += mPrevQs[ibone].x;
                ceilq.y += mPrevQs[ibone].y;
                ceilq.z += mPrevQs[ibone].z;
                DeltaQRecoverW(ceilDelta->mW, ceilq);
                FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, out);
                ceilDelta++;
            }
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone])) = mPrevQs[ibone];
        }
    }

    if (deltaQ->mNumConstBones != 0) {
        for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {
            UMath::Vector4 constq;

            mConstPhysical[ibone].UnQuantize(constq);
            *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, mConstBoneIdxs[ibone])) = constq;
        }
    }

    return true;
}

bool FnDeltaQ::EvalSQTMasked(float currTime, const BoneMask *boneMask, float *sqt) {
    if (!mBins) {
        InitBuffersAsRequired();
    }
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);
    unsigned char numBones = deltaQ->mNumBones;
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (!numBones) {
        return true;
    }
    int floorTime = FloatToInt(currTime);
    int floorKey;

    if (!deltaQ->mTimes) {
        if (floorTime < 0) {
            floorKey = 0;
        } else if (floorTime >= deltaQ->mNumKeys) {
            floorKey = deltaQ->mNumKeys - 1;
        } else {
            floorKey = floorTime;
        }
    } else if (floorTime < deltaQ->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex;

        if (mPrevKey < 1) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaQ->mTimes[timeIndex] <= floorTime) {
            while (timeIndex < deltaQ->mNumKeys - 2 && deltaQ->mTimes[timeIndex + 1] <= floorTime) {
                timeIndex++;
            }
        } else {
            while (timeIndex > 0 && deltaQ->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }

    unsigned int binLenPower = deltaQ->GetBinLengthPower();
    unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

    int floorBinIdx = floorKey >> binLenPower;
    int floorDeltaIdx = floorKey & binLenModMask;
    int prevBinIdx = mPrevKey >> binLenPower;
    int prevDeltaIdx;
    unsigned char *binData = &mBins[floorBinIdx * mBinSize];
    DeltaQPhysical *floorPhys = GetPhysical(binData);
    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();
    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                floorPhys[ibone].UnQuantize(mPrevQs[ibone]);
            }
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 deltaf;
                    DeltaQMinRangef minRangef;

                    mMinRanges[ibone].UnQuantize(minRangef);
                    floorDelta->UnQuantize(minRangef, deltaf);
                    mPrevQs[ibone].x += deltaf.x;
                    mPrevQs[ibone].y += deltaf.y;
                    mPrevQs[ibone].z += deltaf.z;
                }
                floorDelta++;
            }
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 deltaf;
                    DeltaQMinRangef minRangef;

                    mMinRanges[ibone].UnQuantize(minRangef);
                    floorDelta->UnQuantize(minRangef, deltaf);
                    mPrevQs[ibone].x -= deltaf.x;
                    mPrevQs[ibone].y -= deltaf.y;
                    mPrevQs[ibone].z -= deltaf.z;
                }
                floorDelta++;
            }
        }
    }

    if (floorDeltaIdx == 0) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                DeltaQRecoverW(floorPhys[ibone].mW, mPrevQs[ibone]);
            }
        }
    } else {
        DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, floorDeltaIdx - 1);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);
            }
            floorDelta++;
        }
    }
    mPrevKey = floorKey;

    int ceilKey = floorKey + 1;
    float scale = 1.0f;
    bool slerpReqd;

    if (!deltaQ->mTimes) {
        slerpReqd = currTime != floorTime;
        if (slerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (floorKey == 0) {
        slerpReqd = currTime != 0.0f;
        if (slerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        float floorKeyTime = deltaQ->mTimes[floorKey - 1];
        slerpReqd = currTime != floorKeyTime;
        if (slerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    if (slerpReqd && floorKey < deltaQ->mNumKeys - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        DeltaQPhysical *ceilPhys = GetPhysical(&mBins[ceilBinIdx * mBinSize]);

        if (ceilBinIdx != floorBinIdx) {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 ceilq;
                    UMath::Vector4 &out = *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone]));

                    ceilPhys[ibone].UnQuantize(ceilq);
                    FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, out);
                }
            }
        } else {
            DeltaQDelta *ceilDelta = GetDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 ceilq;
                    UMath::Vector4 &out = *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone]));
                    DeltaQMinRangef minRangef;

                    mMinRanges[ibone].UnQuantize(minRangef);
                    ceilDelta->UnQuantize(minRangef, ceilq);
                    ceilq.x += mPrevQs[ibone].x;
                    ceilq.y += mPrevQs[ibone].y;
                    ceilq.z += mPrevQs[ibone].z;
                    DeltaQRecoverW(ceilDelta->mW, ceilq);
                    FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, out);
                }
                ceilDelta++;
            }
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, boneIdxs[ibone])) = mPrevQs[ibone];
            }
        }
    }

    if (deltaQ->mNumConstBones != 0) {
        for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {
            if (boneMask->GetBone(mConstBoneIdxs[ibone])) {
                UMath::Vector4 constq;

                mConstPhysical[ibone].UnQuantize(constq);
                *reinterpret_cast<UMath::Vector4 *>(GetOutputQuat(sqt, mConstBoneIdxs[ibone])) = constq;
            }
        }
    }

    return true;
}

bool FnDeltaQ::EvalWeights(float currTime, float *weights) {
    Eval(currTime, currTime, weights);
    return true;
}

bool FnDeltaQ::EvalVel2D(float currTime, float *vel) {
    Eval(currTime, currTime, vel);
    return true;
}

}; // namespace EAGL4Anim
