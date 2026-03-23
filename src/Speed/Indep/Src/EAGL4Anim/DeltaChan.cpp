#include "DeltaChan.h"
#include "AnimUtil.h"
#include "MemoryPoolManager.h"

namespace EAGL4Anim {

FnDeltaChan::~FnDeltaChan() {
    if (mPrevValues) {
        MemoryPoolManager::DeleteBlock(mPrevValues);
    }
    if (mDofMask) {
        MemoryPoolManager::DeleteBlock(mDofMask);
    }
}

void FnDeltaChan::SetAnimMemoryMap(AnimMemoryMap *anim) {
    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(anim);
    if (mPrevValues) {
        if (deltaChan->GetNumDofs() >= reinterpret_cast<DeltaChan *>(mpAnim)->GetNumDofs()) {
            MemoryPoolManager::DeleteBlock(mPrevValues);
            mPrevValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaChan->GetNumDofs() * sizeof(*mPrevValues)));
        }
    } else {
        mPrevValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaChan->GetNumDofs() * sizeof(*mPrevValues)));
    }
    mpAnim = deltaChan;
    mPrevFrame = -1;
}

void FnDeltaChan::EvalToPrevValues(int frame) {
    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    const int numFrames = deltaChan->GetNumFrames();

    if (frame >= numFrames) {
        frame = numFrames - 1;
    } else if (frame < 0) {
        frame = 0;
    }

    unsigned short numDofs = deltaData->GetNumDofs();
    deltaData->DecompressValues(0, numDofs, mPrevFrame, frame, mPrevValues, mPrevValues);
    mPrevFrame = frame;
}

void FnDeltaChan::EvalToPrevValues(int frame, int numDofPerBone, int, const unsigned short *) {
    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    const int numFrames = deltaChan->GetNumFrames();

    if (frame >= numFrames) {
        frame = numFrames - 1;
    } else if (frame < 0) {
        frame = 0;
    }

    deltaData->DecompressValues(numDofPerBone, mPrevFrame, frame, mPrevValues, mPrevValues, mNumDofs, mDofMask);
    mPrevFrame = frame;
}

void FnDeltaLerpChan::Eval(float prevTime, float currTime, float *evalBuffer) {
    int floorFrame = FloatToInt(currTime);
    int ceilFrame = floorFrame + 1;
    EvalToPrevValues(floorFrame);

    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    int numFrames = deltaChan->GetNumFrames();
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *dofIndices = deltaChan->GetDofIndices();

    if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
        for (int idof = 0; idof < numDofs; idof++) {
            evalBuffer[dofIndices[idof]] = mPrevValues[idof];
        }
    } else {
        float t = currTime - static_cast<float>(floorFrame);
        deltaData->DecompressValuesIndexed(0, numDofs, floorFrame, ceilFrame, mPrevValues, evalBuffer, 1, dofIndices, t);
    }
}

bool FnDeltaLerpChan::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!boneMask) {
        if (mBoneMask) {
            mPrevFrame = -1;
            mBoneMask = nullptr;
        }
        int floorFrame = FloatToInt(currTime);
        int ceilFrame = floorFrame + 1;
        EvalToPrevValues(floorFrame);

        DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
        int numFrames = deltaChan->GetNumFrames();
        DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
        unsigned short numDofs = deltaData->GetNumDofs();
        unsigned short *dofIndices = deltaChan->GetDofIndices();

        if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
            for (int idof = 0; idof < numDofs; idof++) {
                sqt[dofIndices[idof]] = mPrevValues[idof];
            }
        } else {
            float t = currTime - static_cast<float>(floorFrame);
            deltaData->DecompressValuesIndexed(0, numDofs, floorFrame, ceilFrame, mPrevValues, sqt, 1, dofIndices, t);
        }
    } else {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    return true;
}

