#ifndef EAGL4ANIM_DELTAQ_H
#define EAGL4ANIM_DELTAQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"

namespace EAGL4Anim {

// total size: 0x18
struct DeltaQMinRangef {
    UMath::Vector3 mMin;   // offset 0x0, size 0xC
    UMath::Vector3 mRange; // offset 0xC, size 0xC
};

// total size: 0xC
struct DeltaQMinRange {
    void UnQuantize(DeltaQMinRangef &minRangef) {
        minRangef.mMin.x = mMin[0] * (2.0f / 65535.0f) - 1.0f;
        minRangef.mMin.y = mMin[1] * (2.0f / 65535.0f) - 1.0f;
        minRangef.mMin.z = mMin[2] * (2.0f / 65535.0f) - 1.0f;

        minRangef.mRange.x = mRange[0] * (2.0f / 65535.0f);
        minRangef.mRange.y = mRange[1] * (2.0f / 65535.0f);
        minRangef.mRange.z = mRange[2] * (2.0f / 65535.0f);
    }

    unsigned short mMin[3];   // offset 0x0, size 0x6
    unsigned short mRange[3]; // offset 0x6, size 0x6
};

inline void DeltaQRecoverW(int signBit, UMath::Vector4 &q) {
    float ndotn = q.x * q.x + q.y * q.y + q.z * q.z;

    if (ndotn > 1.0f) {
        float len = FastSqrt(ndotn);

        q.x /= len;
        q.y /= len;
        q.z /= len;
        q.w = 0.0f;
    } else {
        q.w = FastSqrt(1.0f - ndotn);

        if (signBit > 0) {
            q.w = -q.w;
        }
    }
}

// total size: 0x6
struct DeltaQPhysical {
    void UnQuantize(UMath::Vector4 &q) {
        const float RangeScale15Bit = 2.0f / 32767.0f;
        const float RangeScale16Bit = 2.0f / 65535.0f;

        q.x = mX * RangeScale15Bit - 1.0f;
        q.y = mY * RangeScale16Bit - 1.0f;
        q.z = mZ * RangeScale16Bit - 1.0f;

        DeltaQRecoverW(mW, q);
    }

    unsigned short mX : 15; // offset 0x0, size 0x2
    unsigned short mW : 1;  // offset 0x0, size 0x2
    unsigned short mY;      // offset 0x2, size 0x2
    unsigned short mZ;      // offset 0x4, size 0x2
};

// total size: 0x3
struct DeltaQDelta {
    static float DeQuantize7Bit(int v) {
        return v * (1.0f / 127.0f);
    }

    static float DeQuantize8Bit(int v) {
        return v * (1.0f / 255.0f);
    }

    void UnQuantize(const DeltaQMinRangef &minRangef, UMath::Vector4 &q) {
        q.x = minRangef.mRange.x * DeQuantize7Bit(mX) + minRangef.mMin.x;
        q.y = minRangef.mRange.y * DeQuantize8Bit(mY) + minRangef.mMin.y;
        q.z = minRangef.mRange.z * DeQuantize8Bit(mZ) + minRangef.mMin.z;
    }

    unsigned char mX : 7; // offset 0x0, size 0x1
    unsigned char mW : 1; // offset 0x0, size 0x1
    unsigned char mY;     // offset 0x1, size 0x1
    unsigned char mZ;     // offset 0x2, size 0x1
};

// total size: 0x14
struct DeltaQ : public AnimMemoryMap {
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

    void GetArrays(DeltaQMinRange *&minRanges, unsigned char *&binStart, unsigned char *&constBoneIndices, DeltaQPhysical *&constPhysical) {
        minRanges = reinterpret_cast<DeltaQMinRange *>(&mPadding[1]);
        binStart = &reinterpret_cast<unsigned char *>(minRanges)[mNumBones * sizeof(DeltaQMinRange)];

        constBoneIndices = GetConstBoneIdx();
        constPhysical = reinterpret_cast<DeltaQPhysical *>(GetConstPhysical());
    }

    int GetBinSize() const {
        return AlignSize2(mNumBones * ((GetBinLength() - 1) * sizeof(DeltaQDelta) + sizeof(DeltaQPhysical)));
    }

    DeltaQMinRange *GetMinRange() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&mPadding[1]);
        return reinterpret_cast<DeltaQMinRange *>(memBytes);
    }

    unsigned char *GetBin(int binIdx) {
        const int bs = GetBinSize();
        unsigned char *memPos = &reinterpret_cast<unsigned char *>(GetMinRange())[mNumBones * sizeof(DeltaQMinRange)];
        return &memPos[binIdx * bs];
    }

    DeltaQPhysical *GetPhysical(unsigned char *binData) {
        return reinterpret_cast<DeltaQPhysical *>(binData);
    }

    DeltaQDelta *GetDelta(unsigned char *binData, int deltaIdx) {
        return &reinterpret_cast<DeltaQDelta *>(&GetPhysical(binData)[mNumBones])[deltaIdx * mNumBones];
    }

    unsigned char *GetConstBoneIdx() {
        const int binSize = AlignSize2(mNumBones * ((GetBinLength() - 1) * sizeof(DeltaQDelta) + sizeof(DeltaQPhysical)));
        unsigned char *s = &GetBin(0)[binSize * (mNumKeys / GetBinLength())];
        int r = mNumKeys - (mNumKeys / GetBinLength()) * GetBinLength();

        if (r > 0) {
            s = &s[mNumBones * ((r - 1) * sizeof(DeltaQDelta) + sizeof(DeltaQPhysical))];
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
