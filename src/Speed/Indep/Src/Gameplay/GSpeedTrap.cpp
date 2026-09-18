#include "GSpeedTrap.h"

#include "GManager.h"
#include "GTrigger.h"
#include "GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

GSpeedTrap::GSpeedTrap() {
    mFlags = 0;
    mBinNumber = 0;
    mSpeedTrapKey = 0;
    mCameraMarkerKey = 0;
    mRequiredValue = 0.0f;
    mRecordedValue = 0.0f;
}

float GSpeedTrap::GetBounty() const {
    Attrib::Gen::gameplay trap(mSpeedTrapKey, 0, NULL);
    if (trap.IsValid()) {
        return (float)trap.Bounty();
    }
    return 0.0f;
}

GTrigger *GSpeedTrap::GetTrapTrigger() const {
    GRuntimeInstance *instance = GManager::Get().FindInstance(mSpeedTrapKey);
    if (instance != NULL) {
        return static_cast<GTrigger *>(instance);
    }
    return NULL;
}

unsigned int GSpeedTrap::GetJumpMarkerKey() const {
    Attrib::Gen::gameplay trap(mSpeedTrapKey, 0, NULL);
    const GCollectionKey &marker = trap.SpawnPoint();
    return marker.GetCollectionKey();
}

void GSpeedTrap::Init(unsigned int trapKey) {
    mSpeedTrapKey = trapKey;
    Reset();
}

void GSpeedTrap::Reset() {
    Attrib::Gen::gameplay trap(mSpeedTrapKey, 0, NULL);

    mCameraMarkerKey = trap.CameraModelMarker().GetCollectionKey();
    mFlags = 0;
    mBinNumber = (unsigned short)trap.BinIndex();
    mRequiredValue = trap.ThreshholdSpeed() * 0.27778f;
    mRecordedValue = 0.0f;
}

void GSpeedTrap::Unlock() {
    SetFlag(kFlag_Unlocked);
}

void GSpeedTrap::Activate() {
    SetFlag(kFlag_Active);
}

inline bool GSpeedTrap::IsFlagClear(unsigned int mask) const {
    return (mFlags & mask) == 0;
}

void GSpeedTrap::NotifyTriggered(float value) {
    if (!IsFlagClear(kFlag_Unlocked) && !IsFlagClear(kFlag_Active)) {
        SetFlag(kFlag_Completed);
        ClearFlag(kFlag_Active);
        mRecordedValue = value;
        GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(mBinNumber);
        if (bin != NULL) {
            bin->RefreshProgress();
        }
        GManager::Get().RefreshSpeedTrapIcons();
    }
}

inline bool GSpeedTrap::IsFlagSet(unsigned int mask) const {
    return (mFlags & mask) != 0;
}
