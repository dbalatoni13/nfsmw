#include "FnDeltaQFast.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"
#include "Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h"

namespace EAGL4Anim {

FnDeltaQFast::FnDeltaQFast()
    : mNextKey(-1),            //
      mBoneMask(nullptr),      //
      mMinRangesf(nullptr),    //
      mBins(nullptr),          //
      mBinSize(-1),            //
      mPrevKey(-1),            //
      mPrevQBlock(nullptr),    //
      mPrevQs(nullptr),        //
      mNextQBlock(nullptr),    //
      mNextQs(nullptr),        //
      mConstBoneIdxs(nullptr), //
      mConstPhysical(nullptr) {
    mType = AnimTypeId::ANIM_DELTAQFAST;
}

inline void FnDeltaQFast::InitBuffers() {
    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);
    DeltaQFastMinRange *minRanges;

    deltaQ->GetArrays(minRanges, mBins, mConstBoneIdxs, mConstPhysical);
    mBinSize = deltaQ->GetBinSize();

    if (deltaQ->mNumBones > 0) {
        int qSize = deltaQ->mNumBones * sizeof(UMath::Vector4);
        int mrSize = deltaQ->mNumBones * sizeof(DeltaQFastMinRangef);

        void *block = MemoryPoolManager::NewBlock(qSize + qSize + mrSize);
        mMinRangesf = reinterpret_cast<DeltaQFastMinRangef *>(block);

        mPrevQBlock = &reinterpret_cast<unsigned char *>(block)[mrSize];
        mPrevQs = reinterpret_cast<UMath::Vector4 *>(mPrevQBlock);

        mNextQBlock = &reinterpret_cast<unsigned char *>(mPrevQBlock)[qSize];
        mNextQs = reinterpret_cast<UMath::Vector4 *>(mNextQBlock);

        for (int i = 0; i < deltaQ->mNumBones; i++) {
            minRanges[i].UnQuantize(mMinRangesf[i]);
        }
    }
}

FnDeltaQFast::~FnDeltaQFast() {
    if (mMinRangesf) {
        MemoryPoolManager::DeleteBlock(mMinRangesf);
    }
}

void FnDeltaQFast::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;

    InitBuffers();
}

bool FnDeltaQFast::GetLength(float &timeLength) const {
    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);

    timeLength = static_cast<float>(deltaQ->GetNumFrames());
    return true;
}

void FnDeltaQFast::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

inline void FnDeltaQFast::AddDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs) {
    DeltaQFastDelta *floorDelta = &reinterpret_cast<DeltaQFastDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones];

    for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {

        for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

            UMath::Vector4 deltaf;

            deltaf.x = mMinRangesf[ibone].mRange.x * DeltaQFastDelta::DeQuantize(floorDelta->mX) + mMinRangesf[ibone].mMin.x;
            deltaf.y = mMinRangesf[ibone].mRange.y * DeltaQFastDelta::DeQuantize(floorDelta->mY) + mMinRangesf[ibone].mMin.y;
            deltaf.z = mMinRangesf[ibone].mRange.z * DeltaQFastDelta::DeQuantize(floorDelta->mZ) + mMinRangesf[ibone].mMin.z;

            unsigned char w = (floorDelta->mW0 << 4) + (floorDelta->mW1 << 2) | floorDelta->mW2;
            deltaf.w = mMinRangesf[ibone].mRange.w * DeltaQFastDelta::DeQuantize(w) + mMinRangesf[ibone].mMin.w;

            prevQs[ibone].x += deltaf.x;
            prevQs[ibone].y += deltaf.y;
            prevQs[ibone].z += deltaf.z;
            prevQs[ibone].w += deltaf.w;

            floorDelta++;
        }
    }
}

inline void FnDeltaQFast::SubDelta(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs) {
    DeltaQFastDelta *floorDelta = &reinterpret_cast<DeltaQFastDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones - 1];

    for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {

        for (int ibone = deltaQ->mNumBones - 1; ibone >= 0; ibone--) {

            UMath::Vector4 deltaf;

            deltaf.x = mMinRangesf[ibone].mRange.x * DeltaQFastDelta::DeQuantize(floorDelta->mX) + mMinRangesf[ibone].mMin.x;
            deltaf.y = mMinRangesf[ibone].mRange.y * DeltaQFastDelta::DeQuantize(floorDelta->mY) + mMinRangesf[ibone].mMin.y;
            deltaf.z = mMinRangesf[ibone].mRange.z * DeltaQFastDelta::DeQuantize(floorDelta->mZ) + mMinRangesf[ibone].mMin.z;

            unsigned char w = (floorDelta->mW0 << 4) + (floorDelta->mW1 << 2) | floorDelta->mW2;
            deltaf.w = mMinRangesf[ibone].mRange.w * DeltaQFastDelta::DeQuantize(w) + mMinRangesf[ibone].mMin.w;

            prevQs[ibone].x -= deltaf.x;
            prevQs[ibone].y -= deltaf.y;
            prevQs[ibone].z -= deltaf.z;
            prevQs[ibone].w -= deltaf.w;

            floorDelta--;
        }
    }
}