bool FnDeltaLerpChan::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevFrame = -1;
    }

    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    int numFrames = deltaChan->GetNumFrames();
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *dofIndices = deltaChan->GetDofIndices();

    int idof;
    int boneIdx;
    if (mDofMask == nullptr && numDofs != 0) {
        mDofMask = reinterpret_cast<unsigned short *>(MemoryPoolManager::NewBlock(numDofs * sizeof(*mDofMask)));
        for (idof = 0; idof < numDofs; idof++) {
            boneIdx = (dofIndices[idof] / 0xCu) & 0xFF; // TODO magic
            if (boneMask->GetBone(boneIdx)) {
                mDofMask[mNumDofs++] = idof;
            }
        }
    }

    int floorFrame = FloatToInt(currTime);
    int ceilFrame = floorFrame + 1;

    EvalToPrevValues(floorFrame, 1, mNumDofs, mDofMask);

    if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
        for (idof = 0; idof < mNumDofs; idof++) {
            sqt[dofIndices[mDofMask[idof]]] = mPrevValues[mDofMask[idof]];
        }
    } else {
        float t = currTime - static_cast<float>(floorFrame);
        deltaData->DecompressValuesIndexed(floorFrame, ceilFrame, mPrevValues, sqt, 1, dofIndices, t, mNumDofs, mDofMask);
    }
    return true;
}

bool FnDeltaLerpChan::EvalWeights(float currTime, float *weights) {
    Eval(currTime, currTime, weights);
    return true;
}

bool FnDeltaLerpChan::EvalVel2D(float currTime, float *vel) {
    Eval(currTime, currTime, vel);
    return true;
}

void FnDeltaQuatChan::Eval(float prevTime, float currTime, float *evalBuffer) {
    int floorFrame = FloatToInt(currTime);
    int ceilFrame = floorFrame + 1;
    EvalToPrevValues(floorFrame);

    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    int numFrames = deltaChan->GetNumFrames();
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *dofIndices = deltaChan->GetDofIndices();

    int idof;
    int iquat;

    if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
        for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
            *reinterpret_cast<UMath::Vector4 *>(&evalBuffer[dofIndices[idof]]) = *reinterpret_cast<UMath::Vector4 *>(&mPrevValues[iquat]);
        }
    } else {
        float t = currTime - static_cast<float>(floorFrame);
        deltaData->DecompressValuesIndexed(0, numDofs, floorFrame, ceilFrame, mPrevValues, evalBuffer, 4, dofIndices, t);
    }
}

bool FnDeltaQuatChan::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!boneMask) {
        if (mBoneMask) {
            mPrevFrame = -1;
            mBoneMask = nullptr;
        }

        DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
        int numFrames = deltaChan->GetNumFrames();
        DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
        unsigned short numDofs = deltaData->GetNumDofs();
        unsigned short *dofIndices = deltaChan->GetDofIndices();

        int idof;
        int iquat;

        int floorFrame = FloatToInt(currTime);
        int ceilFrame = floorFrame + 1;
        EvalToPrevValues(floorFrame);

        if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
            for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
                *reinterpret_cast<UMath::Vector4 *>(&sqt[dofIndices[idof]]) = *reinterpret_cast<UMath::Vector4 *>(&mPrevValues[iquat]);
            }
        } else {
            float t = currTime - static_cast<float>(floorFrame);
            deltaData->DecompressValuesIndexed(0, numDofs, floorFrame, ceilFrame, mPrevValues, sqt, 4, dofIndices, t);
        }
    } else {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    return true;
}

bool FnDeltaQuatChan::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevFrame = -1;
    }

    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(mpAnim);
    int numFrames = deltaChan->GetNumFrames();
    DeltaCompressedData *deltaData = deltaChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *dofIndices = deltaChan->GetDofIndices();

    int idof;
    int iquat;

    int boneIdx;
    if (mDofMask == nullptr && numDofs != 0) {
        mDofMask = reinterpret_cast<unsigned short *>(MemoryPoolManager::NewBlock(numDofs * sizeof(*mDofMask)));
        for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
            boneIdx = (dofIndices[idof] / 0xCu) & 0xFF; // TODO magic
            if (boneMask->GetBone(boneIdx)) {
                mDofMask[mNumDofs++] = idof;
            }
        }
    }

    int floorFrame = FloatToInt(currTime);
    int ceilFrame = floorFrame + 1;
    EvalToPrevValues(floorFrame, 4, mNumDofs, mDofMask);

    if (currTime == static_cast<float>(floorFrame) || ceilFrame >= numFrames) {
        for (idof = 0; idof < mNumDofs; idof++) {
            *reinterpret_cast<UMath::Vector4 *>(&sqt[dofIndices[mDofMask[idof]]]) =
                *reinterpret_cast<UMath::Vector4 *>(&mPrevValues[mDofMask[idof] * 4]);
        }
    } else {
        float t = currTime - static_cast<float>(floorFrame);
        deltaData->DecompressValuesIndexed(floorFrame, ceilFrame, mPrevValues, sqt, 4, dofIndices, t, mNumDofs, mDofMask);
    }
    return true;
}

