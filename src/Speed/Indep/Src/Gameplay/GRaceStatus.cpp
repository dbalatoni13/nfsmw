#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void GRaceParameters::EnsureLoaded() const {
    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }
}

void GRaceParameters::BlockUntilLoaded() {
    EnsureLoaded();
}

bool GRaceParameters::GetIsLoaded() const {
    if (mParentVault && !mParentVault->IsLoaded()) {
        return false;
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        return false;
    }

    return true;
}

void GRaceParameters::NotifyParentVaultUnloading() {
    mRaceRecord = nullptr;
}

void GRaceParameters::NotifyParentVaultLoaded() {
}

unsigned int GRaceParameters::GetCollectionKey() const {
    return mRaceRecord ? mRaceRecord->GetCollection() : 0;
}

float GRaceParameters::GetRaceLengthMeters() const {
    EnsureLoaded();
    return *reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x7C11C52E, 0) ?
                                               mRaceRecord->GetAttributePointer(0x7C11C52E, 0) :
                                               Attrib::DefaultDataArea(sizeof(float)));
}

int GRaceParameters::GetReputation() const {
    return 0;
}

float GRaceParameters::GetCashValue() const {
    return 0.0f;
}

int GRaceParameters::GetLocalizationTag() const {
    EnsureLoaded();
    return *reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0xDB89AB5C, 0) ?
                                              mRaceRecord->GetAttributePointer(0xDB89AB5C, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

int GRaceParameters::GetNumLaps() const {
    EnsureLoaded();
    return *reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0x0EBDC165, 0) ?
                                              mRaceRecord->GetAttributePointer(0x0EBDC165, 0) :
                                              Attrib::DefaultDataArea(sizeof(int)));
}

const char *GRaceParameters::GetEventID() const {
    EnsureLoaded();
    return *reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0xA78403EC, 0) ?
                                                               mRaceRecord->GetAttributePointer(0xA78403EC, 0) :
                                                               Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
}

float GRaceParameters::GetRivalBestTime() const {
    EnsureLoaded();
    return mRaceRecord->RivalBestTime(0);
}

float GRaceParameters::GetChallengeGoal() const {
    EnsureLoaded();
    return *reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x4E90219D, 0) ?
                                                mRaceRecord->GetAttributePointer(0x4E90219D, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

bool GRaceParameters::GetIsPursuitRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x2B1F54F6, 0) ?
                                               mRaceRecord->GetAttributePointer(0x2B1F54F6, 0) :
                                               Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetIsLoopingRace() const {
    EnsureLoaded();
    return mRaceRecord->IsLoopingRace(0);
}

bool GRaceParameters::GetRankPlayersByPoints() const {
    EnsureLoaded();
    return mRaceRecord->RankPlayersByPoints(0);
}

bool GRaceParameters::GetRankPlayersByDistance() const {
    EnsureLoaded();
    return mRaceRecord->RankPlayersByDistance(0);
}

bool GRaceParameters::GetScriptedCopsInRace() const {
    EnsureLoaded();
    return mRaceRecord->ScriptedCopsInRace(0);
}

bool GRaceParameters::GetNeverInQuickRace() const {
    EnsureLoaded();
    return mRaceRecord->NeverInQuickRace(0);
}

float GRaceParameters::GetTimeLimit() const {
    EnsureLoaded();
    return mRaceRecord->TimeLimit(0);
}

float GRaceParameters::GetMaxHeatLevel() const {
    EnsureLoaded();
    return mRaceRecord->MaxHeatLevel(0);
}

bool GRaceParameters::GetNoPostRaceScreen() const {
    EnsureLoaded();
    return mRaceRecord->NoPostRaceScreen(0);
}

float GRaceParameters::GetForceHeatLevel() const {
    EnsureLoaded();
    return static_cast<float>(mRaceRecord->ForceHeatLevel(0));
}

float GRaceParameters::GetInitialPlayerSpeed() const {
    EnsureLoaded();
    return mRaceRecord->InitialPlayerSpeed(0);
}

bool GRaceParameters::GetIsEpicPursuitRace() const {
    EnsureLoaded();
    return mRaceRecord->IsEpicPursuitRace(0);
}

const char *GRaceParameters::GetPlayerCarType() const {
    EnsureLoaded();
    return mRaceRecord->PlayerCarType(0);
}

float GRaceParameters::GetPlayerCarPerformance() const {
    EnsureLoaded();
    return mRaceRecord->PlayerCarPerformance(0);
}

int GRaceParameters::GetBustedLives() const {
    EnsureLoaded();
    return mRaceRecord->BustedLives(0);
}

int GRaceParameters::GetKnockoutsPerLap() const {
    EnsureLoaded();
    return mRaceRecord->KnockoutsPerLap(0);
}

bool GRaceParameters::GetCatchUp() const {
    EnsureLoaded();
    return mRaceRecord->CatchUp(0);
}

bool GRaceParameters::GetCatchUpOverride() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpOverride(0);
}

const char *GRaceParameters::GetCatchUpSkill() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpSkill(0);
}

const char *GRaceParameters::GetCatchUpSpread() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpSpread(0);
}

float GRaceParameters::GetCatchUpIntegral() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpIntegral(0);
}

float GRaceParameters::GetCatchUpDerivative() const {
    EnsureLoaded();
    return mRaceRecord->CatchUpDerivative(0);
}

bool GRaceParameters::GetPhotofinish() const {
    EnsureLoaded();
    return mRaceRecord->DoPhotofinish(0);
}

unsigned int GRaceParameters::GetNumCheckpoints() const {
    Attrib::Attribute checkpointList;

    EnsureLoaded();
    checkpointList = mRaceRecord->Get(0x34AAE3FC);
    return checkpointList.GetLength();
}

bool GRaceParameters::GetCheckpointsVisible() const {
    EnsureLoaded();
    return mRaceRecord->CheckpointsVisible(0);
}

unsigned int GRaceParameters::GetNumShortcuts() const {
    EnsureLoaded();
    return mRaceRecord->Num_Shortcuts();
}

unsigned int GRaceParameters::GetNumBarrierExemptions() const {
    EnsureLoaded();
    return mRaceRecord->Num_BarrierExemptions();
}

unsigned int GRaceParameters::GetBarrierCount() const {
    EnsureLoaded();
    return mRaceRecord->Num_Barriers();
}

const char *GRaceParameters::GetTrafficPattern() const {
    EnsureLoaded();
    return *reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x6319B692, 0) ?
                                                               mRaceRecord->GetAttributePointer(0x6319B692, 0) :
                                                               Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
}

unsigned int GRaceParameters::GetChallengeType() const {
    return 0;
}

GRace::Type GRaceParameters::GetRaceType() const {
    return GRace::kRaceType_None;
}

unsigned int GRaceParameters::GetEventHash() const {
    return Attrib::StringToKey(GetEventID());
}

int GRaceParameters::GetTrafficDensity() const {
    EnsureLoaded();
    return mRaceRecord->ForceTrafficDensity(0);
}

int GRaceParameters::GetNumOpponents() const {
    EnsureLoaded();
    return mRaceRecord->Num_Opponents();
}

float GRaceParameters::GetStartTime() const {
    EnsureLoaded();
    return mRaceRecord->StartTime(0);
}

float GRaceParameters::GetStartPercent() const {
    EnsureLoaded();
    return mRaceRecord->StartPercent(0);
}

const char *GRaceParameters::GetSpeedTrapCamera() const {
    EnsureLoaded();
    return mRaceRecord->SpeedTrapCamera(0);
}
