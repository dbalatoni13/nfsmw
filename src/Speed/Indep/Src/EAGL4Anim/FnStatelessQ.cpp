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

FnStatelessQ::FnStatelessQ()
    : mUseFPS(false), //
      mFPS(0),        //
      mBoneMask(nullptr) {
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
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(mpAnim);

    if (mUseFPS) {
        currTime *= mFPS;
    }

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

        slerpReqd = floorKey < statelessQ->mNumKeys - 1;
        if (slerpReqd) {
            slerpReqd = currTime != floorTime;
            if (slerpReqd) {
                scale = currTime - floorTime;
            }
        }
    } else if (floorTime < statelessQ->mTimes[0]) {
        floorKey = 0;
        slerpReqd = currTime != 0.0f;
        if (slerpReqd) {
            scale = currTime / static_cast<float>(statelessQ->mTimes[0]);
        }
    } else {
        int timeIndex = 0;

        if (mPrevKey != 0) {
            timeIndex = mPrevKey - 1;
        }
        if (floorTime < statelessQ->mTimes[timeIndex]) {
            while (timeIndex > 0 && floorTime < statelessQ->mTimes[timeIndex]) {
                timeIndex--;
            }
        } else {
            while (timeIndex < statelessQ->mNumKeys - 2 && floorTime >= statelessQ->mTimes[timeIndex + 1]) {
                timeIndex++;
            }
        }

        floorKey = timeIndex + 1;
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

        unsigned short *dataBuf = statelessQ->GetData();
        unsigned short *frameData = statelessQ->GetFrameData(dataBuf, floorKey);
        unsigned char *boneIdxs = statelessQ->mBoneIdxs;
        int nBones = statelessQ->mNumBones;

        if (slerpReqd) {
            unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, floorKey + 1);

            for (int ibone = 0; ibone < nBones; ibone++) {
                UMath::Vector4 prevQ;
                UMath::Vector4 nextQ;
                float *q = GetStatelessQOutput(sqt, boneIdxs[ibone]);

                LoadStatelessQ(frameData, prevQ);
                LoadStatelessQ(nextFrameData, nextQ);

                q[0] = prevQ.x + (nextQ.x - prevQ.x) * scale;
                q[1] = prevQ.y + (nextQ.y - prevQ.y) * scale;
                q[2] = prevQ.z + (nextQ.z - prevQ.z) * scale;
                q[3] = prevQ.w + (nextQ.w - prevQ.w) * scale;

                frameData += 4;
                nextFrameData += 4;
            }
        } else {
            for (int ibone = 0; ibone < nBones; ibone++) {
                float *q = GetStatelessQOutput(sqt, boneIdxs[ibone]);

                q[0] = UncompressStatelessQValue(frameData[0]);
                q[1] = UncompressStatelessQValue(frameData[1]);
                q[2] = UncompressStatelessQValue(frameData[2]);
                q[3] = UncompressStatelessQValue(frameData[3]);
                frameData += 4;
            }
        }

        if (statelessQ->mNumConstBones != 0) {
            unsigned short *constBuf = statelessQ->GetConstData(dataBuf);
            unsigned char *constIdxs = statelessQ->GetConstBoneIdx();

            for (int ibone = 0; ibone < statelessQ->mNumConstBones; ibone++) {
                float *q = GetStatelessQOutput(sqt, *constIdxs++);

                q[0] = UncompressStatelessQValue(constBuf[0]);
                q[1] = UncompressStatelessQValue(constBuf[1]);
                q[2] = UncompressStatelessQValue(constBuf[2]);
                q[3] = UncompressStatelessQValue(constBuf[3]);
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
    sqt += 4;
    unsigned short *dataBuf = statelessQ->GetData();
    unsigned short *frameData = statelessQ->GetFrameData(dataBuf, floorKey);
    unsigned char *boneIdxs = statelessQ->mBoneIdxs;
    int nBones = statelessQ->mNumBones;

    if (slerpReqd && floorKey < statelessQ->mNumKeys - 1) {
        unsigned short *nextFrameData = statelessQ->GetFrameData(dataBuf, floorKey + 1);

        for (int ibone = 0; ibone < nBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];

            if (boneMask->GetBone(boneIdx)) {
                UMath::Vector4 prevQ;
                UMath::Vector4 nextQ;
                float *q = &sqt[boneIdx * 12];

                LoadStatelessQ(frameData, prevQ);
                LoadStatelessQ(nextFrameData, nextQ);

                q[0] = prevQ.x + (nextQ.x - prevQ.x) * scale;
                q[1] = prevQ.y + (nextQ.y - prevQ.y) * scale;
                q[2] = prevQ.z + (nextQ.z - prevQ.z) * scale;
                q[3] = prevQ.w + (nextQ.w - prevQ.w) * scale;
            }
            frameData += 4;
            nextFrameData += 4;
        }
    } else {
        for (int ibone = 0; ibone < nBones; ibone++) {
            unsigned char boneIdx = boneIdxs[ibone];

            if (boneMask->GetBone(boneIdx)) {
                float *q = &sqt[boneIdx * 12];

                q[0] = UncompressStatelessQValue(frameData[0]);
                q[1] = UncompressStatelessQValue(frameData[1]);
                q[2] = UncompressStatelessQValue(frameData[2]);
                q[3] = UncompressStatelessQValue(frameData[3]);
            }
            frameData += 4;
        }
    }

    if (statelessQ->mNumConstBones != 0) {
        unsigned char *constIdxs = statelessQ->GetConstBoneIdx();
        unsigned short *constBuf = statelessQ->GetConstData(dataBuf);

        for (int ibone = 0; ibone < statelessQ->mNumConstBones; ibone++) {
            unsigned char boneIdx = constIdxs[ibone];

            if (boneMask->GetBone(boneIdx)) {
                float *q = &sqt[boneIdx * 12];

                q[0] = UncompressStatelessQValue(constBuf[0]);
                q[1] = UncompressStatelessQValue(constBuf[1]);
                q[2] = UncompressStatelessQValue(constBuf[2]);
                q[3] = UncompressStatelessQValue(constBuf[3]);
            }
            constBuf += 4;
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
