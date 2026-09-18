#include "FnDeltaQ.h"
#include "AnimTypeId.h"
#include "MemoryPoolManager.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

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

inline void FnDeltaQ::InitBuffersAsRequired() {

    if (!mBins) {

        DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);
        DeltaQMinRange *minRanges;

        deltaQ->GetArrays(minRanges, mBins, mConstBoneIdxs, mConstPhysical);

        mBinSize = deltaQ->GetBinSize();

        if (deltaQ->mNumBones != 0) {

            mPrevQBlock = MemoryPoolManager::NewBlock(deltaQ->mNumBones * sizeof(UMath::Vector4));
            mPrevQs = reinterpret_cast<UMath::Vector4 *>(mPrevQBlock);

            mMinRanges = minRanges;
        }
    }
}

FnDeltaQ::~FnDeltaQ() {
    if (mPrevQBlock) {
        MemoryPoolManager::DeleteBlock(mPrevQBlock);
    }
}

void FnDeltaQ::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnDeltaQ::GetLength(float &timeLength) const {
    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    timeLength = static_cast<float>(deltaQ->GetNumFrames());
    return true;
}

void FnDeltaQ::Eval(float prevTime, float currTime, float *sqt) {
    EvalSQTMasked(currTime, nullptr, sqt);
}

inline void FastPolarizedQuatBlend(float t, const UMath::Vector4 &q0, const UMath::Vector4 &q1, UMath::Vector4 &result) {

    UMath::Vector4 temp;
    float s;

    temp.x = t * (q1.x - q0.x) + q0.x;
    temp.y = t * (q1.y - q0.y) + q0.y;
    temp.z = t * (q1.z - q0.z) + q0.z;
    temp.w = t * (q1.w - q0.w) + q0.w;

    s = FastSqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z + temp.w * temp.w);

    s = 1.0f / s;

    temp.x *= s;
    temp.y *= s;
    temp.z *= s;
    temp.w *= s;

    result = temp;
}

bool FnDeltaQ::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {

    if (boneMask) {

        return EvalSQTMasked(currTime, boneMask, sqt);
    }

    InitBuffersAsRequired();

    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    if (deltaQ->mNumBones != 0) {

        int floorTime = FloatToInt(currTime);

        int floorKey;

        if (!deltaQ->mTimes) {

            if (floorTime < 0) {

                floorKey = 0;

            } else {

                if (floorTime >= deltaQ->mNumKeys) {

                    floorKey = deltaQ->mNumKeys - 1;

                } else {

                    floorKey = floorTime;
                }
            }
        } else if (floorTime < deltaQ->mTimes[0]) {

            floorKey = 0;

        } else {
            int timeIndex;

            if (mPrevKey <= 0) {

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

        const unsigned int binLenPower = deltaQ->GetBinLengthPower();
        const unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

        int floorBinIdx = floorKey >> binLenPower;
        int floorDeltaIdx = floorKey & binLenModMask;
        int prevBinIdx = mPrevKey >> binLenPower;
        int prevDeltaIdx;

        DeltaQPhysical *floorPhys = deltaQ->GetPhysical(&mBins[floorBinIdx * mBinSize]);
        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        bool preventReverse = floorKey < mPrevKey &&
                              !IsReverseDeltaSumEnabled();

        if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                floorPhys[ibone].UnQuantize(mPrevQs[ibone]);
            }

            prevDeltaIdx = 0;

        } else {

            int mask = binLenModMask;

            if (floorKey < mPrevKey) {
                prevDeltaIdx = mPrevKey & mask;
            } else {
                prevDeltaIdx = mPrevKey & mask;
            }
        }

        if (prevDeltaIdx < floorDeltaIdx) {

            DeltaQDelta *floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones];

            for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {

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

            floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[(floorDeltaIdx - 1) * deltaQ->mNumBones];

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);

                floorDelta++;
            }

        } else if (prevDeltaIdx > floorDeltaIdx) {

            DeltaQDelta *floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones - 1];

            for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {

                for (int ibone = deltaQ->mNumBones - 1; ibone >= 0; ibone--) {

                    UMath::Vector4 deltaf;
                    DeltaQMinRangef minRangef;

                    mMinRanges[ibone].UnQuantize(minRangef);
                    floorDelta->UnQuantize(minRangef, deltaf);

                    mPrevQs[ibone].x -= deltaf.x;
                    mPrevQs[ibone].y -= deltaf.y;
                    mPrevQs[ibone].z -= deltaf.z;

                    floorDelta--;
                }
            }

            floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[(floorDeltaIdx - 1) * deltaQ->mNumBones];

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);

                floorDelta++;
            }
        }

        mPrevKey = floorKey;

        int ceilKey = floorKey + 1;

        float scale;
        bool slerpReqd;

        if (!deltaQ->mTimes) {
            slerpReqd = currTime != floorTime;
            if (slerpReqd) {
                scale = currTime - floorTime;
            }
        } else if (floorKey == 0) {
            slerpReqd = currTime != 0.0f;
            if (slerpReqd) {
                float ceilKeyTime = deltaQ->mTimes[floorKey];
                scale = currTime / ceilKeyTime;
            }
        } else {
            float floorKeyTime = deltaQ->mTimes[floorKey - 1];
            slerpReqd = currTime != floorKeyTime;
            if (slerpReqd) {
                float ceilKeyTime = deltaQ->mTimes[floorKey];
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }

        if (slerpReqd && floorKey < deltaQ->mNumKeys - 1) {

            int ceilBinIdx = ceilKey >> binLenPower;
            DeltaQPhysical *ceilPhys = deltaQ->GetPhysical(&mBins[ceilBinIdx * mBinSize]);

            if (ceilBinIdx != floorBinIdx) {

                for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                    UMath::Vector4 ceilq;

                    ceilPhys[ibone].UnQuantize(ceilq);

                    FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1]);
                }
            } else {

                int ceilDeltaIdx = floorDeltaIdx;
                DeltaQDelta *ceilDelta = &reinterpret_cast<DeltaQDelta *>(&ceilPhys[deltaQ->mNumBones])[ceilDeltaIdx * deltaQ->mNumBones];

                for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                    UMath::Vector4 ceilq;
                    DeltaQMinRangef minRangef;

                    mMinRanges[ibone].UnQuantize(minRangef);
                    ceilDelta->UnQuantize(minRangef, ceilq);

                    ceilq.x += mPrevQs[ibone].x;
                    ceilq.y += mPrevQs[ibone].y;
                    ceilq.z += mPrevQs[ibone].z;

                    DeltaQRecoverW(ceilDelta->mW, ceilq);

                    FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1]);

                    ceilDelta++;
                }
            }
        } else {

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1] = mPrevQs[ibone];
            }
        }
    }

    if (deltaQ->mNumConstBones != 0) {

        for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {

            mConstPhysical[ibone].UnQuantize(reinterpret_cast<UMath::Vector4 *>(sqt)[mConstBoneIdxs[ibone] * 3 + 1]);
        }
    }

    return true;
}

