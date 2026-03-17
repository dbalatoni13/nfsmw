#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

void SetCurrentTimeOfDay(float value);
extern int UnlockAllThings;

GRaceParameters::GRaceParameters(unsigned int collectionKey, GRaceIndexData *index)
    : mIndex(index), //
      mRaceRecord(new Attrib::Gen::gameplay(collectionKey, 0, nullptr)), //
      mParentVault(nullptr), //
      mChildVault(nullptr) {}

GRaceParameters::~GRaceParameters() {
    delete mRaceRecord;
    mRaceRecord = nullptr;
    mIndex = nullptr;
}

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

const Attrib::Gen::gameplay *GRaceParameters::GetGameplayObj() const {
    return mRaceRecord;
}

GVault *GRaceParameters::GetChildVault() const {
    return mChildVault;
}

GVault *GRaceParameters::GetParentVault() const {
    return mParentVault;
}

GActivity *GRaceParameters::GetActivity() const {
    return static_cast<GActivity *>(GManager::Get().FindInstance(GetCollectionKey()));
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

bool GRaceParameters::GetInitiallyUnlockedQuickRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xB39ED8C3, 0) ?
                                              mRaceRecord->GetAttributePointer(0xB39ED8C3, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetInitiallyUnlockedOnline() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x39509746, 0) ?
                                              mRaceRecord->GetAttributePointer(0x39509746, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetInitiallyUnlockedChallenge() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xEA855EAF, 0) ?
                                              mRaceRecord->GetAttributePointer(0xEA855EAF, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetIsDDayRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x8CB01ABF, 0) ?
                                              mRaceRecord->GetAttributePointer(0x8CB01ABF, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetIsBossRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xFF5EE5D6, 0) ?
                                              mRaceRecord->GetAttributePointer(0xFF5EE5D6, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetIsMarkerRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xF2FE50D7, 0) ?
                                              mRaceRecord->GetAttributePointer(0xF2FE50D7, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
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

bool GRaceParameters::GetCopsEnabled() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x3918E889, 0) ?
                                              mRaceRecord->GetAttributePointer(0x3918E889, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetNeverInQuickRace() const {
    EnsureLoaded();
    return mRaceRecord->NeverInQuickRace(0);
}

bool GRaceParameters::GetIsChallengeSeriesRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x1C650104, 0) ?
                                              mRaceRecord->GetAttributePointer(0x1C650104, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

bool GRaceParameters::GetIsCollectorsEditionRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x637584FE, 0) ?
                                              mRaceRecord->GetAttributePointer(0x637584FE, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
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

bool GRaceParameters::GetUseWorldHeatInRace() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x45F2AD6C, 0) ?
                                              mRaceRecord->GetAttributePointer(0x45F2AD6C, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
}

float GRaceParameters::GetForceHeatLevel() const {
    EnsureLoaded();
    return static_cast<float>(mRaceRecord->ForceHeatLevel(0));
}

float GRaceParameters::GetMaxRaceHeatLevel() const {
    EnsureLoaded();
    return *reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0xF5A03629, 0) ?
                                                mRaceRecord->GetAttributePointer(0xF5A03629, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

float GRaceParameters::GetInitialPlayerSpeed() const {
    EnsureLoaded();
    return mRaceRecord->InitialPlayerSpeed(0);
}

bool GRaceParameters::GetIsRollingStart() const {
    EnsureLoaded();
    return *reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xB809D19C, 0) ?
                                              mRaceRecord->GetAttributePointer(0xB809D19C, 0) :
                                              Attrib::DefaultDataArea(sizeof(bool)));
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

const char *GRaceParameters::GetPhotoFinishCamera() const {
    EnsureLoaded();
    return *reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x62DFC259, 0) ?
                                                               mRaceRecord->GetAttributePointer(0x62DFC259, 0) :
                                                               Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
}

const char *GRaceParameters::GetPhotoFinishTexture() const {
    EnsureLoaded();
    return *reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x038A3B53, 0) ?
                                                               mRaceRecord->GetAttributePointer(0x038A3B53, 0) :
                                                               Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
}

const char *GRaceParameters::GetTrafficPattern() const {
    EnsureLoaded();
    return *reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x6319B692, 0) ?
                                                               mRaceRecord->GetAttributePointer(0x6319B692, 0) :
                                                               Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
}

