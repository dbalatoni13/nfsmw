#include "FnDeltaQFast.h"

#include "AnimTypeId.h"
#include "AnimUtil.h"


static void QuatMultXxYxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, const UMath::Vector4 &c, UMath::Vector4 &result);
static void QuatMultXxQ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
static void QuatMultQxZ(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);

namespace EAGL4Anim {

unsigned char *DeltaQFast::GetConstBoneIdx() {
    unsigned int numBones = mNumBones;
    unsigned int binLength = 1u << mBinLengthPower;
    unsigned int numBins = mNumKeys / binLength;
    int remainder = mNumKeys - numBins * binLength;
    int s = reinterpret_cast<int>(this) + 0x12;

    s += numBones * sizeof(DeltaQFastMinRange);
    s += AlignSize2(numBones * (((binLength - 1) * sizeof(DeltaQFastDelta)) + sizeof(DeltaQFastPhysical))) * numBins;

    if (remainder > 0) {
        s += numBones * (((remainder - 1) * sizeof(DeltaQFastDelta)) + sizeof(DeltaQFastPhysical));
    }

    return reinterpret_cast<unsigned char *>(s);
}

DeltaQFastPhysical *DeltaQFast::GetConstPhysical() {
    unsigned int numBones = mNumBones;
    unsigned int binLength = 1u << mBinLengthPower;
    unsigned int numBins = mNumKeys / binLength;
    int remainder = mNumKeys - numBins * binLength;
    int s = reinterpret_cast<int>(this) + 0x12;

    s += numBones * sizeof(DeltaQFastMinRange);
    s += AlignSize2(numBones * (((binLength - 1) * sizeof(DeltaQFastDelta)) + sizeof(DeltaQFastPhysical))) * numBins;

    if (remainder > 0) {
        s += numBones * (((remainder - 1) * sizeof(DeltaQFastDelta)) + sizeof(DeltaQFastPhysical));
    }

    return reinterpret_cast<DeltaQFastPhysical *>(AlignSize2(s + mNumConstBones));
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
        if (deltaQ->mNumKeys > floorTime) {
            return floorTime;
        }
        return deltaQ->mNumKeys - 1;
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
    if (mMinRangesf) {
        MemoryPoolManager::DeleteBlock(mMinRangesf);
    }
}

void FnDeltaQFast::SetAnimMemoryMap(AnimMemoryMap *anim) {
    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(anim);

    mpAnim = anim;
    unsigned char numBones = deltaQ->mNumBones;
    DeltaQFastMinRange *minRange = reinterpret_cast<DeltaQFastMinRange *>(reinterpret_cast<unsigned char *>(deltaQ) + 0x12);
    mBins = reinterpret_cast<unsigned char *>(minRange) + numBones * sizeof(DeltaQFastMinRange);
    mConstBoneIdxs = reinterpret_cast<unsigned char *>(deltaQ->GetConstBoneIdx());
    mConstPhysical = reinterpret_cast<DeltaQFastPhysical *>(deltaQ->GetConstPhysical());
    mBinSize = AlignSize2(deltaQ->mNumBones * (((1 << deltaQ->mBinLengthPower) - 1) * 3 + 6));

    if (deltaQ->mNumBones != 0) {
        unsigned int qBlockOffset = numBones << 5;
        unsigned int nextQBlockOffset = numBones << 4;
        unsigned char *block =
            reinterpret_cast<unsigned char *>(MemoryPoolManager::NewBlock(nextQBlockOffset + nextQBlockOffset + qBlockOffset));
        unsigned char *qBlock = block + qBlockOffset;
        unsigned char *nextQBlock = qBlock + nextQBlockOffset;

        mMinRangesf = reinterpret_cast<DeltaQFastMinRangef *>(block);
        mPrevQBlock = qBlock;
        mPrevQs = reinterpret_cast<UMath::Vector4 *>(qBlock);
        mNextQBlock = nextQBlock;
        mNextQs = reinterpret_cast<UMath::Vector4 *>(mNextQBlock);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            unsigned short *minRangeValues = reinterpret_cast<unsigned short *>(&minRange[ibone]);
            float *minRangefValues = reinterpret_cast<float *>(&mMinRangesf[ibone]);

            minRangefValues[0] = minRangeValues[0] * kQFastMinScale16 - 1.0f;
            minRangefValues[1] = minRangeValues[1] * kQFastMinScale16 - 1.0f;
            minRangefValues[2] = minRangeValues[2] * kQFastMinScale16 - 1.0f;
            minRangefValues[3] = minRangeValues[3] * kQFastMinScale16 - 1.0f;
            minRangefValues[4] = minRangeValues[4] * kQFastMinScale16;
            minRangefValues[5] = minRangeValues[5] * kQFastMinScale16;
            minRangefValues[6] = minRangeValues[6] * kQFastMinScale16;
            minRangefValues[7] = minRangeValues[7] * kQFastMinScale16;
        }
    }

