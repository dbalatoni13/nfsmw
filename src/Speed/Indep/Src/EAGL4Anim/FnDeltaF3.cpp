#include "FnDeltaF3.h"

namespace EAGL4Anim {

FnDeltaF3::FnDeltaF3()
    : mNextKey(-1),         //
      mNextValues(nullptr), //
      mPrevKey(-1),         //
      mPrevVBlock(nullptr), //
      mPrevValues(nullptr), //
      mNextVBlock(nullptr) {
    mType = AnimTypeId::ANIM_DELTAF3;
}

void FnDeltaF3::Eval(float prevTime, float currTime, float *evalBuffer) {
    EvalSQT(currTime, evalBuffer, nullptr);
}

// https://decomp.me/scratch/YWiFo
bool FnDeltaF3::EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) {
    if (!mPrevValues) {
        InitBuffersAsRequired();
    }
    if (boneMask) {
        return EvalSQTMask(currTime, sqt, boneMask);
    }
    if (mBoneMask) {
        mBoneMask = nullptr;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaF3 *deltaF = reinterpret_cast<DeltaF3 *>(mpAnim); // r30
    int floorTime = FloatToInt(currTime);                  // r26
    int floorKey;                                          // r29
    if (!deltaF->mTimes) {
        if (floorTime >= 0) {
            if (floorTime >= deltaF->mNumFrames) {
                floorKey = deltaF->mNumFrames - 1;
            } else {
                floorKey = floorTime;
            }
        } else {
            floorKey = 0;
        }
    } else if (floorTime < deltaF->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex; // r11
        if (mPrevKey <= 0) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaF->mTimes[timeIndex] <= floorTime) {
            if (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                while (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                    timeIndex++;
                }
            }
        } else {
            while (timeIndex > 0 && deltaF->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }
    unsigned int binLenPower = deltaF->GetBinLengthPower();
    unsigned int binLenModMask = deltaF->GetBinLengthModMask(); // r7 // TODO

    int floorBinIdx = floorKey >> binLenPower;    // r21
    int floorDeltaIdx = floorKey & binLenModMask; // r28
    // TODO
    int prevBinIdx = mPrevKey >> binLenPower; // r8 and r22?
    // TODO
    int prevDeltaIdx;                                     // r10
    unsigned char *binData = deltaF->GetBin(floorBinIdx); // r12
    unsigned short *binPhys = deltaF->GetPhysical(binData);
    unsigned char *binDelta; // r6
    int frameSize = deltaF->GetFrameDeltaSize();
    UMath::Vector3 delta; // r1+0x8 TODO F3

    bool preventReverse = false; // r0
    if (floorKey < mPrevKey) {
        preventReverse = IsReverseDeltaSumEnabled() == false;
    }
    // TODO this shouldn't show a register in the dump
    // I'm pretty sure this variable is the culprit of the regswaps
    int ceilKey;
    if (floorKey == mNextKey) {
        UMath::Vector4 *tempBuf = mPrevValues;
        mPrevValues = mNextValues;
        mNextValues = tempBuf;
        prevDeltaIdx = floorDeltaIdx;
        ceilKey = floorKey + 1;
        mNextKey = -1;
        mPrevKey = floorKey;
    } else if (mPrevKey == -1 || floorBinIdx != prevBinIdx || floorDeltaIdx == 0 || preventReverse) {
        for (int idof = 0; idof < deltaF->GetNumBones(); idof++) {
            deltaF->UnQuantizePhysical(mMinRangesf[idof], &binPhys[idof * 3], *reinterpret_cast<UMath::Vector3 *>(&mPrevValues[idof]));
        }
        ceilKey = floorKey + 1;
        prevDeltaIdx = 0;
    } else {
        ceilKey = floorKey + 1;
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx);
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, delta);
                mPrevValues[ibone].x += delta.x;
                mPrevValues[ibone].y += delta.y;
                mPrevValues[ibone].z += delta.z;
            }
            binDelta += frameSize;
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx - 1);
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, delta);
                mPrevValues[ibone].x -= delta.x;
                mPrevValues[ibone].y -= delta.y;
                mPrevValues[ibone].z -= delta.z;
            }
            binDelta -= frameSize;
        }
    }
    mPrevKey = floorKey;

    float scale = 1.0f;
    bool lerpReqd;
    if (!deltaF->mTimes) {
        lerpReqd = currTime != floorTime;
        if (lerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (ceilKey == 0) {
        lerpReqd = currTime != 0.0f;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[ceilKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        // TODO why floorKey - 1 and not floorKey?
        float floorKeyTime = deltaF->mTimes[floorKey - 1];
        lerpReqd = currTime != floorKeyTime;
        if (lerpReqd) {
            // TODO why floorKey here and not ceilKey?
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    unsigned short *dofIndices = deltaF->GetDofIndices(); // r3

    if (lerpReqd && floorKey < deltaF->mNumFrames - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        UMath::Vector3 ceil;
        binData = deltaF->GetBin(ceilBinIdx);
        binPhys = deltaF->GetPhysical(binData);
        if (ceilKey == mNextKey) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                dofIndices++;
            }
        } else {
            if (ceilBinIdx != floorBinIdx) {
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++, dofIndices++) {
                    // TODO * 3 for binPhys?
                    deltaF->UnQuantizePhysical(mMinRangesf[ibone], &binPhys[ibone], *reinterpret_cast<UMath::Vector3 *>(&mNextValues[ibone]));
                    sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                    sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                    sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                }
            } else {
                int ceilDeltaIdx = floorDeltaIdx;
                binDelta = deltaF->GetDelta(binData, ceilDeltaIdx);
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                    deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, ceil);
                    mNextValues[ibone].x = mPrevValues[ibone].x + ceil.x;
                    mNextValues[ibone].y = mPrevValues[ibone].y + ceil.y;
                    mNextValues[ibone].z = mPrevValues[ibone].z + ceil.z;

                    // TODO DIFF
                    sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                    sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                    sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                    dofIndices++;
                }
            }
            mNextKey = ceilKey;
        }
    } else {
        for (int idof = 0; idof < deltaF->GetNumBones(); idof++) {
            *reinterpret_cast<UMath::Vector3 *>(&sqt[dofIndices[idof]]) = *reinterpret_cast<UMath::Vector3 *>(&mPrevValues[idof]);
        }
    }
    if (deltaF->mNumConstBones != 0) {
        unsigned short *constBoneIdxs = deltaF->GetConstBoneIdx(); // r6
        float *constPhys = deltaF->GetConstPhysical();             // r11
        int index;                                                 // r9

        for (int ibone = 0; ibone < deltaF->mNumConstBones; ibone++) {
            index = *constBoneIdxs;
            sqt[index++] = *constPhys++;
            sqt[index++] = *constPhys++;
            sqt[index++] = *constPhys++;

            constBoneIdxs++;
        }
    }

    return true;
}

