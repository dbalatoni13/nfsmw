#ifndef EAGL4ANIM_POSEPALETTE_H
#define EAGL4ANIM_POSEPALETTE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimationTarget.h"
#include "FnAnim.h"
#include "Skeleton.h"

namespace EAGL4Anim {

// total size: 0xC
class PosePalette {
  public:
    int GetNumPoses() const {}

    int GetNumQs() const {}

    int GetNumTs() const {}

    void SetNumPoses(int n) {}

    void SetNumQs(int n) {}

    void SetNumTs(int n) {}

    float *GetPoseData() const {}

    unsigned short *GetDofIndices() const {}

  private:
    float *mPoseData;                // offset 0x0, size 0x4
    short unsigned int *mDofIndices; // offset 0x4, size 0x4
    unsigned char mNumPoses;         // offset 0x8, size 0x1
    unsigned char mNumQs;            // offset 0x9, size 0x1
    unsigned char mNumTs;            // offset 0xA, size 0x1
    unsigned char mPadding[1];       // offset 0xB, size 0x1
};

// total size: 0x8
class PosePaletteBank {
  public:
    bool IsSkelCompatible(const Skeleton *s) const {}

    unsigned short GetSkelCheckSum() const {}

    void SetSkelCheckSum(unsigned short cs) {}

    bool IsAnimCompatible(const FnAnim *a) const {}

    unsigned short GetCheckSum() const {}

    void SetCheckSum(unsigned short cs) {}

    unsigned short GetNumPalettes() const {}

    void SetNumPalettes(unsigned short np) {}

    int GetSize() const {}

    static int ComputeSize(int numPalettes) {}

    const PosePalette **GetPalettes() const {}

  private:
    CheckSum mSkelCheckSum;          // offset 0x0, size 0x2
    CheckSum mCheckSum;              // offset 0x2, size 0x2
    short unsigned int mNumPalettes; // offset 0x4, size 0x2
    unsigned char mPadding[2];       // offset 0x6, size 0x2
};

}; // namespace EAGL4Anim

#endif