    mBoneMask = nullptr;
    mNextKey = -1;
    mPrevKey = -1;
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

inline void FnDeltaQFast::AddDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx,
                                   UMath::Vector4 *prevQs) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
        DeltaQFastDelta *floorDelta = reinterpret_cast<DeltaQFastDelta *>(GetQFastDeltaData(deltaQ, binData, iframe));

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 delta;

            floorDelta->UnQuantize(mMinRangesf[ibone], delta);
            prevQs[ibone].x += delta.x;
            prevQs[ibone].y += delta.y;
            prevQs[ibone].z += delta.z;
            prevQs[ibone].w += delta.w;
            floorDelta++;
        }
    }
}

inline void FnDeltaQFast::SubDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx,
                                   UMath::Vector4 *prevQs) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);

    for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
        DeltaQFastDelta *floorDelta = reinterpret_cast<DeltaQFastDelta *>(GetQFastDeltaData(deltaQ, binData, iframe));

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            UMath::Vector4 delta;

            floorDelta->UnQuantize(mMinRangesf[ibone], delta);
            prevQs[ibone].x -= delta.x;
            prevQs[ibone].y -= delta.y;
            prevQs[ibone].z -= delta.z;
            prevQs[ibone].w -= delta.w;
            floorDelta++;
        }
    }
}

void FnDeltaQFast::UpdateNextQs(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx) {
    if (ceilKey == mNextKey) {
        return;
    }

    int ceilBinIdx = ceilKey >> deltaQ->mBinLengthPower;
    unsigned char *binData = &mBins[ceilBinIdx * mBinSize];

    if (ceilBinIdx != floorBinIdx) {
        unsigned char numBones = deltaQ->mNumBones;

        for (int ibone = 0; ibone < numBones; ibone++) {
            float *nextQ = reinterpret_cast<float *>(&mNextQs[ibone]);
            DeltaQFastPhysical *physical = reinterpret_cast<DeltaQFastPhysical *>(&binData[ibone * 6]);

            nextQ[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            nextQ[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            nextQ[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            nextQ[3] =
                static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) * kQFastPhysicalScale12 -
                kQFastPhysicalBias12;
        }
    } else {
        unsigned int numBones = deltaQ->mNumBones;
        int ceilDeltaIdx = floorDeltaIdx;
        DeltaQFastDelta *ceilDelta = reinterpret_cast<DeltaQFastDelta *>(&binData[numBones * 6 + ceilDeltaIdx * numBones * 3]);

        for (int ibone = 0; ibone < static_cast<int>(numBones); ibone++) {
            UMath::Vector4 ceilq;

            ceilDelta->UnQuantize(mMinRangesf[ibone], ceilq);
            mNextQs[ibone].x = ceilq.x + mPrevQs[ibone].x;
            mNextQs[ibone].y = ceilq.y + mPrevQs[ibone].y;
            mNextQs[ibone].z = ceilq.z + mPrevQs[ibone].z;
            mNextQs[ibone].w = ceilq.w + mPrevQs[ibone].w;
            ceilDelta++;
        }
    }

    mNextKey = ceilKey;
}

void FnDeltaQFast::AddDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, prevDeltaIdx);

    for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
        unsigned char numBones = deltaQ->mNumBones;

        for (int ibone = 0; ibone < numBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector4 delta;

                DecodeQFastDelta(mMinRangesf[ibone], deltaData, delta);
                prevQs[ibone].x += delta.x;
                prevQs[ibone].y += delta.y;
                prevQs[ibone].z += delta.z;
                prevQs[ibone].w += delta.w;
            }
            deltaData += 3;
        }
    }
}

