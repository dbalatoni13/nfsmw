#include "FnDeltaSingleQ.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

namespace {

static const float kSingleQFloatZero = 0.0f;
static const float kSingleQFloatOne = 1.0f;
static const float kSingleQHalf = 0.5f;
static const float kSingleQPi = 3.1415927f;
static const float kSingleQAngleScale16Bit = 9.5875265e-5f;
static const float kSingleQRangeScale16Bit = 3.0518044e-5f;
static const float kSingleQRangeScale8Bit = 7.8431377e-3f;
static const float kSingleQRangeScale4Bit = 0.13333334f;

static inline int GetSingleQFrameDeltaSize(const DeltaSingleQ *deltaQ) {
    return deltaQ->mNumBones * sizeof(DeltaSingleQDelta);
}

static inline int GetSingleQBinSize(const DeltaSingleQ *deltaQ) {
    return static_cast<int>(AlignSize2((deltaQ->mNumBones * sizeof(DeltaSingleQPhysical)) +
                                       ((deltaQ->GetBinLength() - 1) * GetSingleQFrameDeltaSize(deltaQ))));
}

static inline DeltaSingleQMinRange *GetSingleQMinRanges(DeltaSingleQ *deltaQ) {
    return deltaQ->GetMinRange();
}

static inline unsigned char *GetSingleQBinStart(DeltaSingleQ *deltaQ) {
    return &reinterpret_cast<unsigned char *>(GetSingleQMinRanges(deltaQ))[deltaQ->mNumBones * sizeof(DeltaSingleQMinRange)];
}

static inline unsigned char *GetSingleQBin(DeltaSingleQ *deltaQ, int binIdx) {
    return &GetSingleQBinStart(deltaQ)[binIdx * GetSingleQBinSize(deltaQ)];
}

static inline DeltaSingleQPhysical *GetSingleQPhysical(unsigned char *binData) {
    return reinterpret_cast<DeltaSingleQPhysical *>(binData);
}

static inline DeltaSingleQDelta *GetSingleQDelta(DeltaSingleQ *deltaQ, unsigned char *binData, int deltaIdx) {
    return reinterpret_cast<DeltaSingleQDelta *>(&binData[deltaQ->mNumBones * sizeof(DeltaSingleQPhysical) +
                                                               (deltaIdx * GetSingleQFrameDeltaSize(deltaQ))]);
}

static inline void NormalizeSingleQQuat(UMath::Vector4 &q) {
    float s = kSingleQFloatOne / FastSqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

    q.x *= s;
    q.y *= s;
    q.z *= s;
    q.w *= s;
}

static inline void SingleQEulToQuat(const float *eulData, float *quatData) {
    float ti = eulData[0] * kSingleQHalf;
    float tj = eulData[1] * kSingleQHalf;
    float th = eulData[2] * kSingleQHalf;
    float ci = cosf(ti);
    float cj = cosf(tj);
    float ch = cosf(th);
    float si = sinf(ti);
    float sj = sinf(tj);
    float sh = sinf(th);
    float cc = ci * cj;
    float cs = ci * sj;
    float sc = si * cj;
    float ss = si * sj;

    quatData[0] = sc * ch - cs * sh;
    quatData[1] = cs * ch + sc * sh;
    quatData[2] = cc * sh - ss * ch;
    quatData[3] = cc * ch + ss * sh;
}

static inline void SingleQQuatMultXxYxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, const UMath::Vector4 &c, UMath::Vector4 &result) {
    float awby = a.w * b.y;
    float axbw = a.x * b.w;
    float awbw = a.w * b.w;
    float naxby = -(a.x * b.y);

    result.x = axbw * c.w - awby * c.z;
    result.y = axbw * c.z + awby * c.w;
    result.z = naxby * c.w + awbw * c.z;
    result.w = -naxby * c.z + awbw * c.w;
}

static inline void SingleQQuatMultXxQ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = a.w * b.x + a.x * b.w;
    result.y = a.w * b.y + a.x * b.z;
    result.z = -(a.x * b.y) + a.w * b.z;
    result.w = -(a.x * b.x) + a.w * b.w;
}

