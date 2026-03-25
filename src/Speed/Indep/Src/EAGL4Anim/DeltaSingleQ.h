#ifndef EAGL4ANIM_DELTASINGLEQ_H
#define EAGL4ANIM_DELTASINGLEQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace EAGL4Anim {

// total size: 0x1C
struct DeltaSingleQMinRangef {
    float mConst0;        // offset 0x0, size 0x4
    float mConst1;        // offset 0x4, size 0x4
    float mMin[2];        // offset 0x8, size 0x8
    float mRange[2];      // offset 0x10, size 0x8
    unsigned char mIndex; // offset 0x18, size 0x1
};

// total size: 0xE
struct DeltaSingleQMinRange {
    void GetAlignment(float &c0, float &c1) {
        c0 = mConst0 * 9.5875265e-5f + -3.1415927f;
        c1 = mConst1 * 9.5875265e-5f + -3.1415927f;
    }

    void UnQuantize(DeltaSingleQMinRangef &minRangef) {
        const float RangeScale16Bit = 3.0518044e-5f;

        minRangef.mMin[0] = mMin[0] * RangeScale16Bit - 1.0f;
        minRangef.mMin[1] = mMin[1] * RangeScale16Bit - 1.0f;
        minRangef.mRange[0] = mRange[0] * 8.1381455e-6f;
        minRangef.mRange[1] = mRange[1] * 8.1381455e-6f;
        minRangef.mIndex = static_cast<unsigned char>(mIndex);
        GetAlignment(minRangef.mConst0, minRangef.mConst1);
    }

    unsigned short mConst0;   // offset 0x0, size 0x2
    unsigned short mConst1;   // offset 0x2, size 0x2
    unsigned short mMin[2];   // offset 0x4, size 0x4
    unsigned short mRange[2]; // offset 0x8, size 0x4
    unsigned short mIndex;    // offset 0xC, size 0x2
};

// total size: 0x2
struct DeltaSingleQPhysical {
    void UnQuantize(int index, UMath::Vector4 &q) const {
        const float RangeScale8Bit = 7.8431377e-3f;

        q.z = 0.0f;
        q.y = 0.0f;
        q.x = 0.0f;

        if (index == 0) {
            q.x = mV * RangeScale8Bit - 1.0f;
        } else if (index == 1) {
            q.y = mV * RangeScale8Bit - 1.0f;
        } else {
            q.z = mV * RangeScale8Bit - 1.0f;
        }
        q.w = mW * RangeScale8Bit - 1.0f;
    }

    unsigned char mV; // offset 0x0, size 0x1
    unsigned char mW; // offset 0x1, size 0x1
};

// total size: 0x1
struct DeltaSingleQDelta {
    void UnQuantize(const DeltaSingleQMinRangef &minRangef, UMath::Vector4 &q) {
        q.z = 0.0f;
        q.y = 0.0f;
        q.x = 0.0f;

        if (minRangef.mIndex == 0) {
            q.x = minRangef.mRange[0] * mV + minRangef.mMin[0];
        } else if (minRangef.mIndex == 1) {
            q.y = minRangef.mRange[0] * mV + minRangef.mMin[0];
        } else {
            q.z = minRangef.mRange[0] * mV + minRangef.mMin[0];
        }
        q.w = minRangef.mRange[1] * mW + minRangef.mMin[1];
    }

    unsigned char mV : 4; // offset 0x0, size 0x1
    unsigned char mW : 4; // offset 0x0, size 0x1
};

// total size: 0x10
struct DeltaSingleQ : public AnimMemoryMap {
    static int ComputeSize(int numBones, int numConst, int numFrames, int binLen, bool useKeyFrames) {
        // const int binSize;
        // int s;
        // int r;
    }

    int GetNumFrames() const {
        if (!mTimes) {
            return mNumKeys;
        } else {
            return mTimes[mNumKeys - 2] + 1;
        }
    }

    unsigned int GetBinLength() const {
        return 1 << mBinLengthPower;
    }

    unsigned char GetBinLengthPower() const {
        return mBinLengthPower;
    }

    unsigned int GetBinLengthModMask() const {
        unsigned int result = 0x7FFFFFFFU >> (31 - mBinLengthPower);
        return result;
    }

    void GetArrays(DeltaSingleQMinRange *&minRanges, unsigned char *&binStart) {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&this[1]);

        minRanges = reinterpret_cast<DeltaSingleQMinRange *>(memBytes);
        binStart = &memBytes[mNumBones * sizeof(DeltaSingleQMinRange)];
    }

    int GetBinSize() const {
        return static_cast<int>(AlignSize2((mNumBones * sizeof(DeltaSingleQPhysical)) +
                                           ((GetBinLength() - 1) * mNumBones * sizeof(DeltaSingleQDelta))));
    }

    DeltaSingleQMinRange *GetMinRange() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&this[1]);
        return reinterpret_cast<DeltaSingleQMinRange *>(memBytes);
    }

    unsigned char *GetBin(int binIdx) {
        const int bs = GetBinSize();
        unsigned char *memPos = &reinterpret_cast<unsigned char *>(GetMinRange())[mNumBones * sizeof(DeltaSingleQMinRange)];
        return &memPos[binIdx * bs];
    }

    DeltaSingleQPhysical *GetPhysical(unsigned char *binData) {
        return reinterpret_cast<DeltaSingleQPhysical *>(binData);
    }

    DeltaSingleQDelta *GetDelta(unsigned char *binData, int deltaIdx) {
        return reinterpret_cast<DeltaSingleQDelta *>(&binData[mNumBones * sizeof(DeltaSingleQPhysical) +
                                                              (deltaIdx * mNumBones * sizeof(DeltaSingleQDelta))]);
    }

    unsigned short *GetConstBoneIdx() {
        unsigned int binLen = GetBinLength();
        const int binSize = GetBinSize();
        // TODO it's out of line
        // int numBins = mNumFrames >> GetBinLengthPower(); // r8
        // // get to the end of the bins
        // unsigned char *s = &GetBin(0)[binSize * numBins]; // r11
        // int r = mNumFrames & GetBinLengthModMask();       // r31

        // if (r > 0) {
        //     s = reinterpret_cast<unsigned char *>(AlignSize2((intptr_t)s + mNumBones * 2 + (r - 1) * GetFrameDeltaSize()));
        // }

        return reinterpret_cast<unsigned short *>(nullptr);
    }

    float *GetConstPhysical() {
        // return reinterpret_cast<float *>(AlignSize4(reinterpret_cast<intptr_t>(&GetConstBoneIdx()[mNumConstBones])));
    }

    unsigned short mNumKeys;       // offset 0x4, size 0x2
    unsigned char mNumBones;       // offset 0x6, size 0x1
    unsigned char mBinLengthPower; // offset 0x7, size 0x1
    unsigned short *mTimes;        // offset 0x8, size 0x4
    unsigned char *mBoneIdxs;      // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
