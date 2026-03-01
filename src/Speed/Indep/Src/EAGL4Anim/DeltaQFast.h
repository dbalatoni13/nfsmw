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
    void UnQuantize(UMath::Vector4 &q) const {}

    unsigned short mX : 12; // offset 0x0, size 0x2
    unsigned short mW0 : 4; // offset 0x0, size 0x2
    unsigned short mY : 12; // offset 0x2, size 0x2
    unsigned short mW1 : 4; // offset 0x2, size 0x2
    unsigned short mZ : 12; // offset 0x4, size 0x2
    unsigned short mW2 : 4; // offset 0x4, size 0x2
};

// total size: 0x3
struct DeltaQFastDelta {
    void UnQuantize(const DeltaQFastMinRangef &minRangef, UMath::Vector4 &q) const {}

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

    void GetArrays(DeltaQFastMinRange *&minRanges, unsigned char *&binStart, unsigned char *&constBoneIndices, DeltaQFastPhysical *&constPhysical) {}

    int GetBinSize() const {
        // TODO
        return AlignSize2(3 * ((GetBinLength() - 1) * mNumBones));
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

    DeltaQFastDelta *GetDelta(unsigned char *binData, int deltaIdx) {}

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
    unsigned char mNumConstBones;  // offset 0x7, size 0x1
    unsigned short *mTimes;        // offset 0x8, size 0x4
    unsigned char *mBoneIdxs;      // offset 0xC, size 0x4
    unsigned char mBinLengthPower; // offset 0x10, size 0x1
    unsigned char mPadding[1];     // offset 0x11, size 0x1
};

}; // namespace EAGL4Anim

#endif
