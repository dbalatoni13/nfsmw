#include "FnDeltaQ.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

unsigned char *DeltaQ::GetConstBoneIdx() {
    unsigned int numBones = mNumBones;
    unsigned int binLength = 1u << mBinLengthPower;
    unsigned int numBins = mNumKeys / binLength;
    unsigned int remainder = mNumKeys - numBins * binLength;
    int s = reinterpret_cast<int>(this) + 0x12;

    s += numBones * sizeof(DeltaQMinRange);
    s += AlignSize2(numBones * (((binLength - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical))) * numBins;

    if (!remainder) {
        return reinterpret_cast<unsigned char *>(s);
    }

    s += numBones * (((remainder - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical));
    return reinterpret_cast<unsigned char *>(s);
}

DeltaQPhysical *DeltaQ::GetConstPhysical() {
    unsigned int numBones = mNumBones;
    unsigned int binLength = 1u << mBinLengthPower;
    unsigned int numBins = mNumKeys / binLength;
    unsigned int remainder = mNumKeys - numBins * binLength;
    int s = reinterpret_cast<int>(this) + 0x12;

    s += numBones * sizeof(DeltaQMinRange);
    s += AlignSize2(numBones * (((binLength - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical))) * numBins;

    if (remainder > 0) {
        s += numBones * (((remainder - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical));
    }

    return reinterpret_cast<DeltaQPhysical *>(AlignSize2(s + mNumConstBones));
}

namespace {

static const float kFloatZero = 0.0f;
static const float kFloatOne = 1.0f;
static const float kRangeScale16Bit = 3.0518044e-5f;
static const float kRangeScale15Bit = 6.1037019e-5f;
static const float kRangeScale8Bit = 7.8431377e-3f;
static const float kRangeScale7Bit = 1.5748032e-2f;
static const float kRangeScale16To7Bit = 9.6119827e-7f;
static const float kRangeScale16To8Bit = 4.7871444e-7f;

static inline int GetFrameDeltaSize(const DeltaQ *deltaQ) {
    return deltaQ->mNumBones * sizeof(DeltaQDelta);
}

static inline int GetBinSize(const DeltaQ *deltaQ) {
    return static_cast<int>(AlignSize2((deltaQ->mNumBones * sizeof(DeltaQPhysical)) +
                                       ((deltaQ->GetBinLength() - 1) * GetFrameDeltaSize(deltaQ))));
}

static inline DeltaQMinRange *GetMinRanges(DeltaQ *deltaQ) {
    return deltaQ->GetMinRange();
}

static inline unsigned char *GetBinStart(DeltaQ *deltaQ) {
    return &reinterpret_cast<unsigned char *>(GetMinRanges(deltaQ))[deltaQ->mNumBones * sizeof(DeltaQMinRange)];
}

static inline unsigned char *GetBin(DeltaQ *deltaQ, int binIdx) {
    return &GetBinStart(deltaQ)[binIdx * GetBinSize(deltaQ)];
}

static inline DeltaQPhysical *GetPhysical(unsigned char *binData) {
    return reinterpret_cast<DeltaQPhysical *>(binData);
}

static inline DeltaQDelta *GetDelta(DeltaQ *deltaQ, unsigned char *binData, int deltaIdx) {
    return reinterpret_cast<DeltaQDelta *>(&binData[deltaQ->mNumBones * sizeof(DeltaQPhysical) +
                                                     (deltaIdx * GetFrameDeltaSize(deltaQ))]);
}

static inline unsigned char *GetConstBoneIdx(DeltaQ *deltaQ) {
    const int binSize = GetBinSize(deltaQ);
    int numBins = deltaQ->mNumKeys >> deltaQ->GetBinLengthPower();
    unsigned char *s = &GetBin(deltaQ, 0)[binSize * numBins];
    int r = deltaQ->mNumKeys & deltaQ->GetBinLengthModMask();

    if (r > 0) {
        s = reinterpret_cast<unsigned char *>(
            AlignSize2(reinterpret_cast<intptr_t>(s + (deltaQ->mNumBones * sizeof(DeltaQPhysical)) +
                                                  ((r - 1) * GetFrameDeltaSize(deltaQ)))));
    }
    if (deltaQ->mNumBones == 0) {
        s = reinterpret_cast<unsigned char *>(AlignSize2(reinterpret_cast<intptr_t>(s)));
    }

    return s;
}

static inline DeltaQPhysical *GetConstPhysical(DeltaQ *deltaQ) {
    return reinterpret_cast<DeltaQPhysical *>(AlignSize2(reinterpret_cast<intptr_t>(&GetConstBoneIdx(deltaQ)[deltaQ->mNumConstBones])));
}

static inline void RecoverW(int signBit, UMath::Vector4 &q) {
    float ndotn = q.x * q.x + q.y * q.y + q.z * q.z;

    if (ndotn <= kFloatOne) {
        q.w = FastSqrt(kFloatOne - ndotn);
        if (signBit) {
            q.w = -q.w;
        }
    } else {
        float len = FastSqrt(ndotn);

        q.x /= len;
        q.y /= len;
        q.z /= len;
        q.w = kFloatZero;
    }
}

static inline void DecodePhysical(const DeltaQPhysical &physical, UMath::Vector4 &q) {
    q.x = physical.mX * kRangeScale15Bit - kFloatOne;
    q.y = physical.mY * kRangeScale16Bit - kFloatOne;
    q.z = physical.mZ * kRangeScale16Bit - kFloatOne;
    RecoverW(physical.mW, q);
}

static inline void DecodeMinRange(const DeltaQMinRange &minRange, DeltaQMinRangef &minRangef) {
    minRangef.mMin.x = minRange.mMin[0] * kRangeScale16Bit - kFloatOne;
    minRangef.mMin.y = minRange.mMin[1] * kRangeScale16Bit - kFloatOne;
    minRangef.mMin.z = minRange.mMin[2] * kRangeScale16Bit - kFloatOne;

    minRangef.mRange.x = minRange.mRange[0] * kRangeScale16To7Bit;
    minRangef.mRange.y = minRange.mRange[1] * kRangeScale16To8Bit;
    minRangef.mRange.z = minRange.mRange[2] * kRangeScale16To8Bit;
}

static inline void DecodeDelta(const DeltaQMinRange &minRange, const DeltaQDelta &delta, UMath::Vector4 &q) {
    DeltaQMinRangef minRangef;

    DecodeMinRange(minRange, minRangef);

    q.x = minRangef.mMin.x + minRangef.mRange.x * delta.mX;
    q.y = minRangef.mMin.y + minRangef.mRange.y * delta.mY;
    q.z = minRangef.mMin.z + minRangef.mRange.z * delta.mZ;
    RecoverW(delta.mW, q);
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
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    if (!mBins) {
        unsigned char numBones = deltaQ->mNumBones;
        DeltaQMinRange *minRanges = reinterpret_cast<DeltaQMinRange *>(reinterpret_cast<unsigned char *>(deltaQ) + 0x12);

        mBins = reinterpret_cast<unsigned char *>(minRanges) + numBones * sizeof(DeltaQMinRange);
        mConstBoneIdxs = reinterpret_cast<unsigned char *>(deltaQ->GetConstBoneIdx());
        mConstPhysical = reinterpret_cast<DeltaQPhysical *>(deltaQ->GetConstPhysical());
        mBinSize = AlignSize2(numBones * ((((1 << deltaQ->mBinLengthPower) - 1) * 3) + 6));

        if (numBones != 0) {
            UMath::Vector4 *prevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(numBones * sizeof(*mPrevQs)));

            mMinRanges = minRanges;
            mPrevQs = prevQs;
            mPrevQBlock = prevQs;
        } else {
            mMinRanges = minRanges;
        }
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
    unsigned char *binData = GetBin(deltaQ, floorBinIdx);
    DeltaQPhysical *floorPhys = GetPhysical(binData);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();

    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DecodePhysical(floorPhys[ibone], mPrevQs[ibone]);
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 delta;

                DecodeDelta(mMinRanges[ibone], *floorDelta, delta);
                mPrevQs[ibone].x += delta.x;
                mPrevQs[ibone].y += delta.y;
                mPrevQs[ibone].z += delta.z;
                floorDelta++;
            }
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 delta;

                DecodeDelta(mMinRanges[ibone], *floorDelta, delta);
                mPrevQs[ibone].x -= delta.x;
                mPrevQs[ibone].y -= delta.y;
                mPrevQs[ibone].z -= delta.z;
                floorDelta++;
            }
        }
    }

    if (floorDeltaIdx == 0) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            RecoverW(floorPhys[ibone].mW, mPrevQs[ibone]);
        }
    } else {
        DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, floorDeltaIdx - 1);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            RecoverW(floorDelta->mW, mPrevQs[ibone]);
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
        DeltaQPhysical *ceilPhys = GetPhysical(GetBin(deltaQ, ceilBinIdx));

        if (ceilBinIdx != floorBinIdx) {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 ceilq;
                float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                float *ceilQ = reinterpret_cast<float *>(&ceilq);
                float *out = GetOutputQuat(sqt, boneIdxs[ibone]);

                DecodePhysical(ceilPhys[ibone], ceilq);
                if (prevQ[0] * ceilQ[0] + prevQ[1] * ceilQ[1] + prevQ[2] * ceilQ[2] + prevQ[3] * ceilQ[3] > 0.0f) {
                    out[0] = scale * (ceilQ[0] - prevQ[0]) + prevQ[0];
                    out[1] = scale * (ceilQ[1] - prevQ[1]) + prevQ[1];
                    out[2] = scale * (ceilQ[2] - prevQ[2]) + prevQ[2];
                    out[3] = scale * (ceilQ[3] - prevQ[3]) + prevQ[3];
                } else {
                    out[0] = prevQ[0] - scale * (ceilQ[0] + prevQ[0]);
                    out[1] = prevQ[1] - scale * (ceilQ[1] + prevQ[1]);
                    out[2] = prevQ[2] - scale * (ceilQ[2] + prevQ[2]);
                    out[3] = prevQ[3] - scale * (ceilQ[3] + prevQ[3]);
                }

                {
                    float s = 1.0f / FastSqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2] + out[3] * out[3]);

                    out[0] *= s;
                    out[1] *= s;
                    out[2] *= s;
                    out[3] *= s;
                }
            }
        } else {
            DeltaQDelta *ceilDelta = GetDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 delta;
                UMath::Vector4 ceilq;

                DecodeDelta(mMinRanges[ibone], *ceilDelta, delta);
                ceilq.x = mPrevQs[ibone].x + delta.x;
                ceilq.y = mPrevQs[ibone].y + delta.y;
                ceilq.z = mPrevQs[ibone].z + delta.z;
                RecoverW(ceilDelta->mW, ceilq);

                FastQuatBlendF4(scale, reinterpret_cast<float *>(&mPrevQs[ibone]), reinterpret_cast<float *>(&ceilq),
                                GetOutputQuat(sqt, boneIdxs[ibone]));
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
            float *out = GetOutputQuat(sqt, mConstBoneIdxs[ibone]);

            out[0] = mConstPhysical[ibone].mX * kRangeScale15Bit - kFloatOne;
            out[1] = mConstPhysical[ibone].mY * kRangeScale16Bit - kFloatOne;
            out[2] = mConstPhysical[ibone].mZ * kRangeScale16Bit - kFloatOne;
            RecoverW(mConstPhysical[ibone].mW, *reinterpret_cast<UMath::Vector4 *>(out));
        }
    }

    return true;
}

