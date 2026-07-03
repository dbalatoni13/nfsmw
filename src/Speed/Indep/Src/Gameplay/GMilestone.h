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
  public:
    bool GetIsLocked() const { return mState == 1; }
    bool GetIsAvailable() const { return mState == 2; }
    bool GetIsDonePendingEscape() const { return mState == 3; }
    bool GetIsAwarded() const { return mState == 4; }
    unsigned int GetTypeKey() const { return mTypeKey; }
    unsigned int GetChallengeKey() const { return mChallengeKey; }
    unsigned int GetBinNumber() const { return mBinNumber; }
    float GetRequiredValue() const { return mRequiredValue; }
    float GetRecordedPassValue() const { return mRecordedValue; }

    GMilestone();
    float GetCurrentValue() const;
    float GetBounty() const;
    int GetLocalizationTag() const;
    unsigned int GetJumpMarkerKey() const;
    void DebugForceComplete();
    void Init(unsigned int challengeKey);
    void Reset();
    void Unlock();
    void SetGoal(float required);
    bool ValueMeetsGoal(float value);
    void NotifyProgress(float value);
    void NotifyPursuitOver(bool escaped);

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
