#include "FnDeltaQFast.h"

#include "AnimTypeId.h"
#include "AnimUtil.h"


void QuatMultXxYxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, const UMath::Vector4 &c, UMath::Vector4 &result) {
    float awby = a.w * b.y;
    float axbw = a.x * b.w;
    float awbw = a.w * b.w;
    float naxby = -(a.x * b.y);

    result.x = axbw * c.w - awby * c.z;
    result.y = axbw * c.z + awby * c.w;
    result.z = naxby * c.w + awbw * c.z;
    result.w = -naxby * c.z + awbw * c.w;
}

void QuatMultXxQ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = a.w * b.x + a.x * b.w;
    result.y = a.w * b.y + a.x * b.z;
    result.z = -(a.x * b.y) + a.w * b.z;
    result.w = -(a.x * b.x) + a.w * b.w;
}

void QuatMultQxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = a.x * b.w - a.y * b.z;
    result.y = a.x * b.z + a.y * b.w;
    result.z = a.z * b.w + a.w * b.z;
    result.w = -(a.z * b.z) + a.w * b.w;
}

namespace EAGL4Anim {

unsigned short *DeltaQFast::GetConstBoneIdx() {
    const int binSize = GetBinSize();
    int numFrames = GetNumFrames();
    int numBins = numFrames >> GetBinLengthPower();
    unsigned char *s = &GetBin(0)[binSize * numBins];
    int r = numFrames & GetBinLengthModMask();

    if (r > 0) {
        s = reinterpret_cast<unsigned char *>(AlignSize2(reinterpret_cast<intptr_t>(s + mNumBones * 6 + ((r - 1) * mNumBones * 3))));
    }

    return reinterpret_cast<unsigned short *>(s);
}

void *DeltaQFast::GetConstPhysical() {
    return reinterpret_cast<void *>(AlignSize2(reinterpret_cast<intptr_t>(&GetConstBoneIdx()[mNumConstBones])));
}

namespace {

static const float kQFastMinScale16 = 3.0518044e-5f;
static const float kQFastDeltaScale6 = 1.5873017e-2f;
static const float kQFastPhysicalScale12 = 4.8840049e-4f;
static const float kQFastPhysicalBias12 = 1.0002443f;

static inline float *GetQFastOutputQuat(float *sqt, unsigned char boneIdx) {
    return &sqt[boneIdx * 12 + 4];
}

static inline unsigned char *GetQFastBin(unsigned char *bins, int binSize, int binIdx) {
    return &bins[binIdx * binSize];
}

static inline DeltaQFastPhysical *GetQFastPhysical(unsigned char *binData) {
    return reinterpret_cast<DeltaQFastPhysical *>(binData);
}

static inline unsigned char *GetQFastDeltaData(DeltaQFast *deltaQ, unsigned char *binData, int deltaIdx) {
    return &binData[deltaQ->mNumBones * 6 + deltaIdx * deltaQ->mNumBones * 3];
}

static inline void DecodeQFastPhysical(const DeltaQFastPhysical &physical, UMath::Vector4 &q) {
    unsigned short xBits = physical.mX;
    unsigned short yBits = physical.mY;
    unsigned short zBits = physical.mZ;
    unsigned short wBits = static_cast<unsigned short>((physical.mW0 << 8) | (physical.mW1 << 4) | physical.mW2);

    q.x = xBits * kQFastPhysicalScale12 - kQFastPhysicalBias12;
    q.y = yBits * kQFastPhysicalScale12 - kQFastPhysicalBias12;
    q.z = zBits * kQFastPhysicalScale12 - kQFastPhysicalBias12;
    q.w = wBits * kQFastPhysicalScale12 - kQFastPhysicalBias12;
}

static inline void DecodeQFastDelta(const DeltaQFastMinRangef &minRangef, const unsigned char *deltaData, UMath::Vector4 &q) {
    unsigned int xBits = deltaData[0] >> 2;
    unsigned int yBits = deltaData[1] >> 2;
    unsigned int zBits = deltaData[2] >> 2;
    unsigned int wBits = ((deltaData[0] & 3) << 4) | ((deltaData[1] & 3) << 2) | (deltaData[2] & 3);

    q.x = minRangef.mMin.x + minRangef.mRange.x * (xBits * kQFastDeltaScale6);
    q.y = minRangef.mMin.y + minRangef.mRange.y * (yBits * kQFastDeltaScale6);
    q.z = minRangef.mMin.z + minRangef.mRange.z * (zBits * kQFastDeltaScale6);
    q.w = minRangef.mMin.w + minRangef.mRange.w * (wBits * kQFastDeltaScale6);
}

static inline int FindQFastFloorKey(int prevKey, DeltaQFast *deltaQ, float currTime) {
    int floorTime = FloatToInt(currTime);

    if (!deltaQ->mTimes) {
        if (floorTime < 0) {
            return 0;
        }
        if (floorTime >= deltaQ->mNumKeys) {
            return deltaQ->mNumKeys - 1;
        }
        return floorTime;
    }
    if (floorTime < deltaQ->mTimes[0]) {
        return 0;
    }

    int timeIndex = prevKey < 1 ? 0 : prevKey - 1;

    if (deltaQ->mTimes[timeIndex] <= floorTime) {
        while (timeIndex < deltaQ->mNumKeys - 2 && deltaQ->mTimes[timeIndex + 1] <= floorTime) {
            timeIndex++;
        }
    } else {
        while (timeIndex > 0 && deltaQ->mTimes[timeIndex] > floorTime) {
            timeIndex--;
        }
    }

    return timeIndex + 1;
}

static inline float ComputeQFastBlendT(DeltaQFast *deltaQ, int floorKey, int ceilKey, float currTime) {
    if (ceilKey <= floorKey) {
        return 0.0f;
    }
    if (!deltaQ->mTimes) {
        return currTime - static_cast<float>(floorKey);
    }

    float floorTime = static_cast<float>(deltaQ->mTimes[floorKey - 1]);
    float ceilTime = static_cast<float>(deltaQ->mTimes[ceilKey - 1]);

    if (floorKey == 0) {
        floorTime = 0.0f;
    }
    if (ceilTime == floorTime) {
        return 0.0f;
    }

    return (currTime - floorTime) / (ceilTime - floorTime);
}

} // namespace

FnDeltaQFast::FnDeltaQFast()
    : mMinRangesf(nullptr),    //
      mBins(nullptr),          //
      mBinSize(-1),            //
      mPrevKey(-1),            //
      mPrevQBlock(nullptr),    //
      mPrevQs(nullptr),        //
      mNextKey(-1),            //
      mNextQBlock(nullptr),    //
      mNextQs(nullptr),        //
      mConstBoneIdxs(nullptr), //
      mConstPhysical(nullptr), //
      mBoneMask(nullptr) {
    mType = AnimTypeId::ANIM_DELTAQFAST;
}

FnDeltaQFast::~FnDeltaQFast() {
    if (mPrevQBlock) {
        MemoryPoolManager::DeleteBlock(mPrevQBlock);
    }
}

void FnDeltaQFast::SetAnimMemoryMap(AnimMemoryMap *anim) {
    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(anim);
    int numBones = deltaQ->mNumBones;

    mpAnim = anim;
    mBins = reinterpret_cast<unsigned char *>(deltaQ->GetMinRange()) + numBones * sizeof(DeltaQFastMinRange);
    mConstBoneIdxs = reinterpret_cast<unsigned char *>(deltaQ->GetConstBoneIdx());
    mConstPhysical = reinterpret_cast<DeltaQFastPhysical *>(deltaQ->GetConstPhysical());
    mBinSize = AlignSize2(numBones * (6 + ((deltaQ->GetBinLength() - 1) * 3)));
    mPrevKey = -1;
    mNextKey = -1;
    mBoneMask = nullptr;

    if (numBones != 0) {
        unsigned char *block = reinterpret_cast<unsigned char *>(MemoryPoolManager::NewBlock(numBones * 0x40));
        DeltaQFastMinRange *minRange = deltaQ->GetMinRange();

        mMinRangesf = reinterpret_cast<DeltaQFastMinRangef *>(block);
        mPrevQBlock = block + numBones * sizeof(DeltaQFastMinRangef);
        mPrevQs = reinterpret_cast<UMath::Vector4 *>(mPrevQBlock);
        mNextQBlock = reinterpret_cast<unsigned char *>(mPrevQs) + numBones * sizeof(*mPrevQs);
        mNextQs = reinterpret_cast<UMath::Vector4 *>(mNextQBlock);

        for (int ibone = 0; ibone < numBones; ibone++) {
            mMinRangesf[ibone].mMin.x = minRange[ibone].mMin[0] * kQFastMinScale16 - 1.0f;
            mMinRangesf[ibone].mMin.y = minRange[ibone].mMin[1] * kQFastMinScale16 - 1.0f;
            mMinRangesf[ibone].mMin.z = minRange[ibone].mMin[2] * kQFastMinScale16 - 1.0f;
            mMinRangesf[ibone].mMin.w = minRange[ibone].mMin[3] * kQFastMinScale16 - 1.0f;
            mMinRangesf[ibone].mRange.x = minRange[ibone].mRange[0] * kQFastMinScale16;
            mMinRangesf[ibone].mRange.y = minRange[ibone].mRange[1] * kQFastMinScale16;
            mMinRangesf[ibone].mRange.z = minRange[ibone].mRange[2] * kQFastMinScale16;
            mMinRangesf[ibone].mRange.w = minRange[ibone].mRange[3] * kQFastMinScale16;
        }
    }
}

bool FnDeltaQFast::GetLength(float &timeLength) const {
    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);