void FnDeltaQFast::SubDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *binData = reinterpret_cast<unsigned char *>(floorPhys);
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    unsigned char *deltaData = GetQFastDeltaData(deltaQ, binData, prevDeltaIdx - 1) + (deltaQ->mNumBones - 1) * 3;

    for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
        unsigned char numBones = deltaQ->mNumBones;

        for (int ibone = numBones - 1; ibone >= 0; ibone--) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector4 delta;

                DecodeQFastDelta(mMinRangesf[ibone], deltaData, delta);
                prevQs[ibone].x -= delta.x;
                prevQs[ibone].y -= delta.y;
                prevQs[ibone].z -= delta.z;
                prevQs[ibone].w -= delta.w;
            }
            deltaData -= 3;
        }
    }
}

void FnDeltaQFast::UpdateNextQsMask(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx, const BoneMask *boneMask) {
    if (ceilKey == mNextKey) {
        return;
    }

    int ceilBinIdx = ceilKey >> deltaQ->mBinLengthPower;
    unsigned char *binData = &mBins[ceilBinIdx * mBinSize];
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (ceilBinIdx != floorBinIdx) {
        unsigned char numBones = deltaQ->mNumBones;
        DeltaQFastPhysical *physical = reinterpret_cast<DeltaQFastPhysical *>(binData);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];
            if (boneMask->GetBone(boneIdx)) {
                float *nextQ = reinterpret_cast<float *>(&mNextQs[ibone]);

                nextQ[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                nextQ[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                nextQ[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                nextQ[3] =
                    static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) *
                        kQFastPhysicalScale12 -
                    kQFastPhysicalBias12;
            }
            physical++;
        }
    } else {
        unsigned int numBones = deltaQ->mNumBones;
        DeltaQFastDelta *ceilDelta = reinterpret_cast<DeltaQFastDelta *>(&binData[numBones * 6 + floorDeltaIdx * numBones * 3]);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];
            if (boneMask->GetBone(boneIdx)) {
                UMath::Vector4 ceilq;

                ceilDelta->UnQuantize(mMinRangesf[ibone], ceilq);
                mNextQs[ibone].x = ceilq.x + mPrevQs[ibone].x;
                mNextQs[ibone].y = ceilq.y + mPrevQs[ibone].y;
                mNextQs[ibone].z = ceilq.z + mPrevQs[ibone].z;
                mNextQs[ibone].w = ceilq.w + mPrevQs[ibone].w;
            }
            ceilDelta++;
        }
    }

    mNextKey = ceilKey;
}

