#ifndef EAGL4ANIM_DELTAQ_H
#define EAGL4ANIM_DELTAQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"

namespace EAGL4Anim {

inline void DeltaQRecoverW(int signBit, UMath::Vector4 &q);

// total size: 0x18
struct DeltaQMinRangef {
    UMath::Vector3 mMin;   // offset 0x0, size 0xC
    UMath::Vector3 mRange; // offset 0xC, size 0xC
};

// total size: 0xC
struct DeltaQMinRange {
    void UnQuantize(DeltaQMinRangef &minRangef) {
        const float RangeScale16Bit = 3.0518044e-5f;

        minRangef.mMin.x = mMin[0] * RangeScale16Bit - 1.0f;
        minRangef.mMin.y = mMin[1] * RangeScale16Bit - 1.0f;
        minRangef.mMin.z = mMin[2] * RangeScale16Bit - 1.0f;
        minRangef.mRange.x = mRange[0] * 9.6119827e-7f;
        minRangef.mRange.y = mRange[1] * 4.7871444e-7f;
        minRangef.mRange.z = mRange[2] * 4.7871444e-7f;
    }

    unsigned short mMin[3];   // offset 0x0, size 0x6
    unsigned short mRange[3]; // offset 0x6, size 0x6
};

// total size: 0x3
struct DeltaQDelta {
    void UnQuantize(const DeltaQMinRangef &minRangef, UMath::Vector4 &q) {
        q.x = minRangef.mMin.x + minRangef.mRange.x * mX;
        q.y = minRangef.mMin.y + minRangef.mRange.y * mY;
        q.z = minRangef.mMin.z + minRangef.mRange.z * mZ;
    }

    unsigned char mX : 7; // offset 0x0, size 0x1
    unsigned char mW : 1; // offset 0x0, size 0x1
    unsigned char mY;     // offset 0x1, size 0x1
    unsigned char mZ;     // offset 0x2, size 0x1
};

// total size: 0x6
struct DeltaQPhysical {
    void UnQuantize(UMath::Vector4 &q) {
        const float RangeScale16Bit = 3.0518044e-5f;
        const float RangeScale15Bit = 6.1037019e-5f;

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

inline void DeltaQRecoverW(int signBit, UMath::Vector4 &q) {
    float ndotn;

    {
        float len;
    }

    ndotn = q.x * q.x + q.y * q.y + q.z * q.z;
    if (ndotn <= 1.0f) {
        q.w = FastSqrt(1.0f - ndotn);
        if (signBit > 0) {
            q.w = -q.w;
        }
    } else {
        float len = FastSqrt(ndotn);

        q.x /= len;
        q.y /= len;
        q.z /= len;
        q.w = 0.0f;
    }
}

inline void FastPolarizedQuatBlend(float t, const UMath::Vector4 &q0, const UMath::Vector4 &q1, UMath::Vector4 &result) {
    float s;
    UMath::Vector4 temp;

    if (q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w > 0.0f) {
        temp = q1;
    } else {
        temp.x = -q1.x;
        temp.y = -q1.y;
        temp.z = -q1.z;
        temp.w = -q1.w;
    }

    result.x = t * (temp.x - q0.x) + q0.x;
    result.y = t * (temp.y - q0.y) + q0.y;
    result.z = t * (temp.z - q0.z) + q0.z;
    result.w = t * (temp.w - q0.w) + q0.w;
    s = 1.0f / FastSqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
    result.x *= s;
    result.y *= s;
    result.z *= s;
    result.w *= s;
}

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
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(this) + 0x12;

        minRanges = reinterpret_cast<DeltaQMinRange *>(memBytes);
        binStart = &memBytes[mNumBones * sizeof(DeltaQMinRange)];
        constBoneIndices = GetConstBoneIdx();
        constPhysical = GetConstPhysical();
    }

    int GetBinSize() const {
        return AlignSize2(mNumBones * (((GetBinLength() - 1) * sizeof(DeltaQDelta)) + sizeof(DeltaQPhysical)));
    }

    DeltaQMinRange *GetMinRange() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(this) + 0x12;
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
        return reinterpret_cast<DeltaQDelta *>(&binData[mNumBones * sizeof(DeltaQPhysical) + deltaIdx * mNumBones * sizeof(DeltaQDelta)]);
    }

    unsigned char *GetConstBoneIdx();

    DeltaQPhysical *GetConstPhysical();

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
