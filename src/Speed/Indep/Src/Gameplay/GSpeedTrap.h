#ifndef GAMEPLAY_GSPEEDTRAP_H
#define GAMEPLAY_GSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

class GSpeedTrap {
  public:
    typedef enum { kFlag_Unlocked = 1, kFlag_Active = 2, kFlag_Completed = 4, kFlag_KnockedOver = 8 } Flags;

    GSpeedTrap();
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
    Attrib::Key GetSpeedTrapKey() const {
        return mSpeedTrapKey;
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
    float GetBounty() const;
    int GetLocalizationTag() const;
    // TODO impl GTrigger
    class GTrigger *GetTrapTrigger() const;
    Attrib::Key GetJumpMarkerKey() const;
    void NotifyTriggered(float value);
    void DebugForceComplete();
    bool operator<(const struct GSpeedTrap &rhs) const {}

  private:
    void SetFlag(unsigned int mask) {
        mFlags |= mask;
    }
    void ClearFlag(unsigned int mask) {
        mFlags &= ~mask;
    }
    bool IsFlagSet(unsigned int mask) const {
        return (mFlags & mask) != 0;
    };
    bool IsFlagClear(unsigned int mask) const {
        return (mFlags & mask) == 0;
    };

    void Init(Attrib::Key trapKey);
    void Reset();
    void Unlock();
    void Activate();

#ifdef EA_BUILD_A124
    Attrib::Key mSpeedTrapKey;
    uint16 mFlags;
    uint16 mBinNumber;
    Mps mRequiredValue;
    Mps mRecordedValue;
#else
    uint16 mFlags;
    uint16 mBinNumber;
    Attrib::Key mSpeedTrapKey;
    Attrib::Key mCameraMarkerKey;
    Mps mRequiredValue;
    Mps mRecordedValue;
#endif
};

#endif