float GRaceParameters::GetTimeOfDay() const {
    EnsureLoaded();
    return *reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x9DFF3C3D, 0) ?
                                                mRaceRecord->GetAttributePointer(0x9DFF3C3D, 0) :
                                                Attrib::DefaultDataArea(sizeof(float)));
}

unsigned int GRaceParameters::GetChallengeType() const {
    return 0;
}

GRace::Type GRaceParameters::GetRaceType() const {
    return GRace::kRaceType_None;
}

unsigned int GRaceParameters::GetRegion() const {
    static const char *kRaceRegions[3] = {
        "city",
        "coastal",
        "college_town",
    };
    unsigned int i;
    const char *regionName;

    if (mIndex) {
        return reinterpret_cast<const unsigned char *>(mIndex)[0x29];
    }

    EnsureLoaded();
    regionName = mRaceRecord->Region(0);
    for (i = 0; i < 3; i++) {
        if (bStrCmp(regionName, kRaceRegions[i]) == 0) {
            return i;
        }
    }

    return static_cast<unsigned int>(-1);
}

void GRaceParameters::ExtractPosition(Attrib::Gen::gameplay &collection, UMath::Vector3 &pos) const {
    const UMath::Vector3 &collectionPosition = collection.Position(0);

    pos.x = -collectionPosition.y;
    pos.y = collectionPosition.z;
    pos.z = collectionPosition.x;
}

void GRaceParameters::ExtractDirection(Attrib::Gen::gameplay &collection, UMath::Vector3 &dir, float rotate) const {
    UMath::Matrix4 rotMat;
    UMath::Vector3 forward = {0.0f, 0.0f, 1.0f};

    UMath::MultYRot(UMath::Matrix4::kIdentity, -(collection.Rotation(0) + rotate) * 0.00069444446f, rotMat);
    VU0_MATRIX3x4_vect3mult(forward, rotMat, dir);
}

unsigned int GRaceParameters::GetEventHash() const {
    return Attrib::StringToKey(GetEventID());
}

bool GRaceParameters::GetIsAvailable(GRace::Context context) const {
    unsigned int eventHash;

    if (UnlockAllThings) {
        return true;
    }

    eventHash = GetEventHash();
    switch (context) {
    case GRace::kRaceContext_QuickRace:
        return !GetNeverInQuickRace() &&
               GRaceDatabase::Get().CheckRaceScoreFlags(eventHash, GRaceDatabase::kUnlocked_QuickRace);

    case GRace::kRaceContext_TimeTrial:
        return GRaceDatabase::Get().CheckRaceScoreFlags(eventHash, GRaceDatabase::kUnlocked_Online);

    case GRace::kRaceContext_Career:
        if (!GRaceDatabase::Get().CheckRaceScoreFlags(eventHash, GRaceDatabase::kUnlocked_Career)) {
            return false;
        }
        return !GRaceDatabase::Get().CheckRaceScoreFlags(eventHash, GRaceDatabase::kCompleted_ContextCareer);
    }

    return false;
}

int GRaceParameters::GetTrafficDensity() const {
    EnsureLoaded();
    return mRaceRecord->ForceTrafficDensity(0);
}

bool GRaceParameters::GetIsSunsetRace() const {
    return GetTimeOfDay() >= 0.8f;
}

bool GRaceParameters::GetIsMiddayRace() const {
    float timeOfDay;

    timeOfDay = GetTimeOfDay();
    return timeOfDay >= 0.0f && timeOfDay < 0.8f;
}

void GRaceParameters::SetupTimeOfDay() {
    if (GetIsSunsetRace()) {
        SetCurrentTimeOfDay(0.9f);
    } else if (GetIsMiddayRace()) {
        SetCurrentTimeOfDay(0.0f);
    }
}

GRace::Difficulty GRaceParameters::GetDifficulty() const {
    int trafficLevel;

    EnsureLoaded();
    trafficLevel = *reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0x88A7E3BE, 0) ?
                                                     mRaceRecord->GetAttributePointer(0x88A7E3BE, 0) :
                                                     Attrib::DefaultDataArea(sizeof(int)));
    if (trafficLevel < 0x22) {
        return GRace::kRaceDifficulty_Easy;
    }
    if (trafficLevel > 0x42) {
        return GRace::kRaceDifficulty_Hard;
    }
    return GRace::kRaceDifficulty_Medium;
}