FnKeyDeltaChan::~FnKeyDeltaChan() {
    if (mPrevValues) {
        MemoryPoolManager::DeleteBlock(mPrevValues);
    }
    if (mDofMask) {
        MemoryPoolManager::DeleteBlock(mDofMask);
    }
}

void FnKeyDeltaChan::SetAnimMemoryMap(AnimMemoryMap *anim) {
    DeltaChan *deltaChan = reinterpret_cast<DeltaChan *>(anim);
    if (mPrevValues) {
        if (deltaChan->GetNumDofs() >= reinterpret_cast<DeltaChan *>(mpAnim)->GetNumDofs()) {
            MemoryPoolManager::DeleteBlock(mPrevValues);
            mPrevValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaChan->GetNumDofs() * sizeof(*mPrevValues)));
        }
    } else {
        mPrevValues = reinterpret_cast<float *>(MemoryPoolManager::NewBlock(deltaChan->GetNumDofs() * sizeof(*mPrevValues)));
    }
    mpAnim = deltaChan;
    mPrevKey = -1;
}

void FnKeyDeltaChan::EvalToPrevValues(int key) {
    KeyDeltaChan *keyChan = reinterpret_cast<KeyDeltaChan *>(mpAnim);
    DeltaCompressedData *deltaData = keyChan->GetDeltaData();
    unsigned short numDofs = keyChan->GetNumDofs();

    deltaData->DecompressValues(0, numDofs, mPrevKey, key, mPrevValues, mPrevValues);
    mPrevKey = key;
}

void FnKeyDeltaChan::EvalToPrevValues(int key, int numDofPerBone, int, const unsigned short *) {
    KeyDeltaChan *keyChan = reinterpret_cast<KeyDeltaChan *>(mpAnim);
    DeltaCompressedData *deltaData = keyChan->GetDeltaData();

    deltaData->DecompressValues(numDofPerBone, mPrevKey, key, mPrevValues, mPrevValues, mNumDofs, mDofMask);
    mPrevKey = key;
}

int FnKeyDeltaChan::FindLowerKey(float currTime) {
    int floorTime = FloatToInt(currTime);
    KeyDeltaChan *keyChan = reinterpret_cast<KeyDeltaChan *>(mpAnim);
    int numKeys = keyChan->GetNumKeys();
    unsigned short *keyTimes = keyChan->GetKeyTimes();

    if (currTime < keyTimes[0]) {
        return 0;
    }

    int lowerKey; // r3
    if (mPrevKey <= 0) {
        lowerKey = 0;
    } else {
        lowerKey = mPrevKey - 1;
    }

    if (keyTimes[lowerKey] <= floorTime) {
        int timeIndex; // TODO how is this used?
        while (lowerKey < numKeys - 2 && keyTimes[lowerKey + 1] <= floorTime) {
            lowerKey++;
        }
    } else {
        while (lowerKey > 0 && keyTimes[lowerKey] > floorTime) {
            lowerKey--;
        }
    }

    return lowerKey + 1;
}

void FnKeyLerpChan::Eval(float prevTime, float currTime, float *evalBuffer) {
    EvalSQT(currTime, evalBuffer, nullptr);
}

