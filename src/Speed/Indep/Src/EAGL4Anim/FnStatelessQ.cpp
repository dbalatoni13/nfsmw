#include "FnStatelessQ.h"
#include "AnimTypeId.h"
#include "FnStatelessF3.h"
#include "StatelessQ.h"

namespace EAGL4Anim {

namespace {

static inline float UncompressStatelessQValue(unsigned short value) {
    union {
        unsigned int u;
        float f;
    } bits;

    bits.u = ((value & 0x8000) << 16) | ((value & 0x7FFF) << 15);
    return bits.f;
}

#define STORE_STATELESS_Q_BITS(dst, value) *reinterpret_cast<unsigned int *>(&(dst)) = (((value) & 0x8000) << 16) | (((value) & 0x7FFF) << 15)

static inline float *GetStatelessQOutput(float *sqt, unsigned char boneIdx) {
    return &sqt[boneIdx * 12 + 4];
}

static inline void LoadStatelessQ(unsigned short *frameData, UMath::Vector4 &q) {
    q.x = UncompressStatelessQValue(frameData[0]);
    q.y = UncompressStatelessQValue(frameData[1]);
    q.z = UncompressStatelessQValue(frameData[2]);
    q.w = UncompressStatelessQValue(frameData[3]);
}

} // namespace

FnStatelessQ::FnStatelessQ() {
    mType = AnimTypeId::ANIM_STATELESSQ;
    mPrevKey = 0;
}

FnStatelessQ::~FnStatelessQ() {}

void FnStatelessQ::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnStatelessQ::GetLength(float &timeLength) const {
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);

    timeLength = static_cast<float>(statelessQ->GetNumFrames());
    return true;
}