// https://decomp.me/scratch/HzB0K
bool FnDeltaF3::EvalSQTMask(float currTime, float *sqt, const BoneMask *boneMask) {
    if (boneMask != mBoneMask) {
        mBoneMask = boneMask;
        mPrevKey = -1;
        mNextKey = -1;
    }

    DeltaF3 *deltaF = reinterpret_cast<DeltaF3 *>(mpAnim); // r30
    int floorTime = FloatToInt(currTime);                  // r26
    int floorKey;                                          // r29
    if (!deltaF->mTimes) {
        if (floorTime >= 0) {
            if (floorTime >= deltaF->mNumFrames) {
                floorKey = deltaF->mNumFrames - 1;
            } else {
                floorKey = floorTime;
            }
        } else {
            floorKey = 0;
        }
    } else if (floorTime < deltaF->mTimes[0]) {
        floorKey = 0;
    } else {
        int timeIndex; // r11
        if (mPrevKey <= 0) {
            timeIndex = 0;
        } else {
            timeIndex = mPrevKey - 1;
        }
        if (deltaF->mTimes[timeIndex] <= floorTime) {
            if (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                while (timeIndex < deltaF->mNumFrames - 2 && deltaF->mTimes[timeIndex + 1] <= floorTime) {
                    timeIndex++;
                }
            }
        } else {
            while (timeIndex > 0 && deltaF->mTimes[timeIndex] > floorTime) {
                timeIndex--;
            }
        }

        floorKey = timeIndex + 1;
    }
    unsigned int binLenPower = deltaF->GetBinLengthPower();
    unsigned int binLenModMask = deltaF->GetBinLengthModMask(); // r7 // TODO

    int floorBinIdx = floorKey >> binLenPower;    // r21
    int floorDeltaIdx = floorKey & binLenModMask; // r28
    // TODO
    int prevBinIdx = mPrevKey >> binLenPower; // r8 and r22?
    // TODO
    int prevDeltaIdx;                                     // r10
    unsigned char *binData = deltaF->GetBin(floorBinIdx); // r12
    unsigned short *binPhys = deltaF->GetPhysical(binData);
    unsigned char *binDelta; // r6
    int frameSize = deltaF->GetFrameDeltaSize();
    UMath::Vector3 delta; // r1+0x8 TODO F3

    unsigned short *dofIdxs = deltaF->GetDofIndices();
    unsigned char boneIdxs[80]; // r1+0x8
    int idof;                   // r12
    for (idof = 0; idof < deltaF->GetNumBones(); idof++) {
        boneIdxs[idof] = (dofIdxs[idof] / 0xC);
    }

    bool preventReverse = false; // r0
    if (floorKey < mPrevKey) {
        preventReverse = IsReverseDeltaSumEnabled() == false;
    }
    // TODO this shouldn't show a register in the dump
    // I'm pretty sure this variable is the culprit of the regswaps
    int ceilKey;
    if (floorKey == mNextKey) {
        UMath::Vector4 *tempBuf = mPrevValues;
        mPrevValues = mNextValues;
        mNextValues = tempBuf;
        prevDeltaIdx = floorDeltaIdx;
        ceilKey = floorKey + 1;
        mNextKey = -1;
        mPrevKey = floorKey;
    } else if (mPrevKey == -1 || floorBinIdx != prevBinIdx || preventReverse) {
        for (idof = 0; idof < deltaF->GetNumBones(); idof++) {
            if (boneMask->GetBone(boneIdxs[idof])) {
                deltaF->UnQuantizePhysical(mMinRangesf[idof], &binPhys[idof * 3], *reinterpret_cast<UMath::Vector3 *>(&mPrevValues[idof]));
            }
        }
        ceilKey = floorKey + 1;
        prevDeltaIdx = 0;
    } else {
        ceilKey = floorKey + 1;
        prevDeltaIdx = mPrevKey & binLenModMask;
    }

    if (prevDeltaIdx < floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx);
        for (int iframe = prevDeltaIdx; iframe < floorDeltaIdx; iframe++) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, delta);
                    mPrevValues[ibone].x += delta.x;
                    mPrevValues[ibone].y += delta.y;
                    mPrevValues[ibone].z += delta.z;
                }
            }
            binDelta += frameSize;
        }
    } else if (prevDeltaIdx > floorDeltaIdx) {
        binDelta = deltaF->GetDelta(binData, prevDeltaIdx - 1);
        for (int iframe = prevDeltaIdx - 1; iframe >= floorDeltaIdx; iframe--) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, delta);
                    mPrevValues[ibone].x -= delta.x;
                    mPrevValues[ibone].y -= delta.y;
                    mPrevValues[ibone].z -= delta.z;
                }
            }
            binDelta -= frameSize;
        }
    }
    mPrevKey = floorKey;

    float scale = 1.0f;
    bool lerpReqd;
    if (!deltaF->mTimes) {
        lerpReqd = currTime != floorTime;
        if (lerpReqd) {
            scale = currTime - floorTime;
        }
    } else if (ceilKey == 0) {
        lerpReqd = currTime != 0.0f;
        if (lerpReqd) {
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[ceilKey]);
            scale = currTime / ceilKeyTime;
        }
    } else {
        // TODO why floorKey - 1 and not floorKey?
        float floorKeyTime = deltaF->mTimes[floorKey - 1];
        lerpReqd = currTime != floorKeyTime;
        if (lerpReqd) {
            // TODO why floorKey here and not ceilKey?
            float ceilKeyTime = static_cast<float>(deltaF->mTimes[floorKey]);
            scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
        }
    }

    unsigned short *dofIndices = deltaF->GetDofIndices(); // r3

    if (lerpReqd && floorKey < deltaF->mNumFrames - 1) {
        int ceilBinIdx = ceilKey >> binLenPower;
        UMath::Vector3 ceil;
        binData = deltaF->GetBin(ceilBinIdx);
        binPhys = deltaF->GetPhysical(binData);
        if (ceilKey == mNextKey) {
            for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                if (boneMask->GetBone(boneIdxs[ibone])) {
                    sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                    sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                    sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                }
                dofIndices++;
            }
        } else {
            if (ceilBinIdx != floorBinIdx) {
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++, dofIndices++) {
                    // TODO * 3 for binPhys?
                    if (boneMask->GetBone(boneIdxs[ibone])) {
                        deltaF->UnQuantizePhysical(mMinRangesf[ibone], &binPhys[ibone], *reinterpret_cast<UMath::Vector3 *>(&mNextValues[ibone]));
                        sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                        sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                        sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                    }
                }
            } else {
                int ceilDeltaIdx = floorDeltaIdx;
                binDelta = deltaF->GetDelta(binData, ceilDeltaIdx);
                for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
                    if (boneMask->GetBone(boneIdxs[ibone])) {
                        deltaF->UnQuantizeDelta(mMinRangesf[ibone], binDelta, ibone, ceil);
                        mNextValues[ibone].x = mPrevValues[ibone].x + ceil.x;
                        mNextValues[ibone].y = mPrevValues[ibone].y + ceil.y;
                        mNextValues[ibone].z = mPrevValues[ibone].z + ceil.z;

                        // TODO DIFF
                        sqt[dofIndices[0]] = mPrevValues[ibone].x + (mNextValues[ibone].x - mPrevValues[ibone].x) * scale;
                        sqt[dofIndices[1]] = mPrevValues[ibone].y + (mNextValues[ibone].y - mPrevValues[ibone].y) * scale;
                        sqt[dofIndices[2]] = mPrevValues[ibone].z + (mNextValues[ibone].z - mPrevValues[ibone].z) * scale;
                    }
                    dofIndices++;
                }
            }
            mNextKey = ceilKey;
        }
    } else {
        for (int ibone = 0; ibone < deltaF->GetNumBones(); ibone++) {
            if (boneMask->GetBone(boneIdxs[ibone])) {
                *reinterpret_cast<UMath::Vector3 *>(&sqt[dofIndices[ibone]]) = *reinterpret_cast<UMath::Vector3 *>(&mPrevValues[ibone]);
            }
        }
    }
    if (deltaF->mNumConstBones != 0) {
        unsigned short *constBoneIdxs = deltaF->GetConstBoneIdx(); // r3
        float *constPhys = deltaF->GetConstPhysical();             // r8
        unsigned short dofIndex;                                   // r10
        unsigned short boneIndex;
        int ibone; // r5

        for (ibone = 0; ibone < deltaF->mNumConstBones; ibone++) {
            dofIndex = *constBoneIdxs++;
            boneIndex = dofIndex / 0xC;
            if (boneMask->GetBone(boneIndex)) {
                sqt[dofIndex++] = *constPhys++;
                sqt[dofIndex++] = *constPhys++;
                sqt[dofIndex++] = *constPhys++;
            } else {
                constPhys += 3;
            }
        }
    }

    return true;
}

