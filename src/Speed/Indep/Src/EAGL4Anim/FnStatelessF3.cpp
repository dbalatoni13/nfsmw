#include "FnStatelessF3.h"
#include "AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"
#include "StatelessF3.h"

namespace EAGL4Anim {

FnStatelessF3::FnStatelessF3() {
    mType = AnimTypeId::ANIM_STATELESSF3;
}

FnStatelessF3::~FnStatelessF3() {
}

void FnStatelessF3::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnStatelessF3::GetLength(float &timeLength) const {
    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);

    timeLength = static_cast<float>(statelessF3->GetNumFrames());

    return true;
}

inline const AttributeBlock *FnStatelessF3::GetAttributes() const {
    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);

    return statelessF3->mAttributeBlock;
}

void FnStatelessF3::Eval(float, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

bool FnStatelessF3::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {

    if (mUseFPS) {
        currTime = currTime * mFPS;
    }

    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);
    int floorTime = FloatToInt(currTime);
    int floorKey;
    float scale = 0.0f;
    bool slerpReqd;

    if (!statelessF3->mTimes) {

        if (floorTime < 0) {

            floorKey = 0;
        } else if (floorTime >= statelessF3->mNumKeys) {

            floorKey = statelessF3->mNumKeys - 1;
        } else {

            floorKey = floorTime;
        }

        slerpReqd = currTime != floorTime;
        scale = currTime - floorTime;
        if (floorKey >= statelessF3->mNumKeys - 1) {
            slerpReqd = false;
        }
    } else {

        if (floorTime < statelessF3->mTimes[0]) {

            floorKey = 0;
        } else {

            int timeIndex;

            if (mPrevKey) {

                timeIndex = mPrevKey - 1;
            } else {

                timeIndex = 0;
            }

            if (statelessF3->mTimes[timeIndex] <= floorTime) {

                while (timeIndex < statelessF3->mNumKeys - 2 && statelessF3->mTimes[timeIndex + 1] <= floorTime) {

                    timeIndex++;
                }
            } else {

                while (timeIndex > 0 && statelessF3->mTimes[timeIndex] > floorTime) {

                    timeIndex--;
                }
            }

            floorKey = timeIndex + 1;
        }

        if (floorKey == 0) {

            slerpReqd = currTime != 0.0f;

            if (slerpReqd) {

                float ceilKeyTime = statelessF3->mTimes[floorKey];
                scale = currTime / ceilKeyTime;
            }
        } else {

            float floorKeyTime = statelessF3->mTimes[floorKey - 1];
            slerpReqd = currTime != floorKeyTime;

            if (slerpReqd) {

                float ceilKeyTime = statelessF3->mTimes[floorKey];
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }

        if (floorKey >= statelessF3->mNumKeys - 1) {
            slerpReqd = false;
        }
    }

    mPrevKey = floorKey;

    if (boneMask == nullptr) {

        StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
        short *dataBuf = statelessF3->GetData();
        short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
        unsigned short *dofIdxs = statelessF3->mDofIdxs;
        int nBones = statelessF3->mNumBones;
        int ibone;
        int index;

        if (slerpReqd) {

            int nextKey = floorKey + 1;
            short *nextFrameData = statelessF3->GetFrameData(dataBuf, nextKey);
            UMath::Vector3 prev;
            UMath::Vector3 next;

            for (ibone = 0; ibone < nBones; ibone++) {

                statelessF3->UnQuantize(dofInfos[ibone], frameData, prev);
                statelessF3->UnQuantize(dofInfos[ibone], nextFrameData, next);

                frameData += 3;
                nextFrameData += 3;

                index = dofIdxs[ibone];
                sqt[index] = prev.x + scale * (next.x - prev.x);
                sqt[index + 1] = prev.y + scale * (next.y - prev.y);
                sqt[index + 2] = prev.z + scale * (next.z - prev.z);
            }
        } else {

            for (ibone = 0; ibone < nBones; ibone++) {

                index = dofIdxs[ibone];
                float *rangePtr = dofInfos[ibone].mRange;
                sqt[index] = rangePtr[0] * frameData[0];
                sqt[index + 1] = rangePtr[1] * frameData[1];
                sqt[index + 2] = rangePtr[2] * frameData[2];
                frameData += 3;
            }
        }

        if (statelessF3->mNumConstBones) {

            unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
            float *constBuf = statelessF3->GetConstData(dataBuf);

            for (ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {

                index = constIdxs[ibone];
                sqt[index] = constBuf[ibone * 3];
                sqt[index + 1] = constBuf[ibone * 3 + 1];
                sqt[index + 2] = constBuf[ibone * 3 + 2];
            }
        }
    } else {

        return EvalSQTMask(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    return true;
}

bool FnStatelessF3::EvalSQTMask(float, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey,
                                float scale) {

    if (boneMask != mBoneMask) {

        mBoneMask = boneMask;
    }

    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);
    StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
    short *dataBuf = statelessF3->GetData();
    short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
    unsigned short *dofIdxs = statelessF3->mDofIdxs;
    int nBones = statelessF3->mNumBones;
    unsigned char boneIdxs[120];
    int ibone;
    int idof;
    int index;

    idof = 0;
    if (statelessF3->mNumBones != 0) {
        do {
            boneIdxs[idof] = dofIdxs[idof] / 12;
            idof++;
        } while (idof < statelessF3->mNumBones);
    }

    if (slerpReqd) {

        int nextKey = floorKey + 1;
        short *nextFrameData = statelessF3->GetFrameData(dataBuf, nextKey);
        UMath::Vector3 prev;
        UMath::Vector3 next;

        for (ibone = 0; ibone < nBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                statelessF3->UnQuantize(dofInfos[ibone], frameData, prev);
                statelessF3->UnQuantize(dofInfos[ibone], nextFrameData, next);

                frameData += 3;
                nextFrameData += 3;

                index = dofIdxs[ibone];
                sqt[index] = prev.x + scale * (next.x - prev.x);
                sqt[index + 1] = prev.y + scale * (next.y - prev.y);
                sqt[index + 2] = prev.z + scale * (next.z - prev.z);
            }
        }
    } else {

        for (ibone = 0; ibone < nBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                index = dofIdxs[ibone];
                float *rangePtr = dofInfos[ibone].mRange;
                sqt[index] = rangePtr[0] * frameData[0];
                sqt[index + 1] = rangePtr[1] * frameData[1];
                sqt[index + 2] = rangePtr[2] * frameData[2];
                frameData += 3;
            }
        }
    }

    if (statelessF3->mNumConstBones) {

        unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
        float *constBuf = statelessF3->GetConstData(dataBuf);

        for (ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                index = constIdxs[ibone];
                sqt[index] = constBuf[ibone * 3];
                sqt[index + 1] = constBuf[ibone * 3 + 1];
                sqt[index + 2] = constBuf[ibone * 3 + 2];
            }
        }
    }

    return true;
}