void FnStatelessQ::Eval(float, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

bool FnStatelessQ::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (mUseFPS) {
        currTime *= mFPS;
    }

    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);

    int floorTime = FloatToInt(currTime);
    int floorKey;
    bool slerpReqd;
    float scale = 0.0f;

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
            int timeIndex = mPrevKey != 0 ? mPrevKey - 1 : 0;

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
                scale = currTime / static_cast<float>(statelessQ->mTimes[0]);
            }
        } else {
            float floorKeyTime = static_cast<float>(statelessQ->mTimes[floorKey - 1]);

            slerpReqd = currTime != floorKeyTime;
            if (slerpReqd) {
                float ceilKeyTime = static_cast<float>(statelessQ->mTimes[floorKey]);
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }
        if (floorKey >= statelessQ->mNumKeys - 1) {
            slerpReqd = false;
        }
    }

    mPrevKey = static_cast<unsigned short>(floorKey);

    if (!boneMask) {
        if (mBoneMask) {
            mBoneMask = nullptr;
        }

        float *q = sqt + 4;
        unsigned short *dataBuf = statelessQ->GetData();
        unsigned short *frameData = statelessQ->GetFrameData(dataBuf, floorKey);
        unsigned char *boneIdxs = statelessQ->mBoneIdxs;
        int nBones = statelessQ->mNumBones;
        int index;

        if (slerpReqd && floorKey < statelessQ->mNumKeys - 1) {
            int nextKey = floorKey + 1;
            unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, nextKey);

            for (int ibone = 0; ibone < nBones; ibone++) {
                UMath::Vector4 prevQ;
                UMath::Vector4 nextQ;

                STORE_STATELESS_Q_BITS(prevQ.x, frameData[0]);
                STORE_STATELESS_Q_BITS(prevQ.y, frameData[1]);
                STORE_STATELESS_Q_BITS(prevQ.z, frameData[2]);
                STORE_STATELESS_Q_BITS(prevQ.w, frameData[3]);
                frameData += 4;
                STORE_STATELESS_Q_BITS(nextQ.x, nextFrameData[0]);
                STORE_STATELESS_Q_BITS(nextQ.y, nextFrameData[1]);
                STORE_STATELESS_Q_BITS(nextQ.z, nextFrameData[2]);
                STORE_STATELESS_Q_BITS(nextQ.w, nextFrameData[3]);
                nextFrameData += 4;
                index = boneIdxs[ibone] * 12;

                q[index + 0] = scale * (nextQ.x - prevQ.x) + prevQ.x;
                q[index + 1] = scale * (nextQ.y - prevQ.y) + prevQ.y;
                q[index + 2] = scale * (nextQ.z - prevQ.z) + prevQ.z;
                q[index + 3] = scale * (nextQ.w - prevQ.w) + prevQ.w;
            }
        } else {
            for (int ibone = 0; ibone < nBones; ibone++) {
                index = boneIdxs[ibone] * 12;

                STORE_STATELESS_Q_BITS(q[index + 0], frameData[0]);
                STORE_STATELESS_Q_BITS(q[index + 1], frameData[1]);
                STORE_STATELESS_Q_BITS(q[index + 2], frameData[2]);
                STORE_STATELESS_Q_BITS(q[index + 3], frameData[3]);
                frameData += 4;
            }
        }

        if (statelessQ->mNumConstBones != 0) {
            int numConsts = statelessQ->mNumConstBones;
            unsigned char *constIdxs = statelessQ->GetConstBoneIdx();
            unsigned short *constBuf = statelessQ->GetConstData(dataBuf);

            for (int ibone = 0; ibone < numConsts; ibone++) {
                index = *constIdxs++ * 12;

                STORE_STATELESS_Q_BITS(q[index + 0], constBuf[0]);
                STORE_STATELESS_Q_BITS(q[index + 1], constBuf[1]);
                STORE_STATELESS_Q_BITS(q[index + 2], constBuf[2]);
                STORE_STATELESS_Q_BITS(q[index + 3], constBuf[3]);
                constBuf += 4;
            }
        }
    } else {
        EvalSQTMask(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    if (statelessQ->mF3Ptr) {
        reinterpret_cast<FnStatelessF3 *>(statelessQ->mF3Ptr)->EvalSQTfast(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
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
    int index;
    int numConsts;

    if (slerpReqd && floorKey < statelessQ->mNumKeys - 1) {
        int nextKey = floorKey + 1;
        unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, nextKey);

        for (int ibone = 0; ibone < nBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector4 prevQ;
                UMath::Vector4 nextQ;
                prevData = &frameData[ibone * 4];
                nextData = &nextFrameData[ibone * 4];

                STORE_STATELESS_Q_BITS(prevQ.x, prevData[0]);
                STORE_STATELESS_Q_BITS(prevQ.y, prevData[1]);
                STORE_STATELESS_Q_BITS(prevQ.z, prevData[2]);
                STORE_STATELESS_Q_BITS(prevQ.w, prevData[3]);
                STORE_STATELESS_Q_BITS(nextQ.x, nextData[0]);
                STORE_STATELESS_Q_BITS(nextQ.y, nextData[1]);
                STORE_STATELESS_Q_BITS(nextQ.z, nextData[2]);
                STORE_STATELESS_Q_BITS(nextQ.w, nextData[3]);
                index = boneIdxs[ibone] * 12;

                q[index + 0] = scale * (nextQ.x - prevQ.x) + prevQ.x;
                q[index + 1] = scale * (nextQ.y - prevQ.y) + prevQ.y;
                q[index + 2] = scale * (nextQ.z - prevQ.z) + prevQ.z;
                q[index + 3] = scale * (nextQ.w - prevQ.w) + prevQ.w;
            }
        }
    } else {
        for (int ibone = 0; ibone < nBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                prevData = &frameData[ibone * 4];

                index = boneIdxs[ibone] * 12;

                STORE_STATELESS_Q_BITS(q[index + 0], prevData[0]);
                STORE_STATELESS_Q_BITS(q[index + 1], prevData[1]);
                STORE_STATELESS_Q_BITS(q[index + 2], prevData[2]);
                STORE_STATELESS_Q_BITS(q[index + 3], prevData[3]);
            }
        }
    }

    numConsts = statelessQ->mNumConstBones;
    if (numConsts != 0) {
        unsigned char *constIdxs = statelessQ->GetConstBoneIdx();
        unsigned short *constBuf = statelessQ->GetConstData(dataBuf);

        for (int ibone = 0; ibone < numConsts; ibone++) {
            if (boneMask->GetBone(constIdxs[ibone])) {
                prevData = &constBuf[ibone * 4];

                index = constIdxs[ibone] * 12;

                STORE_STATELESS_Q_BITS(q[index + 0], prevData[0]);
                STORE_STATELESS_Q_BITS(q[index + 1], prevData[1]);
                STORE_STATELESS_Q_BITS(q[index + 2], prevData[2]);
                STORE_STATELESS_Q_BITS(q[index + 3], prevData[3]);
            }
        }
    }

    return true;
}

void FnStatelessQ::UseFPS(bool u) {
    mUseFPS = u;
    if (mFPS != 0 || !u) {
        return;
    }
    GetAttribute(AttributeId(AttributeId::ID_FPS), mFPS);
}

unsigned short FnStatelessQ::GetTargetCheckSum() const {
    return mpAnim->GetTargetCheckSum();
}

const AttributeBlock *FnStatelessQ::GetAttributes() const {
    return reinterpret_cast<StatelessQ *>(mpAnim)->GetAttributeBlock();
}

}; // namespace EAGL4Anim
