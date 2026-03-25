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
            int timeIndex = 0;

            if (mPrevKey != 0) {
                timeIndex = mPrevKey - 1;
            }
            if (statelessQ->mTimes[timeIndex] <= floorTime) {
                if (timeIndex < statelessQ->mNumKeys - 2) {
                    while (statelessQ->mTimes[timeIndex + 1] <= floorTime) {
                        timeIndex++;
                        if (timeIndex >= statelessQ->mNumKeys - 2) {
                            break;
                        }
                    }
                }
            } else if (timeIndex > 0) {
                do {
                    timeIndex--;
                    if (timeIndex < 1) {
                        break;
                    }
                } while (statelessQ->mTimes[timeIndex] > floorTime);
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
            unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, floorKey + 1);

            for (int ibone = 0; ibone < nBones; ibone++) {
                UMath::Vector4 prevQ;
                UMath::Vector4 nextQ;

                prevQ.x = UncompressStatelessQValue(*frameData++);
                prevQ.y = UncompressStatelessQValue(*frameData++);
                prevQ.z = UncompressStatelessQValue(*frameData++);
                prevQ.w = UncompressStatelessQValue(*frameData++);
                nextQ.x = UncompressStatelessQValue(*nextFrameData++);
                nextQ.y = UncompressStatelessQValue(*nextFrameData++);
                nextQ.z = UncompressStatelessQValue(*nextFrameData++);
                nextQ.w = UncompressStatelessQValue(*nextFrameData++);
                index = boneIdxs[ibone] * 12;

                q[index + 0] = scale * (nextQ.x - prevQ.x) + prevQ.x;
                q[index + 1] = scale * (nextQ.y - prevQ.y) + prevQ.y;
                q[index + 2] = scale * (nextQ.z - prevQ.z) + prevQ.z;
                q[index + 3] = scale * (nextQ.w - prevQ.w) + prevQ.w;
            }
        } else {
            for (int ibone = 0; ibone < nBones; ibone++) {
                index = boneIdxs[ibone] * 12;

                q[index + 0] = UncompressStatelessQValue(*frameData++);
                q[index + 1] = UncompressStatelessQValue(*frameData++);
                q[index + 2] = UncompressStatelessQValue(*frameData++);
                q[index + 3] = UncompressStatelessQValue(*frameData++);
            }
        }

        if (statelessQ->mNumConstBones != 0) {
            int numConsts = statelessQ->mNumConstBones;
            unsigned short *constBuf = statelessQ->GetConstData(dataBuf);
            unsigned char *constIdxs = statelessQ->GetConstBoneIdx();

            for (int ibone = 0; ibone < numConsts; ibone++) {
                index = *constIdxs++ * 12;

                q[index + 0] = UncompressStatelessQValue(*constBuf++);
                q[index + 1] = UncompressStatelessQValue(*constBuf++);
                q[index + 2] = UncompressStatelessQValue(*constBuf++);
                q[index + 3] = UncompressStatelessQValue(*constBuf++);
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

                LoadStatelessQ(prevData, prevQ);
                LoadStatelessQ(nextData, nextQ);
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

                q[index + 0] = UncompressStatelessQValue(*prevData++);
                q[index + 1] = UncompressStatelessQValue(*prevData++);
                q[index + 2] = UncompressStatelessQValue(*prevData++);
                q[index + 3] = UncompressStatelessQValue(*prevData);
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

                q[index + 0] = UncompressStatelessQValue(*prevData++);
                q[index + 1] = UncompressStatelessQValue(*prevData++);
                q[index + 2] = UncompressStatelessQValue(*prevData++);
                q[index + 3] = UncompressStatelessQValue(*prevData);
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
