#ifndef EAGL4ANIM_DELTAQFAST_H
#define EAGL4ANIM_DELTAQFAST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace EAGL4Anim {

// total size: 0x20
struct DeltaQFastMinRangef {
    UMath::Vector4 mMin;   // offset 0x0, size 0x10
    UMath::Vector4 mRange; // offset 0x10, size 0x10
};

// total size: 0x10
struct DeltaQFastMinRange {
    void UnQuantize(DeltaQFastMinRangef &minRangef) const {
        minRangef.mMin.x = mMin[0] * (2.0f / 65535.0f) - 1.0f;
        minRangef.mMin.y = mMin[1] * (2.0f / 65535.0f) - 1.0f;
        minRangef.mMin.z = mMin[2] * (2.0f / 65535.0f) - 1.0f;
        minRangef.mMin.w = mMin[3] * (2.0f / 65535.0f) - 1.0f;

        minRangef.mRange.x = mRange[0] * (2.0f / 65535.0f);
        minRangef.mRange.y = mRange[1] * (2.0f / 65535.0f);
        minRangef.mRange.z = mRange[2] * (2.0f / 65535.0f);
        minRangef.mRange.w = mRange[3] * (2.0f / 65535.0f);
    }

    unsigned short mMin[4];   // offset 0x0, size 0x8
    unsigned short mRange[4]; // offset 0x8, size 0x8
};

// total size: 0x6
struct DeltaQFastPhysical {
    static int PackW(int w0, int w1) {
        return (w0 << 8) + (w1 << 4);
    }

    void UnQuantize(UMath::Vector4 &q) const {
        q.x = mX * (2.0f / 4095.0f) - 1.0f;
        q.y = mY * (2.0f / 4095.0f) - 1.0f;
        q.z = mZ * (2.0f / 4095.0f) - 1.0f;

        unsigned short w = mW2 | PackW(mW0, mW1);

        q.w = w * (2.0f / 4095.0f) - 1.0f;
    }

    unsigned short mX : 12; // offset 0x0, size 0x2
    unsigned char mW0 : 4;  // offset 0x0, size 0x2
    unsigned short mY : 12; // offset 0x2, size 0x2
    unsigned char mW1 : 4;  // offset 0x2, size 0x2
    unsigned short mZ : 12; // offset 0x4, size 0x2
    unsigned char mW2 : 4;  // offset 0x4, size 0x2
};

// total size: 0x3
struct DeltaQFastDelta {
    static float DeQuantize(int v) {
        return v * (1.0f / 63.0f);
    }

    void UnQuantize(const DeltaQFastMinRangef &minRangef, UMath::Vector4 &q) const {
        q.x = minRangef.mRange.x * DeQuantize(mX) + minRangef.mMin.x;
        q.y = minRangef.mRange.y * DeQuantize(mY) + minRangef.mMin.y;
        q.z = minRangef.mRange.z * DeQuantize(mZ) + minRangef.mMin.z;

        unsigned char w = (mW0 << 4) + (mW1 << 2) | mW2;

        q.w = minRangef.mRange.w * DeQuantize(w) + minRangef.mMin.w;
    }

    unsigned char mX : 6;  // offset 0x0, size 0x1
    unsigned char mW0 : 2; // offset 0x0, size 0x1
    unsigned char mY : 6;  // offset 0x1, size 0x1
    unsigned char mW1 : 2; // offset 0x1, size 0x1
    unsigned char mZ : 6;  // offset 0x2, size 0x1
    unsigned char mW2 : 2; // offset 0x2, size 0x1
};

// total size: 0x14
struct DeltaQFast : public AnimMemoryMap {
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

    void GetArrays(DeltaQFastMinRange *&minRanges, unsigned char *&binStart, unsigned char *&constBoneIndices, DeltaQFastPhysical *&constPhysical) {
        minRanges = reinterpret_cast<DeltaQFastMinRange *>(&mPadding[1]);
        binStart = &reinterpret_cast<unsigned char *>(minRanges)[mNumBones * sizeof(DeltaQFastMinRange)];

        constBoneIndices = GetConstBoneIdx();
        constPhysical = reinterpret_cast<DeltaQFastPhysical *>(GetConstPhysical());
    }

    int GetBinSize() const {
        return AlignSize2(mNumBones * ((GetBinLength() - 1) * sizeof(DeltaQFastDelta) + sizeof(DeltaQFastPhysical)));
    }

    DeltaQFastMinRange *GetMinRange() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&mPadding[1]);
        return reinterpret_cast<DeltaQFastMinRange *>(memBytes);
    }

    unsigned char *GetBin(int binIdx) {
        const int bs = GetBinSize();
        unsigned char *memPos = &reinterpret_cast<unsigned char *>(GetMinRange())[mNumBones * sizeof(DeltaQFastMinRange)];
        return &memPos[binIdx * bs];
    }

    DeltaQFastPhysical *GetPhysical(unsigned char *binData) {
        return reinterpret_cast<DeltaQFastPhysical *>(binData);
    }

    DeltaQFastDelta *GetDelta(unsigned char *binData, int deltaIdx) {}

    unsigned char *GetConstBoneIdx() {
        const int binSize = AlignSize2(mNumBones * ((GetBinLength() - 1) * sizeof(DeltaQFastDelta) + sizeof(DeltaQFastPhysical)));
        unsigned char *s = &GetBin(0)[binSize * (mNumKeys / GetBinLength())];
        int r = mNumKeys - (mNumKeys / GetBinLength()) * GetBinLength();

        if (r > 0) {
            s = &s[mNumBones * ((r - 1) * sizeof(DeltaQFastDelta) + sizeof(DeltaQFastPhysical))];
        }

        return s;
    }

    float *GetConstPhysical() {
        return reinterpret_cast<float *>(AlignSize2(reinterpret_cast<intptr_t>(&GetConstBoneIdx()[mNumConstBones])));
    }

    unsigned short mNumKeys;       // offset 0x4, size 0x2
    unsigned char mNumBones;       // offset 0x6, size 0x1
    unsigned char mNumConstBones;  // offset 0x7, size 0x1
    unsigned short *mTimes;        // offset 0x8, size 0x4
    unsigned char *mBoneIdxs;      // offset 0xC, size 0x4
    unsigned char mBinLengthPower; // offset 0x10, size 0x1
    unsigned char mPadding[1];     // offset 0x11, size 0x1
};

}; // namespace EAGL4Anim

#endif
