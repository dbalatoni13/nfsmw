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
        c0 = mConst0 * (2.0f * 3.1415927f / 65535.0f) + -3.1415927f;
        c1 = mConst1 * (2.0f * 3.1415927f / 65535.0f) + -3.1415927f;
    }

    void UnQuantize(DeltaSingleQMinRangef &minRangef) {
        const float RangeScale16Bit = 2.0f / 65535.0f;

        minRangef.mMin[0] = mMin[0] * RangeScale16Bit - 1.0f;
        minRangef.mMin[1] = mMin[1] * RangeScale16Bit - 1.0f;

        minRangef.mRange[0] = mRange[0] * RangeScale16Bit;
        minRangef.mRange[1] = mRange[1] * RangeScale16Bit;

        minRangef.mIndex = mIndex;

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
        const float RangeScale8Bit = 2.0f / 255.0f;

        q.x = q.y = q.z = 0.0f;

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
    static float DeQuantize4Bit(int v) {
        return v * (1.0f / 15.0f);
    }

    void UnQuantize(const DeltaSingleQMinRangef &minRangef, UMath::Vector4 &q) {
        q.x = q.y = q.z = 0.0f;

        if (minRangef.mIndex == 0) {
            q.x = minRangef.mRange[0] * DeQuantize4Bit(mV) + minRangef.mMin[0];
        } else if (minRangef.mIndex == 1) {
            q.y = minRangef.mRange[0] * DeQuantize4Bit(mV) + minRangef.mMin[0];
        } else {
            q.z = minRangef.mRange[0] * DeQuantize4Bit(mV) + minRangef.mMin[0];
        }

        q.w = minRangef.mRange[1] * DeQuantize4Bit(mW) + minRangef.mMin[1];
    }

    unsigned char mV : 4; // offset 0x0, size 0x1
    unsigned char mW : 4; // offset 0x0, size 0x1
};

// total size: 0x10
struct DeltaSingleQ : public AnimMemoryMap {
    unsigned int GetBinLength() const {
        return 1 << mBinLengthPower;
    }

    unsigned int GetBinLengthPower() const {
        return mBinLengthPower;
    }

    unsigned int GetBinLengthModMask() const {
        unsigned int result = 0x7FFFFFFFU >> (31 - mBinLengthPower);
        return result;
    }

    void GetArrays(DeltaSingleQMinRange *&minRanges, unsigned char *&binStart) {
        minRanges = reinterpret_cast<DeltaSingleQMinRange *>(&this[1]);
        binStart = &reinterpret_cast<unsigned char *>(minRanges)[mNumBones * sizeof(DeltaSingleQMinRange)];
    }

    int GetBinSize() const {
        return AlignSize2(mNumBones * ((GetBinLength() - 1) * sizeof(DeltaSingleQDelta) + sizeof(DeltaSingleQPhysical)));
    }

    int GetNumFrames() const {
        if (!mTimes) {
            return mNumKeys;
        } else {
            return mTimes[mNumKeys - 2] + 1;
        }
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
        return &reinterpret_cast<DeltaSingleQDelta *>(&GetPhysical(binData)[mNumBones])[deltaIdx * mNumBones];
    }

    static int ComputeSize(int numKeys, int numBones, int binLen, bool useKeyFrames) {
        // const int binSize;
        // int s;
        // int r;
    }

    unsigned short mNumKeys;       // offset 0x4, size 0x2
    unsigned char mNumBones;       // offset 0x6, size 0x1
    unsigned char mBinLengthPower; // offset 0x7, size 0x1
    unsigned short *mTimes;        // offset 0x8, size 0x4
    unsigned char *mBoneIdxs;      // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