static inline void SingleQQuatMultQxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = a.x * b.w - a.y * b.z;
    result.y = a.x * b.z + a.y * b.w;
    result.z = a.z * b.w + a.w * b.z;
    result.w = -(a.z * b.z) + a.w * b.w;
}

static inline void DecodeSingleQMinRange(const DeltaSingleQMinRange &minRange, DeltaSingleQMinRangef &minRangef) {
    minRangef.mConst0 = minRange.mConst0 * kSingleQAngleScale16Bit - kSingleQPi;
    minRangef.mConst1 = minRange.mConst1 * kSingleQAngleScale16Bit - kSingleQPi;
    minRangef.mMin[0] = minRange.mMin[0] * kSingleQRangeScale16Bit - kSingleQFloatOne;
    minRangef.mMin[1] = minRange.mMin[1] * kSingleQRangeScale16Bit - kSingleQFloatOne;
    minRangef.mRange[0] = 2.0f * (minRange.mRange[0] * kSingleQRangeScale16Bit) * kSingleQRangeScale4Bit;
    minRangef.mRange[1] = 2.0f * (minRange.mRange[1] * kSingleQRangeScale16Bit) * kSingleQRangeScale4Bit;
    minRangef.mIndex = static_cast<unsigned char>(minRange.mIndex);
}

static inline void DecodeSingleQPhysical(const DeltaSingleQPhysical &physical, int index, UMath::Vector4 &q) {
    q.x = kSingleQFloatZero;
    q.y = kSingleQFloatZero;
    q.z = kSingleQFloatZero;
    q.w = physical.mW * kSingleQRangeScale8Bit - kSingleQFloatOne;

    if (index == 0) {
        q.x = physical.mV * kSingleQRangeScale8Bit - kSingleQFloatOne;
    } else if (index == 1) {
        q.y = physical.mV * kSingleQRangeScale8Bit - kSingleQFloatOne;
    } else {
        q.z = physical.mV * kSingleQRangeScale8Bit - kSingleQFloatOne;
    }
}

static inline void DecodeSingleQDelta(const DeltaSingleQMinRange &minRange, const DeltaSingleQDelta &delta, UMath::Vector4 &q) {
    DeltaSingleQMinRangef minRangef;
    float v = minRangef.mMin[0];
    float w = minRangef.mMin[1];

    DecodeSingleQMinRange(minRange, minRangef);

    v = minRangef.mRange[0] * delta.mV + minRangef.mMin[0];
    w = minRangef.mRange[1] * delta.mW + minRangef.mMin[1];

    q.x = kSingleQFloatZero;
    q.y = kSingleQFloatZero;
    q.z = kSingleQFloatZero;
    q.w = w;

    if (minRangef.mIndex == 0) {
        q.x = v;
    } else if (minRangef.mIndex == 1) {
        q.y = v;
    } else {
        q.z = v;
    }
}

static inline void ComposeSingleQQuat(unsigned short index, const UMath::Vector4 &pre, const UMath::Vector4 &mid,
                               const UMath::Vector4 &post, UMath::Vector4 &result) {
    if (index == 1) {
        SingleQQuatMultXxYxZ(pre, mid, post, result);
    } else if (index == 0) {
        SingleQQuatMultXxQ(mid, post, result);
    } else {
        SingleQQuatMultQxZ(pre, mid, result);
    }
}

static inline float *GetSingleQOutputQuat(float *sqt, unsigned char boneIdx) {
    return &sqt[boneIdx * 12 + 4];
}

} // namespace

FnDeltaSingleQ::FnDeltaSingleQ()
    : mMinRanges(nullptr), //
      mBins(nullptr),      //
      mBinSize(-1),        //
      mPrevKey(-1),        //
      mPrevQBlock(nullptr), //
      mPrevQs(nullptr),     //
      mPreMultQs(nullptr),  //
      mPostMultQs(nullptr) {
    mType = AnimTypeId::ANIM_DELTASINGLEQ;
}

void FnDeltaSingleQ::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

