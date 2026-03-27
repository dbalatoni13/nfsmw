#include "FnStatelessF3.h"
#include "AnimTypeId.h"
#include "StatelessF3.h"

namespace EAGL4Anim {

namespace {

static inline unsigned char GetStatelessF3BoneIndex(unsigned short dofIdx) {
    return static_cast<unsigned char>(dofIdx / 12);
}

static inline void UnquantizeStatelessF3(const StatelessF3::DofInfo &dofInfo, const short *frameBuf, UMath::Vector3 &result) {
    result.x = dofInfo.mRange[0] * frameBuf[0];
    result.y = dofInfo.mRange[1] * frameBuf[1];
    result.z = dofInfo.mRange[2] * frameBuf[2];
}

} // namespace

FnStatelessF3::FnStatelessF3() {
    mType = AnimTypeId::ANIM_STATELESSF3;
}

FnStatelessF3::~FnStatelessF3() {}

void FnStatelessF3::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnStatelessF3::GetLength(float &timeLength) const {
    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);

    timeLength = static_cast<float>(statelessF3->GetNumFrames());
    return true;
}

void FnStatelessF3::Eval(float, float currTime, float *sqt) {
    EvalSQT(currTime, sqt, nullptr);
}

bool FnStatelessF3::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (mUseFPS) {
        currTime *= mFPS;
    }

    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);

    int floorTime = FloatToInt(currTime);
    int floorKey;
    bool slerpReqd;
    float scale = 0.0f;

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
            int timeIndex = 0;

            if (mPrevKey != 0) {
                timeIndex = mPrevKey - 1;
            }
            if (statelessF3->mTimes[timeIndex] <= floorTime) {
                if (timeIndex < statelessF3->mNumKeys - 2) {
                    while (statelessF3->mTimes[timeIndex + 1] <= floorTime) {
                        timeIndex++;
                        if (timeIndex >= statelessF3->mNumKeys - 2) {
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
                } while (statelessF3->mTimes[timeIndex] > floorTime);
            }

            floorKey = timeIndex + 1;
        }
        if (floorKey == 0) {
            slerpReqd = currTime != 0.0f;
            if (slerpReqd) {
                scale = currTime / static_cast<float>(statelessF3->mTimes[0]);
            }
        } else {
            float floorKeyTime = static_cast<float>(statelessF3->mTimes[floorKey - 1]);

            slerpReqd = currTime != floorKeyTime;
            if (slerpReqd) {
                float ceilKeyTime = static_cast<float>(statelessF3->mTimes[floorKey]);
                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }
        if (floorKey >= statelessF3->mNumKeys - 1) {
            slerpReqd = false;
        }
    }

    mPrevKey = static_cast<unsigned short>(floorKey);

    if (!boneMask) {
        if (mBoneMask) {
            mBoneMask = nullptr;
        }

        short *dataBuf = statelessF3->GetData();
        StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
        unsigned short *dofIdxs = statelessF3->mDofIdxs;
        short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
        int nBones = statelessF3->mNumBones;

        if (slerpReqd) {
            short *nextFrameData = statelessF3->GetFrameData(dataBuf, floorKey + 1);

            for (int ibone = 0; ibone < nBones; ibone++) {
                UMath::Vector3 prev;
                UMath::Vector3 next;

                UnquantizeStatelessF3(dofInfos[ibone], frameData, prev);
                UnquantizeStatelessF3(dofInfos[ibone], nextFrameData, next);

                unsigned short index = dofIdxs[ibone];

                sqt[index + 0] = scale * (next.x - prev.x) + prev.x;
                sqt[index + 1] = scale * (next.y - prev.y) + prev.y;
                sqt[index + 2] = scale * (next.z - prev.z) + prev.z;

                frameData += 3;
                nextFrameData += 3;
            }
        } else {
            for (int ibone = 0; ibone < nBones; ibone++) {
                unsigned short index = dofIdxs[ibone];

                sqt[index + 0] = dofInfos[ibone].mRange[0] * frameData[0];
                sqt[index + 1] = dofInfos[ibone].mRange[1] * frameData[1];
                sqt[index + 2] = dofInfos[ibone].mRange[2] * frameData[2];
                frameData += 3;
            }
        }

        if (statelessF3->mNumConstBones != 0) {
            unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
            float *constBuf = statelessF3->GetConstData(dataBuf);

            for (int ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {
                float *constBone = &constBuf[ibone * 3];
                unsigned short index = constIdxs[ibone];

                sqt[index + 0] = constBone[0];
                sqt[index + 1] = constBone[1];
                sqt[index + 2] = constBone[2];
            }
        }
    } else {
        return EvalSQTMask(currTime, sqt, boneMask, slerpReqd, floorKey, scale);
    }

    return true;
}

bool FnStatelessF3::EvalSQTMask(float, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey, float scale) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
    }

    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);
    short *dataBuf = statelessF3->GetData();
    StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
    unsigned short *dofIdxs = statelessF3->mDofIdxs;
    short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
    int nBones = statelessF3->mNumBones;
    unsigned char boneIdxs[120];

    for (int ibone = 0; ibone < nBones; ibone++) {
        boneIdxs[ibone] = GetStatelessF3BoneIndex(dofIdxs[ibone]);
    }

    if (slerpReqd) {
        short *nextFrameData = statelessF3->GetFrameData(dataBuf, floorKey + 1);

        for (int ibone = 0; ibone < nBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector3 prev;
                UMath::Vector3 next;

                UnquantizeStatelessF3(dofInfos[ibone], frameData, prev);
                UnquantizeStatelessF3(dofInfos[ibone], nextFrameData, next);

                unsigned short index = dofIdxs[ibone];

                sqt[index + 0] = scale * (next.x - prev.x) + prev.x;
                sqt[index + 1] = scale * (next.y - prev.y) + prev.y;
                sqt[index + 2] = scale * (next.z - prev.z) + prev.z;
            }
            frameData += 3;
            nextFrameData += 3;
        }
    } else {
        for (int ibone = 0; ibone < nBones; ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                UMath::Vector3 value;
                unsigned short index = dofIdxs[ibone];

                UnquantizeStatelessF3(dofInfos[ibone], frameData, value);
                sqt[index + 0] = value.x;
                sqt[index + 1] = value.y;
                sqt[index + 2] = value.z;
            }
            frameData += 3;
        }
    }

    if (statelessF3->mNumConstBones != 0) {
        unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
        float *constBuf = statelessF3->GetConstData(dataBuf);

        for (int ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {
            unsigned char boneIdx = GetStatelessF3BoneIndex(constIdxs[ibone]);

            if (boneMask->GetBone(boneIdx)) {
                unsigned short index = constIdxs[ibone];

                sqt[index + 0] = constBuf[0];
                sqt[index + 1] = constBuf[1];
                sqt[index + 2] = constBuf[2];
            }
            constBuf += 3;
        }
    }

    return true;
}