bool FnKeyLerpChan::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!boneMask) {
        if (mBoneMask) {
            mPrevKey = -1;
            mBoneMask = nullptr;
        }
        int lowKey = FindLowerKey(currTime);
        EvalToPrevValues(lowKey);

        KeyLerpChan *keyChan = reinterpret_cast<KeyLerpChan *>(mpAnim);
        int numKeys = keyChan->GetNumKeys();
        DeltaCompressedData *deltaData = keyChan->GetDeltaData();
        unsigned short numDofs = deltaData->GetNumDofs();
        unsigned short *keyTimes = keyChan->GetKeyTimes();
        unsigned short *dofIndices = keyChan->GetDofIndices();

        int lowKeyTime;
        if (lowKey == 0) {
            lowKeyTime = 0;
        } else {
            lowKeyTime = keyTimes[lowKey - 1];
        };

        if (lowKeyTime == currTime || (lowKey == numKeys - 1 && currTime > keyTimes[numKeys - 2]) || (lowKey == 0 && currTime < 0.0f)) {
            for (int idof = 0; idof < numDofs; idof++) {
                sqt[dofIndices[idof]] = mPrevValues[idof];
            }
        } else {
            float t = (currTime - lowKeyTime) / (keyTimes[lowKey] - lowKeyTime);
            deltaData->DecompressValuesIndexed(0, numDofs, lowKey, lowKey + 1, mPrevValues, sqt, 1, dofIndices, t);
        }
    } else {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    return true;
}

bool FnKeyLerpChan::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevKey = -1;
    }

    KeyLerpChan *keyChan = reinterpret_cast<KeyLerpChan *>(mpAnim);
    int numKeys = keyChan->GetNumKeys();
    DeltaCompressedData *deltaData = keyChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *keyTimes = keyChan->GetKeyTimes();
    unsigned short *dofIndices = keyChan->GetDofIndices();

    int idof;
    int boneIdx;
    if (mDofMask == nullptr && numDofs != 0) {
        mDofMask = reinterpret_cast<unsigned short *>(MemoryPoolManager::NewBlock(numDofs * sizeof(*mDofMask)));
        for (idof = 0; idof < numDofs; idof++) {
            boneIdx = (dofIndices[idof] / 0xCu) & 0xFF; // TODO magic
            if (boneMask->GetBone(boneIdx)) {
                mDofMask[mNumDofs++] = idof;
            }
        }
    }

    int lowKey = FindLowerKey(currTime);
    EvalToPrevValues(lowKey, 1, mNumDofs, mDofMask);
    int lowKeyTime;
    if (lowKey == 0) {
        lowKeyTime = 0;
    } else {
        lowKeyTime = keyTimes[lowKey - 1];
    };

    if (lowKeyTime == currTime || (lowKey == numKeys - 1 && currTime > keyTimes[numKeys - 2]) || (lowKey == 0 && currTime < 0.0f)) {
        for (idof = 0; idof < mNumDofs; idof++) {
            sqt[dofIndices[mDofMask[idof]]] = mPrevValues[mDofMask[idof]];
        }
    } else {
        float t = (currTime - lowKeyTime) / (keyTimes[lowKey] - lowKeyTime);
        deltaData->DecompressValuesIndexed(lowKey, lowKey + 1, mPrevValues, sqt, 1, dofIndices, t, mNumDofs, mDofMask);
    }
    return true;
}
void FnKeyQuatChan::Eval(float prevTime, float currTime, float *evalBuffer) {
    EvalSQT(currTime, evalBuffer, nullptr);
}

bool FnKeyQuatChan::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!boneMask) {
        if (mBoneMask) {
            mPrevKey = -1;
            mBoneMask = nullptr;
        }

        KeyQuatChan *keyChan = reinterpret_cast<KeyQuatChan *>(mpAnim);
        int numKeys = keyChan->GetNumKeys();
        DeltaCompressedData *deltaData = keyChan->GetDeltaData();
        unsigned short numDofs = deltaData->GetNumDofs();
        unsigned short *keyTimes = keyChan->GetKeyTimes();
        unsigned short *dofIndices = keyChan->GetDofIndices();

        int lowKey = FindLowerKey(currTime);
        EvalToPrevValues(lowKey);

        int iquat;
        int idof;

        int lowKeyTime;
        if (lowKey == 0) {
            lowKeyTime = 0;
        } else {
            lowKeyTime = keyTimes[lowKey - 1];
        };

        if (lowKeyTime == currTime || (lowKey == numKeys - 1 && currTime > keyTimes[numKeys - 2]) || (lowKey == 0 && currTime < 0.0f)) {
            for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
                *reinterpret_cast<UMath::Vector4 *>(&sqt[dofIndices[idof]]) = *reinterpret_cast<UMath::Vector4 *>(&mPrevValues[iquat]);
            }
        } else {
            float t = (currTime - lowKeyTime) / (keyTimes[lowKey] - lowKeyTime);
            UMath::Vector4 ceilQuat;
            for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
                deltaData->DecompressValues(iquat, 4, lowKey, lowKey + 1, &mPrevValues[iquat], reinterpret_cast<float *>(&ceilQuat));

                float *blendValue = &sqt[dofIndices[idof]];
                FastQuatBlendF4(t, &mPrevValues[iquat], reinterpret_cast<float *>(&ceilQuat), blendValue);
            }
        }
    } else {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    return true;
}