bool FnDeltaSingleQ::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask) {
        return EvalSQTMasked(currTime, boneMask, sqt);
    }
    DeltaSingleQ *deltaQ = reinterpret_cast<DeltaSingleQ *>(mpAnim);

    if (!mPrevQs) {
        mMinRanges = GetSingleQMinRanges(deltaQ);
        mBins = GetSingleQBinStart(deltaQ);
        mBinSize = GetSingleQBinSize(deltaQ);

        if (deltaQ->mNumBones != 0) {
            float eul[3];

            mPrevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPrevQs)));
            mPrevQBlock = mPrevQs;
            mPreMultQs =
                reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPreMultQs)));
            mPostMultQs =
                reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPostMultQs)));

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                DeltaSingleQMinRangef minRangef;

                DecodeSingleQMinRange(mMinRanges[ibone], minRangef);

                mPreMultQs[ibone].x = kSingleQFloatZero;
                mPreMultQs[ibone].y = kSingleQFloatZero;
                mPreMultQs[ibone].z = kSingleQFloatZero;
                mPreMultQs[ibone].w = kSingleQFloatOne;
                mPostMultQs[ibone].x = kSingleQFloatZero;
                mPostMultQs[ibone].y = kSingleQFloatZero;
                mPostMultQs[ibone].z = kSingleQFloatZero;
                mPostMultQs[ibone].w = kSingleQFloatOne;

                if (minRangef.mIndex == 0) {
                    eul[0] = kSingleQFloatZero;
                    eul[1] = minRangef.mConst0;
                    eul[2] = minRangef.mConst1;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
                } else if (minRangef.mIndex == 1) {
                    eul[0] = minRangef.mConst0;
                    eul[1] = kSingleQFloatZero;
                    eul[2] = kSingleQFloatZero;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));

                    eul[0] = kSingleQFloatZero;
                    eul[1] = kSingleQFloatZero;
                    eul[2] = minRangef.mConst1;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
                } else {
                    eul[0] = minRangef.mConst0;
                    eul[1] = minRangef.mConst1;
                    eul[2] = kSingleQFloatZero;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));
                }
            }
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
    unsigned char *binData = GetSingleQBin(deltaQ, floorBinIdx);
    DeltaSingleQPhysical *floorPhys = GetSingleQPhysical(binData);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorKey < mPrevKey) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DecodeSingleQPhysical(floorPhys[ibone], mMinRanges[ibone].mIndex, mPrevQs[ibone]);
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            DeltaSingleQDelta *floorDelta = GetSingleQDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 deltaf;

                DecodeSingleQDelta(mMinRanges[ibone], floorDelta[ibone], deltaf);
                mPrevQs[ibone].x += deltaf.x;
                mPrevQs[ibone].y += deltaf.y;
                mPrevQs[ibone].z += deltaf.z;
                mPrevQs[ibone].w += deltaf.w;
            }
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
        DeltaSingleQPhysical *ceilPhys = GetSingleQPhysical(GetSingleQBin(deltaQ, ceilBinIdx));

        if (ceilBinIdx == floorBinIdx) {
            DeltaSingleQDelta *ceilDelta = GetSingleQDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 ceilq;
                UMath::Vector4 interpq;
                UMath::Vector4 outq;

                DecodeSingleQDelta(mMinRanges[ibone], ceilDelta[ibone], ceilq);
                ceilq.x += mPrevQs[ibone].x;
                ceilq.y += mPrevQs[ibone].y;
                ceilq.z += mPrevQs[ibone].z;
                ceilq.w += mPrevQs[ibone].w;

                interpq.x = mPrevQs[ibone].x + (ceilq.x - mPrevQs[ibone].x) * scale;
                interpq.y = mPrevQs[ibone].y + (ceilq.y - mPrevQs[ibone].y) * scale;
                interpq.z = mPrevQs[ibone].z + (ceilq.z - mPrevQs[ibone].z) * scale;
                interpq.w = mPrevQs[ibone].w + (ceilq.w - mPrevQs[ibone].w) * scale;
                NormalizeSingleQQuat(interpq);

                ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], interpq, mPostMultQs[ibone], outq);
                *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
            }
        } else {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                UMath::Vector4 ceilq;
                UMath::Vector4 interpq;
                UMath::Vector4 outq;

                DecodeSingleQPhysical(ceilPhys[ibone], mMinRanges[ibone].mIndex, ceilq);
                interpq.x = mPrevQs[ibone].x + (ceilq.x - mPrevQs[ibone].x) * scale;
                interpq.y = mPrevQs[ibone].y + (ceilq.y - mPrevQs[ibone].y) * scale;
                interpq.z = mPrevQs[ibone].z + (ceilq.z - mPrevQs[ibone].z) * scale;
                interpq.w = mPrevQs[ibone].w + (ceilq.w - mPrevQs[ibone].w) * scale;
                NormalizeSingleQQuat(interpq);

                ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], interpq, mPostMultQs[ibone], outq);
                *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
            }
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 outq;

            ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], mPrevQs[ibone], mPostMultQs[ibone], outq);
            *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
        }
    }

    return true;
}

