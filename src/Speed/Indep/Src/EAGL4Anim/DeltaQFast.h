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
    void UnQuantize(DeltaQFastMinRangef &minRangef) const {}

    unsigned short mMin[4];   // offset 0x0, size 0x8
    unsigned short mRange[4]; // offset 0x8, size 0x8
};

// total size: 0x6
struct DeltaQFastPhysical {
    void UnQuantize(UMath::Vector4 &q) const {
        unsigned short w = static_cast<unsigned short>((mW0 << 8) | (mW1 << 4) | mW2);

        q.x = static_cast<float>(mX) * 4.8840049e-4f - 1.0002443f;
        q.y = static_cast<float>(mY) * 4.8840049e-4f - 1.0002443f;
        q.z = static_cast<float>(mZ) * 4.8840049e-4f - 1.0002443f;
        q.w = static_cast<float>(w) * 4.8840049e-4f - 1.0002443f;
    }

    unsigned short mX : 12; // offset 0x0, size 0x2
    unsigned short mW0 : 4; // offset 0x0, size 0x2
    unsigned short mY : 12; // offset 0x2, size 0x2
    unsigned short mW1 : 4; // offset 0x2, size 0x2
    unsigned short mZ : 12; // offset 0x4, size 0x2
    unsigned short mW2 : 4; // offset 0x4, size 0x2
};

// total size: 0x3
struct DeltaQFastDelta {
    void UnQuantize(const DeltaQFastMinRangef &minRangef, UMath::Vector4 &q) const {
        unsigned char w = static_cast<unsigned char>((mW0 << 4) | (mW1 << 2) | mW2);

        q.x = minRangef.mMin.x + minRangef.mRange.x * static_cast<float>(mX) * 1.5873017e-2f;
        q.y = minRangef.mMin.y + minRangef.mRange.y * static_cast<float>(mY) * 1.5873017e-2f;
        q.z = minRangef.mMin.z + minRangef.mRange.z * static_cast<float>(mZ) * 1.5873017e-2f;
        q.w = minRangef.mMin.w + minRangef.mRange.w * static_cast<float>(w) * 1.5873017e-2f;
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

    unsigned int GetBinLengthPower() const {
        return mBinLengthPower;
    }

    unsigned int GetBinLengthModMask() const {
        unsigned int result = 0x7FFFFFFFU >> (31 - mBinLengthPower);
        return result;
    }

    void GetArrays(DeltaQFastMinRange *&minRanges, unsigned char *&binStart, unsigned char *&constBoneIndices, DeltaQFastPhysical *&constPhysical) {
        minRanges = GetMinRange();
        binStart = GetBin(0);
        constBoneIndices = reinterpret_cast<unsigned char *>(GetConstBoneIdx());
        constPhysical = reinterpret_cast<DeltaQFastPhysical *>(GetConstPhysical());
    }

    int GetBinSize() const {
        return AlignSize2(mNumBones * (6 + ((GetBinLength() - 1) * 3)));
    }

    DeltaQFastMinRange *GetMinRange() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&this[1]);
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

    DeltaQFastDelta *GetDelta(unsigned char *binData, int deltaIdx) {
        return reinterpret_cast<DeltaQFastDelta *>(&binData[mNumBones * 6 + deltaIdx * mNumBones * 3]);
    }

    unsigned char *GetConstBoneIdx();

    DeltaQFastPhysical *GetConstPhysical();

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