bool FnStatelessF3::EvalSQTfast(float currTime, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey, float scale) {
    mPrevKey = static_cast<unsigned short>(floorKey);

    if (!boneMask) {
        StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(mpAnim);
        short *dataBuf = statelessF3->GetData();
        StatelessF3::DofInfo *dofInfos = statelessF3->GetDofInfo();
        unsigned short *dofIdxs = statelessF3->mDofIdxs;
        short *frameData = statelessF3->GetFrameData(dataBuf, floorKey);
        int nBones = statelessF3->mNumBones;

        if (slerpReqd) {
            short *nextFrameData = statelessF3->GetFrameData(dataBuf, floorKey + 1);

            for (int ibone = 0; ibone < nBones; ibone++) {
                UMath::Vector3 prev;
                UMath::Vector3 next;

                UnquantizeStatelessF3(dofInfos[ibone], frameData, prev);
                UnquantizeStatelessF3(dofInfos[ibone], nextFrameData, next);

                unsigned short index = dofIdxs[ibone];

                sqt[index + 0] = scale * (next.x - prev.x) + prev.x;
                sqt[index + 1] = scale * (next.y - prev.y) + prev.y;
                sqt[index + 2] = scale * (next.z - prev.z) + prev.z;

                frameData += 3;
                nextFrameData += 3;
            }
        } else {
            for (int ibone = 0; ibone < nBones; ibone++) {
                unsigned short index = dofIdxs[ibone];

                sqt[index + 0] = dofInfos[ibone].mRange[0] * frameData[0];
                sqt[index + 1] = dofInfos[ibone].mRange[1] * frameData[1];
                sqt[index + 2] = dofInfos[ibone].mRange[2] * frameData[2];
                frameData += 3;
            }
        }

        if (statelessF3->mNumConstBones != 0) {
            unsigned short *constIdxs = statelessF3->GetConstBoneIdx();
            float *constBuf = statelessF3->GetConstData(dataBuf);

            for (int ibone = 0; ibone < statelessF3->mNumConstBones; ibone++) {
                float *constBone = &constBuf[ibone * 3];
                unsigned short index = constIdxs[ibone];

                sqt[index + 0] = constBone[0];
                sqt[index + 1] = constBone[1];
                sqt[index + 2] = constBone[2];
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

unsigned short FnStatelessF3::GetTargetCheckSum() const {
    return mpAnim->GetTargetCheckSum();
}

const AttributeBlock *FnStatelessF3::GetAttributes() const {
    return reinterpret_cast<StatelessF3 *>(mpAnim)->GetAttributeBlock();
}

}; // namespace EAGL4Anim
