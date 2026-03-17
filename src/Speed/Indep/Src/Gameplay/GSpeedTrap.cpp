#include "Speed/Indep/Src/Gameplay/GSpeedTrap.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

GSpeedTrap::GSpeedTrap()
    : mFlags(0), //
      mBinNumber(0), //
      mSpeedTrapKey(0), //
      mCameraMarkerKey(0), //
      mRequiredValue(0.0f), //
      mRecordedValue(0.0f) {}

float GSpeedTrap::GetBounty() const {
    Attrib::Gen::gameplay gameplayObj(mSpeedTrapKey, 0, nullptr);

    if (!gameplayObj.IsValid()) {
        return 0.0f;
    }

    return static_cast<float>(gameplayObj.Bounty(0));
}

GTrigger *GSpeedTrap::GetTrapTrigger() const {
    return static_cast<GTrigger *>(GManager::Get().FindInstance(mSpeedTrapKey));
}

unsigned int GSpeedTrap::GetJumpMarkerKey() const {
    Attrib::Gen::gameplay gameplayObj(mSpeedTrapKey, 0, nullptr);

    return gameplayObj.SpawnPoint(0).GetCollectionKey();
}

void GSpeedTrap::DebugForceComplete() {
    SetFlag(kFlag_Unlocked | kFlag_Active | kFlag_Completed);
    mRecordedValue = mRequiredValue;
}

void GSpeedTrap::Init(unsigned int trapKey) {
    mSpeedTrapKey = trapKey;
    Reset();
}

void GSpeedTrap::Reset() {
    Attrib::Gen::gameplay gameplayObj(mSpeedTrapKey, 0, nullptr);

    mFlags = 0;
    mCameraMarkerKey = gameplayObj.CameraModelMarker(0).GetCollectionKey();
    mBinNumber = static_cast<unsigned short>(gameplayObj.BinIndex(0));
    mRequiredValue = gameplayObj.ThreshholdSpeed(0) * 0.277779996f;
    mRecordedValue = 0.0f;
}

void GSpeedTrap::Unlock() {
    mFlags = static_cast<unsigned short>(mFlags | kFlag_Unlocked);
}

void GSpeedTrap::Activate() {
    mFlags = static_cast<unsigned short>(mFlags | kFlag_Active);
}

void GSpeedTrap::NotifyTriggered(float value) {
    if (IsFlagSet(kFlag_Unlocked) && IsFlagSet(kFlag_Active)) {
        mFlags = static_cast<unsigned short>((mFlags & ~kFlag_Active) | kFlag_Completed);
        mRecordedValue = value;

        if (GRaceDatabase::Get().GetBinNumber(mBinNumber)) {
            GRaceDatabase::Get().GetBinNumber(mBinNumber)->RefreshProgress();
        }

        GManager::Get().RefreshSpeedTrapIcons();
    }
}
