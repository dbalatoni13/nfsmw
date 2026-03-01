#ifndef EAGL4ANIM_DELTAF1_H
#define EAGL4ANIM_DELTAF1_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "AnimUtil.h"

namespace EAGL4Anim {

// total size: 0x10
struct DeltaF1MinRange {
    float mPhysMin;    // offset 0x0, size 0x4
    float mPhysRange;  // offset 0x4, size 0x4
    float mDeltaMin;   // offset 0x8, size 0x4
    float mDeltaRange; // offset 0xC, size 0x4
};

// total size: 0x14
struct DeltaF1 : public AnimMemoryMap {
    // total size: 0xC
    struct DofInfo {
        float mPhysMin;             // offset 0x0, size 0x4
        float mPhysRange;           // offset 0x4, size 0x4
        unsigned short mQuantMin;   // offset 0x8, size 0x2
        unsigned short mQuantRange; // offset 0xA, size 0x2
    };

    static int ComputeSize(int numBones, int numConst, int numFrames, int binLen, bool useKeyFrames) {
        int size;
        int deltaSize;
        int binSize;
        int numBins;
        int leftOver;
    }

    unsigned short GetNumFrames() const {
        if (!mTimes) {
            return mNumFrames;
        } else {
            return mTimes[mNumFrames - 2] + 1;
        }
    }

    void SetNumFrames(unsigned short numFrames) {
        mNumFrames = numFrames;
    }

    unsigned short GetNumBones() const {
        return mNumBones;
    }

    void SetNumBones(unsigned short numBones) {
        mNumBones = numBones;
    }

    unsigned char GetNumConstBones() const {
        return mNumConstBones;
    }

    void SetNumConstBones(int b) {
        mNumConstBones = b;
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

    void SetBinLengthPower(int power) {
        mBinLengthPower = power;
    }

    int GetFrameDeltaSize() const {
        return mNumBones;
    }

    unsigned short *GetDofIndices() {
        return mDofIdxs;
    }

    int GetBinSize() const {
        return AlignSize2((mNumBones * 2) + ((GetBinLength() - 1) * GetFrameDeltaSize()));
    }

    void GetArrays(DofInfo *&dofInfo, unsigned char *&binStart) {}

    // int GetFrameStride() const {}

    DofInfo *GetDofInfo() {
        unsigned char *memBytes = reinterpret_cast<unsigned char *>(&this[1]);
        return reinterpret_cast<DofInfo *>(memBytes);
    }

    unsigned char *GetBin(int binIdx) {
        const int bs = GetBinSize();
        unsigned char *memPos = &reinterpret_cast<unsigned char *>(GetDofInfo())[mNumBones * sizeof(DofInfo)];
        return &memPos[binIdx * bs];
    }

    unsigned short *GetPhysical(unsigned char *binData) {
        return reinterpret_cast<unsigned short *>(binData);
    }

    unsigned char *GetDelta(unsigned char *binData, int frameIdx) {
        return &(&binData[mNumBones * 2])[frameIdx * GetFrameDeltaSize()];
    }

    float UnQuantizePhysical(const DeltaF1MinRange &dofInfo, unsigned short physFrame) const {
        return physFrame * dofInfo.mPhysRange + dofInfo.mPhysMin;
    }

    float UnQuantizeDelta(const DeltaF1MinRange &dofInfo, unsigned char deltaFrame) const {
        return deltaFrame * dofInfo.mDeltaRange + dofInfo.mDeltaMin;
    }

    unsigned short *GetConstBoneIdx() {
        const int binSize = GetBinSize();
        int numBins = mNumFrames >> GetBinLengthPower();
        // get to the end of the bins
        unsigned char *s = &GetBin(0)[binSize * numBins];
        int r = mNumFrames & GetBinLengthModMask();

        if (r > 0) {
            s = reinterpret_cast<unsigned char *>(AlignSize2((intptr_t)s + mNumBones * 2 + (r - 1) * GetFrameDeltaSize()));
        }
        if (mNumBones == 0) {
            s = reinterpret_cast<unsigned char *>(AlignSize2((intptr_t)s));
        }

        return reinterpret_cast<unsigned short *>(s);
    }

    float *GetConstPhysical() {
        return reinterpret_cast<float *>(AlignSize4(reinterpret_cast<intptr_t>(&GetConstBoneIdx()[mNumConstBones])));
    }

    unsigned short *mDofIdxs;      // offset 0x4, size 0x4
    unsigned short *mTimes;        // offset 0x8, size 0x4
    unsigned short mNumFrames;     // offset 0xC, size 0x2
    unsigned short mNumBones;      // offset 0xE, size 0x2
    unsigned char mBinLengthPower; // offset 0x10, size 0x1
    unsigned char mNumConstBones;  // offset 0x11, size 0x1
    unsigned char mPadding[2];     // offset 0x12, size 0x2
};

}; // namespace EAGL4Anim

#endif