bool FnDeltaQ::EvalSQTMasked(float currTime, const BoneMask *boneMask, float *sqt) {

    InitBuffersAsRequired();

    DeltaQ *deltaQ = reinterpret_cast<DeltaQ *>(mpAnim);

    if (deltaQ->mNumBones != 0) {

        int floorTime = FloatToInt(currTime);

        int floorKey;

        if (!deltaQ->mTimes) {

            if (floorTime < 0) {

                floorKey = 0;

            } else {

                if (floorTime >= deltaQ->mNumKeys) {

                    floorKey = deltaQ->mNumKeys - 1;

                } else {

                    floorKey = floorTime;
                }
            }
        } else if (floorTime < deltaQ->mTimes[0]) {

            floorKey = 0;

        } else {
            int timeIndex;

            if (mPrevKey <= 0) {

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

        const unsigned int binLenPower = deltaQ->GetBinLengthPower();
        const unsigned int binLenModMask = deltaQ->GetBinLengthModMask();

        int floorBinIdx = floorKey >> binLenPower;
        int floorDeltaIdx = floorKey & binLenModMask;
        int prevBinIdx = mPrevKey >> binLenPower;
        int prevDeltaIdx;

        DeltaQPhysical *floorPhys = deltaQ->GetPhysical(&mBins[floorBinIdx * mBinSize]);
        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        bool preventReverse = floorKey < mPrevKey &&
                              !IsReverseDeltaSumEnabled();

        if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                if (boneMask->GetBone(boneIdxs[ibone])) {

                    floorPhys[ibone].UnQuantize(mPrevQs[ibone]);
                }
            }

            prevDeltaIdx = 0;

        } else {

            int mask = binLenModMask;

            if (floorKey < mPrevKey) {
                prevDeltaIdx = mPrevKey & mask;
            } else {
                prevDeltaIdx = mPrevKey & mask;
            }
        }

        if (prevDeltaIdx < floorDeltaIdx) {

            DeltaQDelta *floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones];

            for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {

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

            floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[(floorDeltaIdx - 1) * deltaQ->mNumBones];

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                if (boneMask->GetBone(boneIdxs[ibone])) {

                    DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);
                }

                floorDelta++;
            }

        } else if (prevDeltaIdx > floorDeltaIdx) {

            DeltaQDelta *floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[prevDeltaIdx * deltaQ->mNumBones - 1];

            for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {

                for (int ibone = deltaQ->mNumBones - 1; ibone >= 0; ibone--) {

                    if (boneMask->GetBone(boneIdxs[ibone])) {

                        UMath::Vector4 deltaf;
                        DeltaQMinRangef minRangef;

                        mMinRanges[ibone].UnQuantize(minRangef);
                        floorDelta->UnQuantize(minRangef, deltaf);

                        mPrevQs[ibone].x -= deltaf.x;
                        mPrevQs[ibone].y -= deltaf.y;
                        mPrevQs[ibone].z -= deltaf.z;
                    }

                    floorDelta--;
                }
            }

            floorDelta = &reinterpret_cast<DeltaQDelta *>(&floorPhys[deltaQ->mNumBones])[(floorDeltaIdx - 1) * deltaQ->mNumBones];

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                if (boneMask->GetBone(boneIdxs[ibone])) {

                    DeltaQRecoverW(floorDelta->mW, mPrevQs[ibone]);
                }

                floorDelta++;
            }
        }

        mPrevKey = floorKey;

        int ceilKey = floorKey + 1;

        float scale;
        bool slerpReqd;

        if (!deltaQ->mTimes) {
            slerpReqd = currTime != floorTime;
            if (slerpReqd) {
                scale = currTime - floorTime;
            }
        } else if (floorKey == 0) {
            slerpReqd = currTime != 0.0f;
            if (slerpReqd) {
                float ceilKeyTime = deltaQ->mTimes[floorKey];
                scale = currTime / ceilKeyTime;
            }
        } else {
            float floorKeyTime = deltaQ->mTimes[floorKey - 1];
            slerpReqd = currTime != floorKeyTime;
            if (slerpReqd) {
                float ceilKeyTime = deltaQ->mTimes[floorKey];
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }

        if (slerpReqd && floorKey < deltaQ->mNumKeys - 1) {

            int ceilBinIdx = ceilKey >> binLenPower;
            DeltaQPhysical *ceilPhys = deltaQ->GetPhysical(&mBins[ceilBinIdx * mBinSize]);

            if (ceilBinIdx != floorBinIdx) {

                for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                    if (boneMask->GetBone(boneIdxs[ibone])) {

                        UMath::Vector4 ceilq;

                        ceilPhys[ibone].UnQuantize(ceilq);

                        FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1]);
                    }
                }
            } else {

                int ceilDeltaIdx = floorDeltaIdx;
                DeltaQDelta *ceilDelta = &reinterpret_cast<DeltaQDelta *>(&ceilPhys[deltaQ->mNumBones])[ceilDeltaIdx * deltaQ->mNumBones];

                for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                    if (boneMask->GetBone(boneIdxs[ibone])) {

                        UMath::Vector4 ceilq;
                        DeltaQMinRangef minRangef;

                        mMinRanges[ibone].UnQuantize(minRangef);
                        ceilDelta->UnQuantize(minRangef, ceilq);

                        ceilq.x += mPrevQs[ibone].x;
                        ceilq.y += mPrevQs[ibone].y;
                        ceilq.z += mPrevQs[ibone].z;

                        DeltaQRecoverW(ceilDelta->mW, ceilq);

                        FastPolarizedQuatBlend(scale, mPrevQs[ibone], ceilq, reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1]);
                    }

                    ceilDelta++;
                }
            }
        } else {

            for (int ibone = 0; ibone < deltaQ->mNumBones; ibone++) {

                if (boneMask->GetBone(boneIdxs[ibone])) {

                    reinterpret_cast<UMath::Vector4 *>(sqt)[boneIdxs[ibone] * 3 + 1] = mPrevQs[ibone];
                }
            }
        }
    }

    if (deltaQ->mNumConstBones != 0) {

        unsigned char *boneIdxs = deltaQ->mBoneIdxs;

        for (int ibone = 0; ibone < deltaQ->mNumConstBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                mConstPhysical[ibone].UnQuantize(reinterpret_cast<UMath::Vector4 *>(sqt)[mConstBoneIdxs[ibone] * 3 + 1]);
            }
        }
    }

    mPrevKey = -1;

    return true;
}

}; // namespace EAGL4Anim
