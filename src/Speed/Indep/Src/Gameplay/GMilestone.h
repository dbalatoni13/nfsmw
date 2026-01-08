#ifndef GAMEPLAY_GMILESTONE_H
#define GAMEPLAY_GMILESTONE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct MilestoneTypeInfo {
    unsigned int mTypeKey; // offset 0x0, size 0x4
    float mLastKnownValue; // offset 0x4, size 0x4
    float mBestValue;      // offset 0x8, size 0x4
    unsigned int mFlags;   // offset 0xC, size 0x4
};

// total size: 0x14
class GMilestone {
  private:
    unsigned int mTypeKey;      // offset 0x0, size 0x4
    unsigned int mChallengeKey; // offset 0x4, size 0x4
    unsigned char mState;       // offset 0x8, size 0x1
    unsigned char mFlags;       // offset 0x9, size 0x1
    unsigned short mBinNumber;  // offset 0xA, size 0x2
    float mRequiredValue;       // offset 0xC, size 0x4
    float mRecordedValue;       // offset 0x10, size 0x4
};

#endif