bool FnDeltaQ::EvalSQTMasked(float currTime, const BoneMask *boneMask, float *sqt) {
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    if (!mBins) {
        unsigned char numBones = deltaQ->mNumBones;
        DeltaQMinRange *minRanges = reinterpret_cast<DeltaQMinRange *>(reinterpret_cast<unsigned char *>(deltaQ) + 0x12);

        mBins = reinterpret_cast<unsigned char *>(minRanges) + numBones * sizeof(DeltaQMinRange);
        mConstBoneIdxs = reinterpret_cast<unsigned char *>(deltaQ->GetConstBoneIdx());
        mConstPhysical = reinterpret_cast<DeltaQPhysical *>(deltaQ->GetConstPhysical());
        mBinSize = AlignSize2(numBones * ((((1 << deltaQ->mBinLengthPower) - 1) * 3) + 6));

        if (numBones != 0) {
            UMath::Vector4 *prevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(numBones * sizeof(*mPrevQs)));

            mMinRanges = minRanges;
            mPrevQs = prevQs;
            mPrevQBlock = prevQs;
        } else {
            mMinRanges = minRanges;
        }
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
    unsigned char *binData = GetBin(deltaQ, floorBinIdx);
    DeltaQPhysical *floorPhys = GetPhysical(binData);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 ||
        (floorKey < mPrevKey && !IsReverseDeltaSumEnabled())) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                DecodePhysical(floorPhys[ibone], mPrevQs[ibone]);
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
                    UMath::Vector4 delta;

                    DecodeDelta(mMinRanges[ibone], *floorDelta, delta);
                    mPrevQs[ibone].x += delta.x;
                    mPrevQs[ibone].y += delta.y;
                    mPrevQs[ibone].z += delta.z;
                }
                floorDelta++;
            }
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 delta;

                    DecodeDelta(mMinRanges[ibone], *floorDelta, delta);
                    mPrevQs[ibone].x -= delta.x;
                    mPrevQs[ibone].y -= delta.y;
                    mPrevQs[ibone].z -= delta.z;
                }
                floorDelta++;
            }
        }
    }

    if (floorDeltaIdx == 0) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                RecoverW(floorPhys[ibone].mW, mPrevQs[ibone]);
            }
        }
    } else {
        DeltaQDelta *floorDelta = GetDelta(deltaQ, binData, floorDeltaIdx - 1);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                RecoverW(floorDelta->mW, mPrevQs[ibone]);
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
        DeltaQPhysical *ceilPhys = GetPhysical(GetBin(deltaQ, ceilBinIdx));

        if (ceilBinIdx != floorBinIdx) {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 ceilq;
                    float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                    float *ceilQ = reinterpret_cast<float *>(&ceilq);
                    float *out = GetOutputQuat(sqt, boneIdxs[ibone]);

                    ceilQ[0] = ceilPhys[ibone].mX * kRangeScale15Bit - kFloatOne;
                    ceilQ[1] = ceilPhys[ibone].mY * kRangeScale16Bit - kFloatOne;
                    ceilQ[2] = ceilPhys[ibone].mZ * kRangeScale16Bit - kFloatOne;
                    RecoverW(ceilPhys[ibone].mW, ceilq);
                    if (prevQ[0] * ceilQ[0] + prevQ[1] * ceilQ[1] + prevQ[2] * ceilQ[2] + prevQ[3] * ceilQ[3] > 0.0f) {
                        out[0] = scale * (ceilQ[0] - prevQ[0]) + prevQ[0];
                        out[1] = scale * (ceilQ[1] - prevQ[1]) + prevQ[1];
                        out[2] = scale * (ceilQ[2] - prevQ[2]) + prevQ[2];
                        out[3] = scale * (ceilQ[3] - prevQ[3]) + prevQ[3];
                    } else {
                        out[0] = prevQ[0] - scale * (ceilQ[0] + prevQ[0]);
                        out[1] = prevQ[1] - scale * (ceilQ[1] + prevQ[1]);
                        out[2] = prevQ[2] - scale * (ceilQ[2] + prevQ[2]);
                        out[3] = prevQ[3] - scale * (ceilQ[3] + prevQ[3]);
                    }

                    {
                        float s = 1.0f / FastSqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2] + out[3] * out[3]);

                        out[0] *= s;
                        out[1] *= s;
                        out[2] *= s;
                        out[3] *= s;
                    }
                }
            }
        } else {
            DeltaQDelta *ceilDelta = GetDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 delta;
                    UMath::Vector4 ceilq;
                    float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                    float *ceilQ = reinterpret_cast<float *>(&ceilq);
                    float *out = GetOutputQuat(sqt, boneIdxs[ibone]);

                    DecodeDelta(mMinRanges[ibone], *ceilDelta, delta);
                    ceilq.x = mPrevQs[ibone].x + delta.x;
                    ceilq.y = mPrevQs[ibone].y + delta.y;
                    ceilq.z = mPrevQs[ibone].z + delta.z;
                    RecoverW(ceilDelta->mW, ceilq);

                    if (prevQ[0] * ceilQ[0] + prevQ[1] * ceilQ[1] + prevQ[2] * ceilQ[2] + prevQ[3] * ceilQ[3] > 0.0f) {
                        out[0] = scale * (ceilQ[0] - prevQ[0]) + prevQ[0];
                        out[1] = scale * (ceilQ[1] - prevQ[1]) + prevQ[1];
                        out[2] = scale * (ceilQ[2] - prevQ[2]) + prevQ[2];
                        out[3] = scale * (ceilQ[3] - prevQ[3]) + prevQ[3];
                    } else {
                        out[0] = prevQ[0] - scale * (ceilQ[0] + prevQ[0]);
                        out[1] = prevQ[1] - scale * (ceilQ[1] + prevQ[1]);
                        out[2] = prevQ[2] - scale * (ceilQ[2] + prevQ[2]);
                        out[3] = prevQ[3] - scale * (ceilQ[3] + prevQ[3]);
                    }

                    {
                        float s = 1.0f / FastSqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2] + out[3] * out[3]);

                        out[0] *= s;
                        out[1] *= s;
                        out[2] *= s;
                        out[3] *= s;
                    }
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

                DecodePhysical(mConstPhysical[ibone], constq);
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

void FnDeltaQ::InitBuffersAsRequired() {
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    mMinRanges = GetMinRanges(deltaQ);
    mBins = GetBinStart(deltaQ);
    mBinSize = GetBinSize(deltaQ);
    mConstBoneIdxs = GetConstBoneIdx(deltaQ);
    mConstPhysical = GetConstPhysical(deltaQ);

    if (deltaQ->mNumBones != 0) {
        mPrevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPrevQs)));
        mPrevQBlock = mPrevQs;
    }
}

}; // namespace EAGL4Anim