    timeLength = static_cast<float>(deltaQ->GetNumFrames());
    return true;
}

void FnDeltaQFast::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

void FnDeltaQFast::InitBuffers() {
    if (mpAnim) {
        SetAnimMemoryMap(mpAnim);
    }
}

void FnDeltaQFast::AddDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, iframe);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            prevQs[ibone].x += delta.x;
            prevQs[ibone].y += delta.y;
            prevQs[ibone].z += delta.z;
            prevQs[ibone].w += delta.w;
        }
    }
}

void FnDeltaQFast::SubDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, iframe);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            prevQs[ibone].x -= delta.x;
            prevQs[ibone].y -= delta.y;
            prevQs[ibone].z -= delta.z;
            prevQs[ibone].w -= delta.w;
        }
    }
}

void FnDeltaQFast::UpdateNextQs(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx) {
    if (ceilKey == mNextKey) {
        return;
    }

    int ceilBinIdx = ceilKey >> deltaQ->GetBinLengthPower();
    unsigned char *binData = GetQFastBin(mBins, mBinSize, ceilBinIdx);

    if (ceilBinIdx == floorBinIdx) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, floorDeltaIdx);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            mNextQs[ibone].x = mPrevQs[ibone].x + delta.x;
            mNextQs[ibone].y = mPrevQs[ibone].y + delta.y;
            mNextQs[ibone].z = mPrevQs[ibone].z + delta.z;
            mNextQs[ibone].w = mPrevQs[ibone].w + delta.w;
        }
    } else {
        DeltaQFastPhysical *physical = GetQFastPhysical(binData);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DecodeQFastPhysical(physical[ibone], mNextQs[ibone]);
        }
    }

    mNextKey = ceilKey;
}