bool FnKeyQuatChan::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevKey = -1;
    }

    KeyQuatChan *keyChan = reinterpret_cast<KeyQuatChan *>(mpAnim);
    int numKeys = keyChan->GetNumKeys();
    DeltaCompressedData *deltaData = keyChan->GetDeltaData();
    unsigned short numDofs = deltaData->GetNumDofs();
    unsigned short *keyTimes = keyChan->GetKeyTimes();
    unsigned short *dofIndices = keyChan->GetDofIndices();

    int idof;
    int iquat;
    int boneIdx;
    if (mDofMask == nullptr && numDofs != 0) {
        mDofMask = reinterpret_cast<unsigned short *>(MemoryPoolManager::NewBlock(numDofs * sizeof(*mDofMask)));
        for (iquat = 0, idof = 0; iquat < numDofs; iquat += 4, idof++) {
            boneIdx = (dofIndices[idof] / 0xCu) & 0xFF; // TODO magic
            if (boneMask->GetBone(boneIdx)) {
                mDofMask[mNumDofs++] = idof;
            }
        }
    }

    int lowKey = FindLowerKey(currTime);
    EvalToPrevValues(lowKey, 4, mNumDofs, mDofMask);
    int lowKeyTime;
    if (lowKey == 0) {
        lowKeyTime = 0;
    } else {
        lowKeyTime = keyTimes[lowKey - 1];
    };

    if (lowKeyTime == currTime || (lowKey == numKeys - 1 && currTime > keyTimes[numKeys - 2]) || (lowKey == 0 && currTime < 0.0f)) {
        for (idof = 0; idof < mNumDofs; idof++) {
            *reinterpret_cast<UMath::Vector4 *>(&sqt[dofIndices[mDofMask[idof]]]) =
                *reinterpret_cast<UMath::Vector4 *>(&mPrevValues[mDofMask[idof] * 4]);
        }
    } else {
        float t = (currTime - lowKeyTime) / (keyTimes[lowKey] - lowKeyTime);
        UMath::Vector4 ceilQuat;
        for (idof = 0; idof < mNumDofs; idof++) {
            deltaData->DecompressValues(mDofMask[idof] * 4, 4, lowKey, lowKey + 1, &mPrevValues[mDofMask[idof] * 4],
                                        reinterpret_cast<float *>(&ceilQuat));

            float *blendValue = &sqt[dofIndices[mDofMask[idof]]];
            FastQuatBlendF4(t, &mPrevValues[mDofMask[idof] * 4], reinterpret_cast<float *>(&ceilQuat), blendValue);
        }
    }
    return true;
}

// TODO move
float qt0[7];

// TODO move
void QuatF4(float *&data, float *output) {
    output[0] = *data++;
    output[1] = *data++;
    output[2] = *data++;
    output[3] = *data++;
}

// TODO inline and move
void EulF3(float *&data, float *output) {
    const float degreesToRadians = 0.017453294f;
    float x = *data++ * degreesToRadians * 0.5f;
    float y = *data++ * degreesToRadians * 0.5f;
    float z = *data++ * degreesToRadians * 0.5f;
    float cx = cosf(x);
    float cy = cosf(y);
    float cz = cosf(z);
    float sx = sinf(x);
    float sy = sinf(y);
    float sz = sinf(z);

    output[0] = cy * sx * cz - sy * cx * sz;
    output[2] = cy * cx * sz - sy * sx * cz;
    output[1] = cy * sx * sz + sy * cx * cz;
    output[3] = cy * cx * cz + sy * sx * sz;
}

// TODO inline and move
void TranF3(float *&data, float *output) {
    output[4] = *data++;
    output[5] = *data++;
    output[6] = *data++;
}