bool FnDeltaQFast::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {

    if (!boneMask) {

        if (mBoneMask) {

            mPrevKey = -1;
            mNextKey = -1;
            mBoneMask = boneMask;
        }

        DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);

        UMath::Vector4 *quats = &reinterpret_cast<UMath::Vector4 *>(sqt)[1];

        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        if (deltaQ->mNumBones > 0) {

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
                int i;

                if (mPrevKey <= 0) {

                    i = 0;

                } else {

                    i = mPrevKey - 1;
                }

                if (deltaQ->mTimes[i] <= floorTime) {

                    while (i < deltaQ->mNumKeys - 2 && deltaQ->mTimes[i + 1] <= floorTime) {

                        i++;
                    }
                } else {

                    while (i > 0 && deltaQ->mTimes[i] > floorTime) {

                        i--;
                    }
                }

                floorKey = i + 1;
            }

            const unsigned int binLenPower = deltaQ->GetBinLengthPower();
            const unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

            int floorBinIdx = floorKey >> binLenPower;
            int floorDeltaIdx = floorKey & binLenModMask;
            int prevBinIdx = mPrevKey >> binLenPower;
            int prevDeltaIdx;

            int ceilKey = floorKey + 1;

            if (mNextKey == floorKey) {

                UMath::Vector4 *swapQs = mNextQs;
                mNextQs = mPrevQs;
                mPrevQs = swapQs;
                mNextKey = mPrevKey;

            } else {

                if (mPrevKey == ceilKey) {

                    for (int b = 0; b < deltaQ->mNumBones; b++) {
                        mNextQs[b] = mPrevQs[b];
                    }
                    mNextKey = mPrevKey;
                }

                DeltaQFastPhysical *floorPhys = deltaQ->GetPhysical(&mBins[floorBinIdx * mBinSize]);

                bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();

                if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {

                    for (int b = 0; b < deltaQ->mNumBones; b++) {

                        floorPhys[b].UnQuantize(mPrevQs[b]);
                    }

                    prevDeltaIdx = 0;

                } else {

                    if (floorKey < mPrevKey) {
                        prevDeltaIdx = mPrevKey & binLenModMask;
                    } else {
                        prevDeltaIdx = mPrevKey & binLenModMask;
                    }
                }

                if (prevDeltaIdx < floorDeltaIdx) {

                    AddDelta(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);

                } else if (prevDeltaIdx > floorDeltaIdx) {

                    SubDelta(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs);
                }
            }

            mPrevKey = floorKey;

            float scale;
            bool lerpReqd;

            if (!deltaQ->mTimes) {
                lerpReqd = currTime != floorTime;
                if (lerpReqd) {
                    scale = currTime - floorTime;
                }
            } else if (floorKey == 0) {
                lerpReqd = currTime != 0.0f;
                if (lerpReqd) {
                    float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
                    scale = currTime / ceilKeyTime;
                }
            } else {
                float floorKeyTime = deltaQ->mTimes[floorKey - 1];
                lerpReqd = currTime != floorKeyTime;
                if (lerpReqd) {
                    float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
                    scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
                }
            }

            if (lerpReqd && floorKey < deltaQ->mNumKeys - 1) {

                UpdateNextQs(deltaQ, ceilKey, floorBinIdx, floorDeltaIdx);

                for (int b = 0; b < deltaQ->mNumBones; b++) {

                    FastPolarizedQuatBlend(scale, mPrevQs[b], mNextQs[b], quats[boneIdxs[b] * 3]);
                }
            } else {

                for (int b = 0; b < deltaQ->mNumBones; b++) {

                    quats[boneIdxs[b] * 3] = mPrevQs[b];
                }
            }
        }

        if (deltaQ->mNumConstBones > 0) {

            for (int c = 0; c < deltaQ->mNumConstBones; c++) {

                mConstPhysical[c].UnQuantize(quats[mConstBoneIdxs[c] * 3]);
            }
        }

        return true;
    }

    return EvalSQTMask(currTime, sqt, boneMask);
}