bool FnStatelessF3::EvalSQTfast(float currTime, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey,
                                float scale) {

    mPrevKey = floorKey;

    if (boneMask == nullptr) {

        StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);
        StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
        short *dataBuf = statelessF3->GetData();
        short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
        unsigned short *dofIdxs = statelessF3->mDofIdxs;
        int nBones = statelessF3->mNumBones;
        int ibone;
        int index;

        if (slerpReqd) {

            int nextKey = floorKey + 1;
            short *nextFrameData = statelessF3->GetFrameData(dataBuf, nextKey);
            UMath::Vector3 prev;
            UMath::Vector3 next;

            for (ibone = 0; ibone < nBones; ibone++) {

                statelessF3->UnQuantize(dofInfos[ibone], frameData, prev);
                statelessF3->UnQuantize(dofInfos[ibone], nextFrameData, next);

                frameData += 3;
                nextFrameData += 3;

                index = dofIdxs[ibone];
                sqt[index] = prev.x + scale * (next.x - prev.x);
                sqt[index + 1] = prev.y + scale * (next.y - prev.y);
                sqt[index + 2] = prev.z + scale * (next.z - prev.z);
            }
        } else {

            for (ibone = 0; ibone < nBones; ibone++) {

                index = dofIdxs[ibone];
                const StatelessF3::DofInfo &dofInfo = dofInfos[ibone];
                sqt[index] = dofInfo.mRange[0] * frameData[0];
                sqt[index + 1] = dofInfo.mRange[1] * frameData[1];
                sqt[index + 2] = dofInfo.mRange[2] * frameData[2];
                frameData += 3;
            }
        }

        if (statelessF3->mNumConstBones) {

            unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
            float *constBuf = statelessF3->GetConstData(dataBuf);

            for (ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {

                index = constIdxs[ibone];
                sqt[index] = constBuf[ibone * 3];
                sqt[index + 1] = constBuf[ibone * 3 + 1];
                sqt[index + 2] = constBuf[ibone * 3 + 2];
            }
        }
    } else {

        return EvalSQTMask(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    return true;
}

void FnStatelessF3::UseFPS(bool u) {
    mUseFPS = u;
    if (mFPS != 0 || !mUseFPS) {
        return;
    }
    GetAttribute(AttributeId(AttributeId::ID_FPS), mFPS);
}

}; // namespace EAGL4Anim