// TODO inline and move
void QuatF4Interp(float w, float *&data0, float *&data1, float *output) {
    qt0[0] = *data0++;
    qt0[1] = *data0++;
    qt0[2] = *data0++;
    qt0[3] = *data0++;

    output[0] = *data1++;
    output[1] = *data1++;
    output[2] = *data1++;
    output[3] = *data1++;

    if (qt0[0] * output[0] + qt0[1] * output[1] + qt0[2] * output[2] + qt0[3] * output[3] > 0.0f) {
        output[0] = w * (output[0] - qt0[0]) + qt0[0];
        output[1] = w * (output[1] - qt0[1]) + qt0[1];
        output[2] = w * (output[2] - qt0[2]) + qt0[2];
        output[3] = w * (output[3] - qt0[3]) + qt0[3];
    } else {
        output[0] = qt0[0] - w * (output[0] + qt0[0]);
        output[1] = qt0[1] - w * (output[1] + qt0[1]);
        output[2] = qt0[2] - w * (output[2] + qt0[2]);
        output[3] = qt0[3] - w * (output[3] + qt0[3]);
    }

    float invNorm = 1.0f / sqrtf(output[0] * output[0] + output[1] * output[1] + output[2] * output[2] + output[3] * output[3]);

    output[0] = output[0] * invNorm;
    output[3] = output[3] * invNorm;
    output[1] = output[1] * invNorm;
    output[2] = output[2] * invNorm;
}

// TODO inline and move
void EulF3Interp(float w, float *&data0, float *&data1, float *output) {
    const float degreesToRadians = 0.017453294f;

    float x = *data0++ * degreesToRadians * 0.5f;
    float y = *data0++ * degreesToRadians * 0.5f;
    float z = *data0++ * degreesToRadians * 0.5f;
    float cx = cosf(x);
    float cy = cosf(y);
    float cz = cosf(z);
    float sx = sinf(x);
    float sy = sinf(y);
    float sz = sinf(z);

    qt0[0] = cy * sx * cz - sy * cx * sz;
    qt0[2] = cy * cx * sz - sy * sx * cz;
    qt0[1] = cy * sx * sz + sy * cx * cz;
    qt0[3] = cy * cx * cz + sy * sx * sz;

    x = *data1++ * degreesToRadians * 0.5f;
    y = *data1++ * degreesToRadians * 0.5f;
    z = *data1++ * degreesToRadians * 0.5f;
    cx = cosf(x);
    cy = cosf(y);
    cz = cosf(z);
    sx = sinf(x);
    sy = sinf(y);
    sz = sinf(z);

    output[0] = cy * sx * cz - sy * cx * sz;
    output[2] = cy * cx * sz - sy * sx * cz;
    output[1] = cy * sx * sz + sy * cx * cz;
    output[3] = cy * cx * cz + sy * sx * sz;

    if (qt0[0] * output[0] + qt0[1] * output[1] + qt0[2] * output[2] + qt0[3] * output[3] > 0.0f) {
        output[0] = w * (output[0] - qt0[0]) + qt0[0];
        output[1] = w * (output[1] - qt0[1]) + qt0[1];
        output[2] = w * (output[2] - qt0[2]) + qt0[2];
        output[3] = w * (output[3] - qt0[3]) + qt0[3];
    } else {
        output[0] = qt0[0] - w * (output[0] + qt0[0]);
        output[1] = qt0[1] - w * (output[1] + qt0[1]);
        output[2] = qt0[2] - w * (output[2] + qt0[2]);
        output[3] = qt0[3] - w * (output[3] + qt0[3]);
    }

    float invNorm = 1.0f / sqrtf(output[0] * output[0] + output[1] * output[1] + output[2] * output[2] + output[3] * output[3]);

    output[0] = output[0] * invNorm;
    output[3] = output[3] * invNorm;
    output[1] = output[1] * invNorm;
    output[2] = output[2] * invNorm;
}

// TODO inline and move
void TranF3Interp(float w, float *&data0, float *&data1, float *output) {
    qt0[4] = *data0++;
    qt0[5] = *data0++;
    qt0[6] = *data0++;
    output[4] = *data1++;
    output[5] = *data1++;
    output[6] = *data1++;
    output[4] = qt0[4] + w * (output[4] - qt0[4]);
    output[5] = qt0[5] + w * (output[5] - qt0[5]);
    output[6] = qt0[6] + w * (output[6] - qt0[6]);
}

}; // namespace EAGL4Anim