void FnDeltaQFast::UpdateNextQs(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx) {
    if (ceilKey != mNextKey) {

        int ceilBinIdx = ceilKey >> deltaQ->GetBinLengthPower();

        unsigned char *bin = &mBins[ceilBinIdx * mBinSize];

        if (ceilBinIdx != floorBinIdx) {

            for (int b = 0; b < deltaQ->mNumBones; b++) {
                deltaQ->GetPhysical(bin)[b].UnQuantize(mNextQs[b]);
            }
        } else {
            DeltaQFastDelta *delta =
                &reinterpret_cast<DeltaQFastDelta *>(&deltaQ->GetPhysical(bin)[deltaQ->mNumBones])[floorDeltaIdx * deltaQ->mNumBones];

            for (int b = 0; b < deltaQ->mNumBones; b++) {
                UMath::Vector4 q;

                delta->UnQuantize(mMinRangesf[b], q);

                mNextQs[b].x = q.x + mPrevQs[b].x;
                mNextQs[b].y = q.y + mPrevQs[b].y;
                mNextQs[b].z = q.z + mPrevQs[b].z;
                mNextQs[b].w = q.w + mPrevQs[b].w;

                delta++;
            }
        }

        mNextKey = ceilKey;
    }
}

bool FnDeltaQFast::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {

    if (boneMask != mBoneMask) {

        mPrevKey = -1;
        mNextKey = -1;
        mBoneMask = boneMask;
    }

    DeltaQFast *deltaQ = reinterpret_cast<DeltaQFast *>(mpAnim);

    UMath::Vector4 *quats = &reinterpret_cast<UMath::Vector4 *>(sqt)[1];

    unsigned char *boneIdxs = deltaQ->mBoneIdxs;

    if (deltaQ->mNumBones > 0) {

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
            int i;

            if (mPrevKey <= 0) {

                i = 0;

            } else {

                i = mPrevKey - 1;
            }

            if (deltaQ->mTimes[i] <= floorTime) {

                while (i < deltaQ->mNumKeys - 2 && deltaQ->mTimes[i + 1] <= floorTime) {

                    i++;
                }
            } else {

                while (i > 0 && deltaQ->mTimes[i] > floorTime) {

                    i--;
                }
            }

            floorKey = i + 1;
        }

        const unsigned int binLenPower = deltaQ->GetBinLengthPower();
        const unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

        int floorBinIdx = floorKey >> binLenPower;
        int floorDeltaIdx = floorKey & binLenModMask;
        int prevBinIdx = mPrevKey >> binLenPower;
        int prevDeltaIdx;

        int ceilKey = floorKey + 1;

        if (mNextKey == floorKey) {

            UMath::Vector4 *swapQs = mNextQs;
            mNextQs = mPrevQs;
            mPrevQs = swapQs;
            mNextKey = mPrevKey;

        } else {

            if (mPrevKey == ceilKey) {

                for (int b = 0; b < deltaQ->mNumBones; b++) {
                    mNextQs[b] = mPrevQs[b];
                }
                mNextKey = mPrevKey;
            }

            DeltaQFastPhysical *floorPhys = deltaQ->GetPhysical(&mBins[floorBinIdx * mBinSize]);

            bool preventReverse = floorKey < mPrevKey &&
                                  !IsReverseDeltaSumEnabled();

            if (mPrevKey == -1 || floorBinIdx != prevBinIdx || preventReverse) {

                for (int b = 0; b < deltaQ->mNumBones; b++) {

                    if (boneMask->GetBone(boneIdxs[b])) {
                        floorPhys[b].UnQuantize(mPrevQs[b]);
                    }
                }

                prevDeltaIdx = 0;

            } else {

                if (floorKey < mPrevKey) {
                    prevDeltaIdx = mPrevKey & binLenModMask;
                } else {
                    prevDeltaIdx = mPrevKey & binLenModMask;
                }
            }

            if (prevDeltaIdx < floorDeltaIdx) {

                AddDeltaMask(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs, boneMask);

            } else if (prevDeltaIdx > floorDeltaIdx) {

                SubDeltaMask(floorPhys, deltaQ, prevDeltaIdx, floorDeltaIdx, mPrevQs, boneMask);
            }
        }

        mPrevKey = floorKey;

        float scale;
        bool lerpReqd;

        if (!deltaQ->mTimes) {
            lerpReqd = currTime != floorTime;
            if (lerpReqd) {
                scale = currTime - floorTime;
            }
        } else if (floorKey == 0) {
            lerpReqd = currTime != 0.0f;
            if (lerpReqd) {
                float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
                scale = currTime / ceilKeyTime;
            }
        } else {
            float floorKeyTime = deltaQ->mTimes[floorKey - 1];
            lerpReqd = currTime != floorKeyTime;
            if (lerpReqd) {
                float ceilKeyTime = static_cast<float>(deltaQ->mTimes[floorKey]);
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }

        if (lerpReqd && floorKey < deltaQ->mNumKeys - 1) {

            UpdateNextQsMask(deltaQ, ceilKey, floorBinIdx, floorDeltaIdx, boneMask);

            for (int b = 0; b < deltaQ->mNumBones; b++) {

                if (boneMask->GetBone(boneIdxs[b])) {

                    FastPolarizedQuatBlend(scale, mPrevQs[b], mNextQs[b], quats[boneIdxs[b] * 3]);
                }
            }
        } else {

            for (int b = 0; b < deltaQ->mNumBones; b++) {

                if (boneMask->GetBone(boneIdxs[b])) {

                    quats[boneIdxs[b] * 3] = mPrevQs[b];
                }
            }
        }
    }

    if (deltaQ->mNumConstBones > 0) {

        for (int c = 0; c < deltaQ->mNumConstBones; c++) {

            if (boneMask->GetBone(mConstBoneIdxs[c])) {

                mConstPhysical[c].UnQuantize(quats[mConstBoneIdxs[c] * 3]);
            }
        }
    }

    return true;
}

