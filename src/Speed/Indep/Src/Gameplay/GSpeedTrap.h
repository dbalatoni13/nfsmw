#ifndef GAMEPLAY_GSPEEDTRAP_H
#define GAMEPLAY_GSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x14
class GSpeedTrap {
  public:
    enum Flags {
        kFlag_Unlocked = 1,
        kFlag_Active = 2,
        kFlag_Completed = 4,
        kFlag_KnockedOver = 8,
    };

    bool GetIsLocked() const {
        return IsFlagClear(kFlag_Unlocked);
    }

    bool GetIsUnlocked() const {
        return IsFlagSet(kFlag_Unlocked);
    }

    bool GetIsCompleted() const {
        return IsFlagSet(kFlag_Completed);
    }

    bool GetIsKnockedOver() const {
        return IsFlagSet(kFlag_KnockedOver);
    }

    bool GetIsActive() const {
        return IsFlagSet(kFlag_Active);
    }

    unsigned int GetSpeedTrapKey() const {
        return mSpeedTrapKey;
    }

    unsigned int GetCameraMarkerKey() const {
        return mCameraMarkerKey;
    }

    unsigned int GetBinNumber() const {
        return mBinNumber;
    }

    float GetTriggerSpeed() const {
        return mRequiredValue;
    }

    float GetRecordedPassSpeed() const {
        return mRecordedValue;
    }

    bool operator<(const GSpeedTrap &rhs) const {
        return mSpeedTrapKey < rhs.mSpeedTrapKey;
    }

    void SetFlag(unsigned int mask) {
        mFlags = static_cast<unsigned short>(mFlags | mask);
    }

    void ClearFlag(unsigned int mask) {
        mFlags = static_cast<unsigned short>(mFlags & ~mask);
    }

    bool IsFlagSet(unsigned int mask) const {
        return (mFlags & mask) != 0;
    }

    bool IsFlagClear(unsigned int mask) const {
        return (mFlags & mask) == 0;
    }

    GSpeedTrap();
    float GetBounty() const;
    class GTrigger *GetTrapTrigger() const;
    unsigned int GetJumpMarkerKey() const;
    void DebugForceComplete();
    void Init(unsigned int trapKey);
    void Reset();
    void Unlock();
    void Activate();
    void NotifyTriggered(float value);

  private:
    unsigned short mFlags;         // offset 0x0, size 0x2
    unsigned short mBinNumber;     // offset 0x2, size 0x2
    unsigned int mSpeedTrapKey;    // offset 0x4, size 0x4
    unsigned int mCameraMarkerKey; // offset 0x8, size 0x4
    float mRequiredValue;          // offset 0xC, size 0x4
    float mRecordedValue;          // offset 0x10, size 0x4
};

#endif