bool FnDeltaF3::EvalWeights(float currTime, float *weights) {
    Eval(currTime, currTime, weights);
    return true;
}

bool FnDeltaF3::EvalVel2D(float currTime, float *vel) {
    Eval(currTime, currTime, vel);
    return true;
}

void FnDeltaF3::InitBuffersAsRequired() {
    DeltaF3 *deltaF = reinterpret_cast<DeltaF3 *>(mpAnim);

    if (deltaF->GetNumBones() != 0) {
        mPrevValues = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mPrevValues)));
        mPrevVBlock = mPrevValues;

        mNextValues = reinterpret_cast<UMath::Vector4 *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mNextValues)));
        mNextVBlock = mNextValues;

        mMinRangesf = reinterpret_cast<DeltaF3MinRange *>(MemoryPoolManager::NewBlock(deltaF->GetNumBones() * sizeof(*mMinRangesf)));

        DeltaF3::DofInfo *dofInfo = deltaF->GetDofInfo();

        for (int b = 0; b < deltaF->GetNumBones(); b++) {
            for (int c = 0; c < 3; c++) {
                mMinRangesf[b].mPhysMin[c] = dofInfo->mPhysMin[c];

                mMinRangesf[b].mPhysRange[c] = dofInfo->mPhysRange[c];
                // TODO magic
                mMinRangesf[b].mPhysRange[c] *= 1.5259022e-5f;

                // TODO magic
                mMinRangesf[b].mDeltaMin[c] = 2 * (dofInfo->mQuantMin[c] * 1.5259022e-5f) * dofInfo->mPhysRange[c] - dofInfo->mPhysRange[c];

                mMinRangesf[b].mDeltaRange[c] = 2 * (dofInfo->mQuantRange[c] * 1.5259022e-5f) * dofInfo->mPhysRange[c];
                mMinRangesf[b].mDeltaRange[c] *= 0.003921569f;
            }
            dofInfo++;
        }
    }
}

}; // namespace EAGL4Anim
