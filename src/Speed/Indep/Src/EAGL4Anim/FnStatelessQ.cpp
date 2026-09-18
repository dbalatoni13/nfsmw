#include "FnStatelessQ.h"
#include "AnimTypeId.h"
#include "FnStatelessF3.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"
#include "StatelessQ.h"

namespace EAGL4Anim {

FnStatelessQ::FnStatelessQ() {
    mType = AnimTypeId::ANIM_STATELESSQ;
    mPrevKey = 0;
}

FnStatelessQ::~FnStatelessQ() {
}

void FnStatelessQ::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnStatelessQ::GetLength(float &timeLength) const {
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);

    timeLength = static_cast<float>(statelessQ->GetNumFrames());

    return true;
}

inline const AttributeBlock *FnStatelessQ::GetAttributes() const {
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);

    return statelessQ->mAttributeBlock;
}

void FnStatelessQ::Eval(float, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

bool FnStatelessQ::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {

    if (mUseFPS) {
        currTime = currTime * mFPS;
    }

    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);
    int floorTime = FloatToInt(currTime);
    int floorKey;
    float scale = 0.0f;
    bool slerpReqd;

    if (!statelessQ->mTimes) {

        if (floorTime < 0) {

            floorKey = 0;
        } else if (floorTime >= statelessQ->mNumKeys) {

            floorKey = statelessQ->mNumKeys - 1;
        } else {

            floorKey = floorTime;
        }

        slerpReqd = currTime != floorTime;
        scale = currTime - floorTime;
        if (floorKey >= statelessQ->mNumKeys - 1) {
            slerpReqd = false;
        }
    } else {

        if (floorTime < statelessQ->mTimes[0]) {

            floorKey = 0;
        } else {

            int timeIndex;

            if (mPrevKey) {

                timeIndex = mPrevKey - 1;
            } else {

                timeIndex = 0;
            }

            if (statelessQ->mTimes[timeIndex] <= floorTime) {

                while (timeIndex < statelessQ->mNumKeys - 2 && statelessQ->mTimes[timeIndex + 1] <= floorTime) {

                    timeIndex++;
                }
            } else {

                while (timeIndex > 0 && statelessQ->mTimes[timeIndex] > floorTime) {

                    timeIndex--;
                }
            }

            floorKey = timeIndex + 1;
        }

        if (floorKey == 0) {

            slerpReqd = currTime != 0.0f;

            if (slerpReqd) {

                float ceilKeyTime = statelessQ->mTimes[floorKey];
                scale = currTime / ceilKeyTime;
            }
        } else {

            float floorKeyTime = statelessQ->mTimes[floorKey - 1];
            slerpReqd = currTime != floorKeyTime;

            if (slerpReqd) {

                float ceilKeyTime = statelessQ->mTimes[floorKey];
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }

        if (floorKey >= statelessQ->mNumKeys - 1) {
            slerpReqd = false;
        }
    }

    mPrevKey = floorKey;

    if (boneMask == nullptr) {

        if (mBoneMask) {

            mBoneMask = boneMask;
        }

        float *q = sqt + 4;
        unsigned short *dataBuf = statelessQ->GetData();
        unsigned short *frameData = statelessQ->GetFrameData(dataBuf, floorKey);
        unsigned char *boneIdxs = statelessQ->mBoneIdxs;
        int nBones = statelessQ->mNumBones;
        int ibone;
        int index;
        int numConsts;

        if (slerpReqd && floorKey < statelessQ->mNumKeys - 1) {

            int nextKey = floorKey + 1;
            unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, nextKey);
            UMath::Vector4 prevQ;
            UMath::Vector4 nextQ;

            for (ibone = 0; ibone < nBones; ibone++) {

                *reinterpret_cast<unsigned int *>(&prevQ.x) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&prevQ.y) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&prevQ.z) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&prevQ.w) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&nextQ.x) = StatelessQ::Uncompress2Bits(*nextFrameData++);
                *reinterpret_cast<unsigned int *>(&nextQ.y) = StatelessQ::Uncompress2Bits(*nextFrameData++);
                *reinterpret_cast<unsigned int *>(&nextQ.z) = StatelessQ::Uncompress2Bits(*nextFrameData++);
                *reinterpret_cast<unsigned int *>(&nextQ.w) = StatelessQ::Uncompress2Bits(*nextFrameData++);

                index = boneIdxs[ibone] * 12;
                q[index] = prevQ.x + scale * (nextQ.x - prevQ.x);
                q[index + 1] = prevQ.y + scale * (nextQ.y - prevQ.y);
                q[index + 2] = prevQ.z + scale * (nextQ.z - prevQ.z);
                q[index + 3] = prevQ.w + scale * (nextQ.w - prevQ.w);
            }
        } else {

            for (ibone = 0; ibone < nBones; ibone++) {

                index = boneIdxs[ibone] * 12;
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*frameData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*frameData++);
            }
        }

        numConsts = statelessQ->mNumConstBones;

        if (numConsts > 0) {

            unsigned char *constIdxs = statelessQ->GetConstBoneIdx();
            unsigned short *constBuf = statelessQ->GetConstData(dataBuf);

            for (ibone = 0; ibone < numConsts; ibone++) {

                index = constIdxs[ibone] * 12;
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
            }
        }
    } else {

        EvalSQTMask(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    if (statelessQ->mF3Ptr) {

        reinterpret_cast<FnStatelessF3 *>(statelessQ->mF3Ptr)
            ->EvalSQTfast(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    return true;
}

bool FnStatelessQ::EvalSQTMask(float, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey, float scale) {

    if (boneMask != mBoneMask) {

        mBoneMask = boneMask;
    }

    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);
    float *q = sqt + 4;
    unsigned short *dataBuf = statelessQ->GetData();
    unsigned short *frameData = statelessQ->GetFrameData(dataBuf, floorKey);
    unsigned short *prevData;
    unsigned short *nextData;
    unsigned char *boneIdxs = statelessQ->mBoneIdxs;
    int nBones = statelessQ->mNumBones;
    int ibone;
    int index;
    int numConsts;

    if (slerpReqd && floorKey < statelessQ->mNumKeys - 1) {

        int nextKey = floorKey + 1;
        unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, nextKey);
        UMath::Vector4 prevQ;
        UMath::Vector4 nextQ;

        for (ibone = 0; ibone < nBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                prevData = &frameData[ibone * 4];
                nextData = &nextFrameData[ibone * 4];

                *reinterpret_cast<unsigned int *>(&prevQ.x) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&prevQ.y) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&prevQ.z) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&prevQ.w) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&nextQ.x) = StatelessQ::Uncompress2Bits(*nextData++);
                *reinterpret_cast<unsigned int *>(&nextQ.y) = StatelessQ::Uncompress2Bits(*nextData++);
                *reinterpret_cast<unsigned int *>(&nextQ.z) = StatelessQ::Uncompress2Bits(*nextData++);
                *reinterpret_cast<unsigned int *>(&nextQ.w) = StatelessQ::Uncompress2Bits(*nextData++);

                index = boneIdxs[ibone] * 12;
                q[index] = prevQ.x + scale * (nextQ.x - prevQ.x);
                q[index + 1] = prevQ.y + scale * (nextQ.y - prevQ.y);
                q[index + 2] = prevQ.z + scale * (nextQ.z - prevQ.z);
                q[index + 3] = prevQ.w + scale * (nextQ.w - prevQ.w);
            }
        }
    } else {

        for (ibone = 0; ibone < nBones; ibone++) {

            if (boneMask->GetBone(boneIdxs[ibone])) {

                index = boneIdxs[ibone] * 12;
                prevData = &frameData[ibone * 4];
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*prevData++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*prevData++);
            }
        }
    }

    numConsts = statelessQ->mNumConstBones;

    if (numConsts > 0) {

        unsigned char *constIdxs = statelessQ->GetConstBoneIdx();
        unsigned short *constBuf = statelessQ->GetConstData(dataBuf);

        for (ibone = 0; ibone < numConsts; ibone++) {

            if (boneMask->GetBone(constIdxs[ibone])) {

                index = constIdxs[ibone] * 12;
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
                *reinterpret_cast<unsigned int *>(&q[index++]) = StatelessQ::Uncompress2Bits(*constBuf++);
            }
        }
    }

    return true;
}

void FnStatelessQ::UseFPS(bool u) {
    mUseFPS = u;
    if (mFPS != 0 || !mUseFPS) {
        return;
    }
    GetAttribute(AttributeId(AttributeId::ID_FPS), mFPS);
}

}; // namespace EAGL4Anim