bool FnDeltaSingleQ::EvalSQTMasked(float currTime, const BoneMask *boneMask, float *sqt) {
    DeltaSingleQ *deltaQ = reinterpret_cast<DeltaSingleQ *>(mpAnim);

    if (!mPrevQs) {
        mMinRanges = GetSingleQMinRanges(deltaQ);
        mBins = GetSingleQBinStart(deltaQ);
        mBinSize = GetSingleQBinSize(deltaQ);

        if (deltaQ->mNumBones != 0) {
            float eul[3];

            mPrevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPrevQs)));
            mPrevQBlock = mPrevQs;
            mPreMultQs =
                reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPreMultQs)));
            mPostMultQs =
                reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPostMultQs)));

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                DeltaSingleQMinRangef minRangef;

                DecodeSingleQMinRange(mMinRanges[ibone], minRangef);

                mPreMultQs[ibone].x = kSingleQFloatZero;
                mPreMultQs[ibone].y = kSingleQFloatZero;
                mPreMultQs[ibone].z = kSingleQFloatZero;
                mPreMultQs[ibone].w = kSingleQFloatOne;
                mPostMultQs[ibone].x = kSingleQFloatZero;
                mPostMultQs[ibone].y = kSingleQFloatZero;
                mPostMultQs[ibone].z = kSingleQFloatZero;
                mPostMultQs[ibone].w = kSingleQFloatOne;

                if (minRangef.mIndex == 0) {
                    eul[0] = kSingleQFloatZero;
                    eul[1] = minRangef.mConst0;
                    eul[2] = minRangef.mConst1;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
                } else if (minRangef.mIndex == 1) {
                    eul[0] = minRangef.mConst0;
                    eul[1] = kSingleQFloatZero;
                    eul[2] = kSingleQFloatZero;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));

                    eul[0] = kSingleQFloatZero;
                    eul[1] = kSingleQFloatZero;
                    eul[2] = minRangef.mConst1;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
                } else {
                    eul[0] = minRangef.mConst0;
                    eul[1] = minRangef.mConst1;
                    eul[2] = kSingleQFloatZero;
                    SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));
                }
            }
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
    unsigned char *binData = GetSingleQBin(deltaQ, floorBinIdx);
    DeltaSingleQPhysical *floorPhys = GetSingleQPhysical(binData);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorKey < mPrevKey) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                DecodeSingleQPhysical(floorPhys[ibone], mMinRanges[ibone].mIndex, mPrevQs[ibone]);
            }
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            DeltaSingleQDelta *floorDelta = GetSingleQDelta(deltaQ, binData, iframe);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 deltaf;

                    DecodeSingleQDelta(mMinRanges[ibone], floorDelta[ibone], deltaf);
                    mPrevQs[ibone].x += deltaf.x;
                    mPrevQs[ibone].y += deltaf.y;
                    mPrevQs[ibone].z += deltaf.z;
                    mPrevQs[ibone].w += deltaf.w;
                }
            }
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
        DeltaSingleQPhysical *ceilPhys = GetSingleQPhysical(GetSingleQBin(deltaQ, ceilBinIdx));

        if (ceilBinIdx == floorBinIdx) {
            DeltaSingleQDelta *ceilDelta = GetSingleQDelta(deltaQ, binData, floorDeltaIdx);

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 ceilq;
                    UMath::Vector4 interpq;
                    UMath::Vector4 outq;

                    DecodeSingleQDelta(mMinRanges[ibone], ceilDelta[ibone], ceilq);
                    ceilq.x += mPrevQs[ibone].x;
                    ceilq.y += mPrevQs[ibone].y;
                    ceilq.z += mPrevQs[ibone].z;
                    ceilq.w += mPrevQs[ibone].w;

                    interpq.x = mPrevQs[ibone].x + (ceilq.x - mPrevQs[ibone].x) * scale;
                    interpq.y = mPrevQs[ibone].y + (ceilq.y - mPrevQs[ibone].y) * scale;
                    interpq.z = mPrevQs[ibone].z + (ceilq.z - mPrevQs[ibone].z) * scale;
                    interpq.w = mPrevQs[ibone].w + (ceilq.w - mPrevQs[ibone].w) * scale;
                    NormalizeSingleQQuat(interpq);

                    ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], interpq, mPostMultQs[ibone], outq);
                    *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
                }
            }
        } else {
            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    UMath::Vector4 ceilq;
                    UMath::Vector4 interpq;
                    UMath::Vector4 outq;

                    DecodeSingleQPhysical(ceilPhys[ibone], mMinRanges[ibone].mIndex, ceilq);
                    interpq.x = mPrevQs[ibone].x + (ceilq.x - mPrevQs[ibone].x) * scale;
                    interpq.y = mPrevQs[ibone].y + (ceilq.y - mPrevQs[ibone].y) * scale;
                    interpq.z = mPrevQs[ibone].z + (ceilq.z - mPrevQs[ibone].z) * scale;
                    interpq.w = mPrevQs[ibone].w + (ceilq.w - mPrevQs[ibone].w) * scale;
                    NormalizeSingleQQuat(interpq);

                    ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], interpq, mPostMultQs[ibone], outq);
                    *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
                }
            }
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector4 outq;

                ComposeSingleQQuat(mMinRanges[ibone].mIndex, mPreMultQs[ibone], mPrevQs[ibone], mPostMultQs[ibone], outq);
                *reinterpret_cast<UMath::Vector4 *>(GetSingleQOutputQuat(sqt, boneIdxs[ibone])) = outq;
            }
        }
    }

    return true;
}