int GRaceParameters::GetCopDensity() const {
    int density;

    EnsureLoaded();
    density = *reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0xDBC08D32, 0) ?
                                                 mRaceRecord->GetAttributePointer(0xDBC08D32, 0) :
                                                 Attrib::DefaultDataArea(sizeof(int)));
    if (density == 0 && !GetCopsEnabled()) {
        return 0;
    }
    if (density < 0x22) {
        return 1;
    }
    if (density > 0x42) {
        return 3;
    }
    return 2;
}

bool GRaceParameters::GetCanBeReversed() const {
    return false;
}

GCharacter *GRaceParameters::GetOpponentChar(unsigned int index) const {
    unsigned int characterKey;

    EnsureLoaded();
    if (GetIsBossRace() && GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career) {
        characterKey = Attrib::StringToLowerCaseKey("character_smart");
    } else {
        characterKey = mRaceRecord->Opponents(index).GetCollectionKey();
    }

    return static_cast<GCharacter *>(GManager::Get().FindInstance(characterKey));
}

int GRaceParameters::GetNumOpponents() const {
    EnsureLoaded();
    return mRaceRecord->Num_Opponents();
}

void GRaceParameters::GetStartPosition(UMath::Vector3 &pos) const {
    unsigned int collectionKey;

    EnsureLoaded();
    pos = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->racestart(0).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay raceStart(collectionKey, 0, nullptr);

        ExtractPosition(raceStart, pos);
    }
}

void GRaceParameters::GetStartDirection(UMath::Vector3 &dir) const {
    unsigned int collectionKey;

    EnsureLoaded();
    dir = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->racestart(0).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay raceStart(collectionKey, 0, nullptr);

        ExtractDirection(raceStart, dir, 0.0f);
    }
}

bool GRaceParameters::HasFinishLine() const {
    EnsureLoaded();
    return mRaceRecord->racefinish(0).GetCollectionKey() != 0;
}

void GRaceParameters::GetFinishPosition(UMath::Vector3 &pos) const {
    unsigned int collectionKey;

    EnsureLoaded();
    pos = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->racefinish(0).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay raceFinish(collectionKey, 0, nullptr);

        ExtractPosition(raceFinish, pos);
    }
}

void GRaceParameters::GetFinishDirection(UMath::Vector3 &dir) const {
    unsigned int collectionKey;

    EnsureLoaded();
    dir = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->racefinish(0).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay raceFinish(collectionKey, 0, nullptr);

        ExtractDirection(raceFinish, dir, 0.0f);
    }
}

void GRaceParameters::GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const {
    unsigned int collectionKey;

    EnsureLoaded();
    pos = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->Checkpoint(index).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay checkpoint(collectionKey, 0, nullptr);

        ExtractPosition(checkpoint, pos);
    }
}

void GRaceParameters::GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const {
    unsigned int collectionKey;

    EnsureLoaded();
    dir = UMath::Vector3::kZero;
    collectionKey = mRaceRecord->Checkpoint(index).GetCollectionKey();
    if (collectionKey) {
        Attrib::Gen::gameplay checkpoint(collectionKey, 0, nullptr);

        ExtractDirection(checkpoint, dir, 0.0f);
    }
}

GMarker *GRaceParameters::GetShortcut(unsigned int index) const {
    unsigned int collectionKey;

    EnsureLoaded();
    collectionKey = mRaceRecord->Shortcuts(index).GetCollectionKey();
    return static_cast<GMarker *>(GManager::Get().FindInstance(collectionKey));
}

GMarker *GRaceParameters::GetBarrierExemption(unsigned int index) const {
    unsigned int collectionKey;

    EnsureLoaded();
    collectionKey = mRaceRecord->BarrierExemptions(index).GetCollectionKey();
    return static_cast<GMarker *>(GManager::Get().FindInstance(collectionKey));
}

const char *GRaceParameters::GetBarrierName(unsigned int index) const {
    const char *barrierName;

    EnsureLoaded();
    barrierName = mRaceRecord->Barriers(index);
    if (barrierName && *barrierName == '*') {
        return barrierName + 1;
    }

    return barrierName;
}

bool GRaceParameters::GetBarrierIsFlipped(unsigned int index) const {
    const char *barrierName;

    EnsureLoaded();
    barrierName = mRaceRecord->Barriers(index);
    return barrierName && *barrierName == '*';
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
