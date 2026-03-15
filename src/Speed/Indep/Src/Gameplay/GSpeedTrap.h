#ifndef GAMEPLAY_GSPEEDTRAP_H
#define GAMEPLAY_GSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct GSpeedTrap {
    enum Flags { kFlag_Unlocked = 1, kFlag_Active = 2, kFlag_Completed = 4, kFlag_KnockedOver = 8, };
    unsigned short mFlags;
    unsigned short mBinNumber;
    unsigned int mSpeedTrapKey;
    unsigned int mCameraMarkerKey;
    float mRequiredValue;
    float mRecordedValue;
    void SetFlag(unsigned int mask) { mFlags |= mask; }
    void ClearFlag(unsigned int mask) { mFlags &= ~mask; }
    bool IsFlagSet(unsigned int mask) const;
    bool IsFlagClear(unsigned int mask) const;
    bool GetIsLocked() const { return IsFlagClear(kFlag_Unlocked); }
    bool GetIsUnlocked() const { return IsFlagSet(kFlag_Unlocked); }
    bool GetIsCompleted() const { return IsFlagSet(kFlag_Completed); }
    bool GetIsKnockedOver() const { return IsFlagSet(kFlag_KnockedOver); }
    bool GetIsActive() const { return IsFlagSet(kFlag_Active); }
    unsigned int GetSpeedTrapKey() const { return mSpeedTrapKey; }
    unsigned int GetBinNumber() const { return mBinNumber; }
    float GetTriggerSpeed() const { return mRequiredValue; }
    float GetRecordedPassSpeed() const { return mRecordedValue; }
    GSpeedTrap();
    float GetBounty() const;
    int GetLocalizationTag() const;
    unsigned int GetJumpMarkerKey() const;
    void Init(unsigned int trapKey);
    void Reset();
    void Unlock();
    void Activate();
    void NotifyTriggered(float value);
    void DebugForceComplete();
};

#endif
