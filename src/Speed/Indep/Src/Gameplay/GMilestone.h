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
    void DebugForceComplete();

    enum State {
        kState_Invalid = 0,
        kState_Locked = 1,
        kState_Available = 2,
        kState_DonePendingEscape = 3,
        kState_Awarded = 4,
    };

    enum Flags {
        kFlag_BiggerIsBetter = 1,
        kFlag_CompletionFaked = 2,
    };

    GMilestone();

    void Init(unsigned int challengeKey);

    void Reset();

    void Unlock();

    // Declarada en el DWARF y estripada del enlace (rango 0xFFFFFFFF).
    void SetGoal(float required);

    bool ValueMeetsGoal(float value);

    void NotifyProgress(float value);

    void NotifyPursuitOver(bool escaped);

    float GetCurrentValue() const;

    float GetBounty() const;

    int GetLocalizationTag() const;

    unsigned int GetJumpMarkerKey() const;

    bool GetIsLocked() const {
        return mState == kState_Locked;
    }

    // "Disponible" incluye la que espera la huida: GManager::GetNextMilestone
    // expande ESTE inline en 0x801AFEF0 y alli el codigo es un test de rango,
    // `(mState - 2) <= 1`.
    bool GetIsAvailable() const {
        return mState >= kState_Available && mState <= kState_DonePendingEscape;
    }

    bool GetIsDonePendingEscape() const {
        return mState == kState_DonePendingEscape;
    }

    bool GetIsAwarded() const {
        return mState == kState_Awarded;
    }

    unsigned int GetTypeKey() const {
        return mTypeKey;
    }

    unsigned int GetChallengeKey() const {
        return mChallengeKey;
    }

    unsigned int GetBinNumber() const {
        return mBinNumber;
    }

    float GetRequiredValue() const {
        return mRequiredValue;
    }

    float GetRecordedPassValue() const {
        return mRecordedValue;
    }

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