void FnDeltaSingleQ::InitBuffersAsRequired() {
    DeltaSingleQ *deltaQ = reinterpret_cast<DeltaSingleQ *>(mpAnim);

    mMinRanges = GetSingleQMinRanges(deltaQ);
    mBins = GetSingleQBinStart(deltaQ);
    mBinSize = GetSingleQBinSize(deltaQ);

    if (deltaQ->mNumBones != 0) {
        float eul[3];

        mPrevQs = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPrevQs)));
        mPrevQBlock = mPrevQs;
        mPreMultQs =
            reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPreMultQs)));
        mPostMultQs =
            reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(*mPostMultQs)));

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DeltaSingleQMinRangef minRangef;

            DecodeSingleQMinRange(mMinRanges[ibone], minRangef);

            mPreMultQs[ibone].x = kSingleQFloatZero;
            mPreMultQs[ibone].y = kSingleQFloatZero;
            mPreMultQs[ibone].z = kSingleQFloatZero;
            mPreMultQs[ibone].w = kSingleQFloatOne;
            mPostMultQs[ibone].x = kSingleQFloatZero;
            mPostMultQs[ibone].y = kSingleQFloatZero;
            mPostMultQs[ibone].z = kSingleQFloatZero;
            mPostMultQs[ibone].w = kSingleQFloatOne;

            if (minRangef.mIndex == 0) {
                eul[0] = kSingleQFloatZero;
                eul[1] = minRangef.mConst0;
                eul[2] = minRangef.mConst1;
                SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
            } else if (minRangef.mIndex == 1) {
                eul[0] = minRangef.mConst0;
                eul[1] = kSingleQFloatZero;
                eul[2] = kSingleQFloatZero;
                SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));

                eul[0] = kSingleQFloatZero;
                eul[1] = kSingleQFloatZero;
                eul[2] = minRangef.mConst1;
                SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPostMultQs[ibone]));
            } else {
                eul[0] = minRangef.mConst0;
                eul[1] = minRangef.mConst1;
                eul[2] = kSingleQFloatZero;
                SingleQEulToQuat(eul, reinterpret_cast<float *>(&mPreMultQs[ibone]));
            }
        }
    }
}

}; // namespace EAGL4Anim