void FnDeltaQFast::AddDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, iframe);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            prevQs[ibone].x += delta.x;
            prevQs[ibone].y += delta.y;
            prevQs[ibone].z += delta.z;
            prevQs[ibone].w += delta.w;
        }
    }
}

void FnDeltaQFast::SubDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, iframe);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            prevQs[ibone].x -= delta.x;
            prevQs[ibone].y -= delta.y;
            prevQs[ibone].z -= delta.z;
            prevQs[ibone].w -= delta.w;
        }
    }
}

void FnDeltaQFast::UpdateNextQsMask(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx, const BoneMask *boneMask) {
    if (ceilKey == mNextKey) {
        return;
    }

    int ceilBinIdx = ceilKey >> deltaQ->GetBinLengthPower();
    unsigned char *binData = GetQFastBin(mBins, mBinSize, ceilBinIdx);

    if (ceilBinIdx == floorBinIdx) {
        unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, floorDeltaIdx);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            UMath::Vector4 delta;

            DecodeQFastDelta(mMinRangesf[ibone], &deltaData[ibone * 3], delta);
            mNextQs[ibone].x = mPrevQs[ibone].x + delta.x;
            mNextQs[ibone].y = mPrevQs[ibone].y + delta.y;
            mNextQs[ibone].z = mPrevQs[ibone].z + delta.z;
            mNextQs[ibone].w = mPrevQs[ibone].w + delta.w;
        }
    } else {
        DeltaQFastPhysical *physical = GetQFastPhysical(binData);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            DecodeQFastPhysical(physical[ibone], mNextQs[ibone]);
        }
    }

    mNextKey = ceilKey;
}