void FnDeltaQFast::AddDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    DeltaQFastDelta *delta = &reinterpret_cast<DeltaQFastDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones];

    for (int i = prevDeltaIdx; i < floorDeltaIdx; i++) {

        for (int b = 0; b < deltaQ->mNumBones; b++) {

            if (boneMask->GetBone(boneIdxs[b])) {
                UMath::Vector4 q;

                delta->UnQuantize(mMinRangesf[b], q);

                prevQs[b].x += q.x;
                prevQs[b].y += q.y;
                prevQs[b].z += q.z;
                prevQs[b].w += q.w;
            }

            delta++;
        }
    }
}

void FnDeltaQFast::SubDeltaMask(DeltaQFastPhysical *floorPhys, DeltaQFast *deltaQ, int prevDeltaIdx, int floorDeltaIdx, UMath::Vector4 *prevQs,
                                const BoneMask *boneMask) {
    unsigned char *boneIdxs = deltaQ->mBoneIdxs;
    DeltaQFastDelta *delta = &reinterpret_cast<DeltaQFastDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones - 1];

    for (int i = prevDeltaIdx - 1; i >= floorDeltaIdx; i--) {

        for (int b = deltaQ->mNumBones - 1; b >= 0; b--) {

            if (boneMask->GetBone(boneIdxs[b])) {
                UMath::Vector4 q;

                delta->UnQuantize(mMinRangesf[b], q);

                prevQs[b].x -= q.x;
                prevQs[b].y -= q.y;
                prevQs[b].z -= q.z;
                prevQs[b].w -= q.w;
            }

            delta--;
        }
    }
}

void FnDeltaQFast::UpdateNextQsMask(DeltaQFast *deltaQ, int ceilKey, int floorBinIdx, int floorDeltaIdx, const BoneMask *boneMask) {
    if (ceilKey != mNextKey) {

        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        int ceilBinIdx = ceilKey >> deltaQ->GetBinLengthPower();

        unsigned char *bin = &mBins[ceilBinIdx * mBinSize];

        if (ceilBinIdx != floorBinIdx) {

            for (int b = 0; b < deltaQ->mNumBones; b++) {

                if (boneMask->GetBone(boneIdxs[b])) {
                    deltaQ->GetPhysical(bin)[b].UnQuantize(mNextQs[b]);
                }
            }
        } else {
            DeltaQFastDelta *delta =
                &reinterpret_cast<DeltaQFastDelta *>(&deltaQ->GetPhysical(bin)[deltaQ->mNumBones])[floorDeltaIdx * deltaQ->mNumBones];

            for (int b = 0; b < deltaQ->mNumBones; b++) {

                if (boneMask->GetBone(boneIdxs[b])) {
                    UMath::Vector4 q;

                    delta->UnQuantize(mMinRangesf[b], q);

                    mNextQs[b].x = q.x + mPrevQs[b].x;
                    mNextQs[b].y = q.y + mPrevQs[b].y;
                    mNextQs[b].z = q.z + mPrevQs[b].z;
                    mNextQs[b].w = q.w + mPrevQs[b].w;
                }

                delta++;
            }
        }

        mNextKey = ceilKey;
    }
}

}; // namespace EAGL4Anim