bool FnDeltaQFast::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!boneMask) {
        if (mBoneMask) {
            mBoneMask = boneMask;
            mPrevKey = -1;
            mNextKey = -1;
        }

        DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);
        float *quatBase = sqt + 4;
        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        if (deltaQ->mNumBones) {
            int floorTime = FloatToInt(currTime);
            unsigned short *times = deltaQ->mTimes;
            int floorKey = FindQFastFloorKey(mPrevKey, deltaQ, currTime);
            unsigned int floorDeltaIdx;
            int floorBinIdx;
            unsigned int binLengthMask;

        floorBinIdx = static_cast<int>(floorKey) >> deltaQ->mBinLengthPower;
        binLengthMask = 0x7FFFFFFFU >> (0x1F - deltaQ->mBinLengthPower);
        floorDeltaIdx = floorKey & binLengthMask;

        if (mNextKey == floorKey) {
            UMath::Vector4 *swapQs = mNextQs;

            mNextQs = mPrevQs;
            mPrevQs = swapQs;
            mNextKey = mPrevKey;
        } else {
            int binData = reinterpret_cast<int>(mBins) + floorBinIdx * mBinSize;
            int prevDeltaIdx = mPrevKey;

            if (mPrevKey == static_cast<int>(floorKey + 1)) {
                for (int ibone = 0; ibone < static_cast<int>(deltaQ->mNumBones); ibone++) {
                    mPrevQs[ibone] = mNextQs[ibone];
                }

                mNextKey = mPrevKey;
            }

                    if (prevDeltaIdx == -1 || floorBinIdx != (mPrevKey >> deltaQ->mBinLengthPower) || floorDeltaIdx == 0 ||
                (floorKey < mPrevKey && !IsReverseDeltaSumEnabled())) {
                    for (int ibone = 0; ibone < static_cast<int>(deltaQ->mNumBones); ibone++) {
                        float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                        DeltaQFastPhysical *physical = reinterpret_cast<DeltaQFastPhysical *>(binData + ibone * 6);

                        prevQ[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                        prevQ[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                        prevQ[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
                        prevQ[3] =
                            static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) *
                                kQFastPhysicalScale12 -
                            kQFastPhysicalBias12;
                    }

                    prevDeltaIdx = 0;
                } else {
                    prevDeltaIdx &= static_cast<int>(binLengthMask);
                }

                if (prevDeltaIdx < static_cast<int>(floorDeltaIdx)) {
                    AddDelta(reinterpret_cast<DeltaQFastPhysical *>(binData), deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);
                } else if (static_cast<int>(floorDeltaIdx) < prevDeltaIdx) {
                    SubDelta(reinterpret_cast<DeltaQFastPhysical *>(binData), deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);
                }
            }

            mPrevKey = floorKey;
            times = deltaQ->mTimes;
            bool slerpReqd = false;
            float t = 0.0f;

            if (!times) {
                float floorTimef = static_cast<float>(floorTime);

                slerpReqd = currTime != floorTimef;
                if (slerpReqd) {
                    t = currTime - floorTimef;
                }
            } else if (floorKey == 0) {
                slerpReqd = currTime != 0.0f;
                if (slerpReqd) {
                    t = currTime / static_cast<float>(times[0]);
                }
            } else {
                float floorTimef = static_cast<float>(times[floorKey - 1]);

                slerpReqd = currTime != floorTimef;
                if (slerpReqd) {
                    t = (currTime - floorTimef) / (static_cast<float>(times[floorKey]) - floorTimef);
                }
            }

            if (slerpReqd && static_cast<int>(floorKey) < deltaQ->mNumKeys - 1) {
                UpdateNextQs(deltaQ, floorKey + 1, floorBinIdx, floorDeltaIdx);

                for (int ibone = 0; ibone < static_cast<int>(deltaQ->mNumBones); ibone++) {
                    float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                    float *nextQ = reinterpret_cast<float *>(&mNextQs[ibone]);
                    float x = t * (nextQ[0] - prevQ[0]) + prevQ[0];
                    unsigned char boneIdx = boneIdxs[ibone];
                    float y = t * (nextQ[1] - prevQ[1]) + prevQ[1];
                    float z = t * (nextQ[2] - prevQ[2]) + prevQ[2];
                    float w = t * (nextQ[3] - prevQ[3]) + prevQ[3];
                    float *out = reinterpret_cast<float *>(boneIdx * 0x30 + reinterpret_cast<int>(quatBase));
                    float invNorm = 1.0f / sqrtf(x * x + y * y + z * z + w * w);

                    out[0] = x * invNorm;
                    out[1] = y * invNorm;
                    out[2] = z * invNorm;
                    out[3] = w * invNorm;
                }

                goto finish_const_bones;
            }

            for (int ibone = 0; ibone < static_cast<int>(deltaQ->mNumBones); ibone++) {
                float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
                float *out = reinterpret_cast<float *>(boneIdxs[ibone] * 0x30 + reinterpret_cast<int>(quatBase));

                out[0] = prevQ[0];
                out[1] = prevQ[1];
                out[2] = prevQ[2];
                out[3] = prevQ[3];
            }
        }

    finish_const_bones:
        for (int ibone = 0; ibone < static_cast<int>(deltaQ->mNumConstBones); ibone++) {
            DeltaQFastPhysical *physical = reinterpret_cast<DeltaQFastPhysical *>(reinterpret_cast<unsigned char *>(mConstPhysical) + ibone * 6);
            float *out = reinterpret_cast<float *>(mConstBoneIdxs[ibone] * 0x30 + reinterpret_cast<int>(quatBase));

            out[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            out[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            out[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            out[3] = static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) *
                         kQFastPhysicalScale12 -
                     kQFastPhysicalBias12;
        }

        return true;
    }
    return EvalSQTMask(currTime, sqt, boneMask);
}

bool FnDeltaQFast::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);
    float *quatBase = sqt + 4;
    unsigned char numBones = deltaQ->mNumBones;
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (!numBones) {
        return true;
    }

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
            if (!boneMask->GetBone(boneIdxs[ibone])) {
                continue;
            }

            DeltaQFastPhysical *physical = &floorPhys[ibone];
            float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);

            prevQ[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            prevQ[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            prevQ[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
            prevQ[3] = static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) *
                       kQFastPhysicalScale12 -
                       kQFastPhysicalBias12;
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
    bool slerpReqd = false;
    float t = 0.0f;

    if (ceilKey >= deltaQ->mNumKeys) {
        ceilKey = floorKey;
    }

    if (!deltaQ->mTimes) {
        float floorTimef = static_cast<float>(floorKey);

        slerpReqd = currTime != floorTimef;
        if (slerpReqd) {
            t = currTime - floorTimef;
        }
    } else if (floorKey == 0) {
        slerpReqd = currTime != 0.0f;
        if (slerpReqd) {
            t = currTime / static_cast<float>(deltaQ->mTimes[0]);
        }
    } else {
        float floorTimef = static_cast<float>(deltaQ->mTimes[floorKey - 1]);

        slerpReqd = currTime != floorTimef;
        if (slerpReqd) {
            t = (currTime - floorTimef) / (static_cast<float>(deltaQ->mTimes[floorKey]) - floorTimef);
        }
    }

    if (slerpReqd && ceilKey > floorKey) {
        UpdateNextQsMask(deltaQ, ceilKey, floorBinIdx, floorDeltaIdx, boneMask);

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];

            if (!boneMask->GetBone(boneIdx)) {
                continue;
            }

            float *prevQ = reinterpret_cast<float *>(&mPrevQs[ibone]);
            float *nextQ = reinterpret_cast<float *>(&mNextQs[ibone]);
            float *out = &quatBase[boneIdx * 12];
            float x = t * (nextQ[0] - prevQ[0]) + prevQ[0];
            float y = t * (nextQ[1] - prevQ[1]) + prevQ[1];
            float z = t * (nextQ[2] - prevQ[2]) + prevQ[2];
            float w = t * (nextQ[3] - prevQ[3]) + prevQ[3];
            float invNorm = 1.0f / sqrtf(x * x + y * y + z * z + w * w);

            out[0] = x * invNorm;
            out[1] = y * invNorm;
            out[2] = z * invNorm;
            out[3] = w * invNorm;
        }
    } else {
        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];

            if (!boneMask->GetBone(boneIdx)) {
                continue;
            }

            float *out = &quatBase[boneIdx * 12];

            out[0] = mPrevQs[ibone].x;
            out[1] = mPrevQs[ibone].y;
            out[2] = mPrevQs[ibone].z;
            out[3] = mPrevQs[ibone].w;
        }
    }

    for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {
        unsigned char boneIdx = mConstBoneIdxs[ibone];

        if (!boneMask->GetBone(boneIdx)) {
            continue;
        }

        DeltaQFastPhysical *physical = &mConstPhysical[ibone];
        float *out = &quatBase[boneIdx * 12];

        out[0] = static_cast<float>(physical->mX) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
        out[1] = static_cast<float>(physical->mY) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
        out[2] = static_cast<float>(physical->mZ) * kQFastPhysicalScale12 - kQFastPhysicalBias12;
        out[3] = static_cast<float>(static_cast<unsigned short>((physical->mW0 << 8) | (physical->mW1 << 4) | physical->mW2)) *
                     kQFastPhysicalScale12 -
                 kQFastPhysicalBias12;
    }

    return true;
}

}; // namespace EAGL4Anim