bool FnDeltaQFast::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!mpAnim) {
        return false;
    }
    if (!mMinRangesf && reinterpret_cast<DeltaQFast *>(mpAnim)->mNumBones) {
        InitBuffers();
    }
    if (boneMask) {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    if (mBoneMask) {
        mBoneMask = nullptr;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);
    int floorKey = FindQFastFloorKey(mPrevKey, deltaQ, currTime);
    int floorBinIdx = floorKey >> deltaQ->GetBinLengthPower();
    int floorDeltaIdx = floorKey & deltaQ->GetBinLengthModMask();
    int prevBinIdx = mPrevKey >> deltaQ->GetBinLengthPower();
    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();
    unsigned char *binData = GetQFastBin(mBins, mBinSize, floorBinIdx);
    DeltaQFastPhysical *floorPhys = GetQFastPhysical(binData);
    int prevDeltaIdx;

    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            DecodeQFastPhysical(floorPhys[ibone], mPrevQs[ibone]);
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & deltaQ->GetBinLengthModMask();
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        AddDelta(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);
    } else if (prevDeltaIdx > floorDeltaIdx) {
        SubDelta(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);
    }

    int ceilKey = floorKey + 1;

    if (ceilKey >= deltaQ->mNumKeys) {
        ceilKey = floorKey;
    }

    if (ceilKey > floorKey) {
        UpdateNextQs(deltaQ, ceilKey, floorBinIdx, floorDeltaIdx);
        float t = ComputeQFastBlendT(deltaQ, floorKey, ceilKey, currTime);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            FastQuatBlendF4(t, reinterpret_cast<float *>(&mPrevQs[ibone]), reinterpret_cast<float *>(&mNextQs[ibone]),
                            GetQFastOutputQuat(sqt, deltaQ->mBoneIdxs[ibone]));
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            float *out = GetQFastOutputQuat(sqt, deltaQ->mBoneIdxs[ibone]);

            out[0] = mPrevQs[ibone].x;
            out[1] = mPrevQs[ibone].y;
            out[2] = mPrevQs[ibone].z;
            out[3] = mPrevQs[ibone].w;
        }
    }

    for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {
        UMath::Vector4 q;
        float *out = GetQFastOutputQuat(sqt, mConstBoneIdxs[ibone]);

        DecodeQFastPhysical(mConstPhysical[ibone], q);
        out[0] = q.x;
        out[1] = q.y;
        out[2] = q.z;
        out[3] = q.w;
    }

    mPrevKey = floorKey;
    return true;
}

bool FnDeltaQFast::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!mpAnim) {
        return false;
    }
    if (!mMinRangesf && reinterpret_cast<DeltaQFast *>(mpAnim)->mNumBones) {
        InitBuffers();
    }
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);
    int floorKey = FindQFastFloorKey(mPrevKey, deltaQ, currTime);
    int floorBinIdx = floorKey >> deltaQ->GetBinLengthPower();
    int floorDeltaIdx = floorKey & deltaQ->GetBinLengthModMask();
    int prevBinIdx = mPrevKey >> deltaQ->GetBinLengthPower();
    bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();
    unsigned char *binData = GetQFastBin(mBins, mBinSize, floorBinIdx);
    DeltaQFastPhysical *floorPhys = GetQFastPhysical(binData);
    int prevDeltaIdx;

    if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            DecodeQFastPhysical(floorPhys[ibone], mPrevQs[ibone]);
        }
        prevDeltaIdx = 0;
    } else {
        prevDeltaIdx = mPrevKey & deltaQ->GetBinLengthModMask();
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        AddDeltaMask(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs, boneMask);
    } else if (prevDeltaIdx > floorDeltaIdx) {
        SubDeltaMask(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs, boneMask);
    }

    int ceilKey = floorKey + 1;

    if (ceilKey >= deltaQ->mNumKeys) {
        ceilKey = floorKey;
    }

    if (ceilKey > floorKey) {
        UpdateNextQsMask(deltaQ, ceilKey, floorBinIdx, floorDeltaIdx, boneMask);
        float t = ComputeQFastBlendT(deltaQ, floorKey, ceilKey, currTime);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            FastQuatBlendF4(t, reinterpret_cast<float *>(&mPrevQs[ibone]), reinterpret_cast<float *>(&mNextQs[ibone]),
                            GetQFastOutputQuat(sqt, deltaQ->mBoneIdxs[ibone]));
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            if (!boneMask->GetBone(deltaQ->mBoneIdxs[ibone])) {
                continue;
            }

            float *out = GetQFastOutputQuat(sqt, deltaQ->mBoneIdxs[ibone]);

            out[0] = mPrevQs[ibone].x;
            out[1] = mPrevQs[ibone].y;
            out[2] = mPrevQs[ibone].z;
            out[3] = mPrevQs[ibone].w;
        }
    }

    for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {
        if (!boneMask->GetBone(mConstBoneIdxs[ibone])) {
            continue;
        }

        UMath::Vector4 q;
        float *out = GetQFastOutputQuat(sqt, mConstBoneIdxs[ibone]);

        DecodeQFastPhysical(mConstPhysical[ibone], q);
        out[0] = q.x;
        out[1] = q.y;
        out[2] = q.z;
        out[3] = q.w;
    }

    mPrevKey = floorKey;
    return true;
}

}; // namespace EAGL4Anim
