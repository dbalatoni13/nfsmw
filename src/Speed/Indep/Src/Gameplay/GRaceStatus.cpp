#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

void SetCurrentTimeOfDay(float value);
extern int UnlockAllThings;

GRaceStatus *GRaceStatus::fObj = nullptr;

bool GRacerInfo::GetIsHuman() const {
    ISimable *simable = GetSimable();
    return simable && simable->IsPlayer();
}

void GRacerInfo::SetName(const char *name) {
    mName = name;
}

void GRacerInfo::SetIndex(int index) {
    mIndex = index;
}

void GRacerInfo::SetRanking(int ranking) {
    mRanking = ranking;
}

void GRacerInfo::SetSimable(ISimable *isim) {
    mhSimable = isim ? isim->GetInstanceHandle() : nullptr;
}

void GRacerInfo::KnockOut() {
    if (mFinishedRacing) {
        return;
    }

    mKnockedOut = true;
    mFinishedRacing = true;
    GRaceStatus::Get().CalculateRankings();
}

void GRacerInfo::TotalVehicle() {
    if (mFinishedRacing) {
        return;
    }

    mTotalled = true;
    mFinishedRacing = true;
    GRaceStatus::Get().CalculateRankings();
}

void GRacerInfo::Busted() {
    if (mFinishedRacing) {
        return;
    }

    mBusted = true;
    mFinishedRacing = true;
    GRaceStatus::Get().CalculateRankings();
}

void GRacerInfo::ChallengeComplete() {
    mChallengeComplete = true;
}

void GRacerInfo::ForceStop() {
    ISimable *simable = GetSimable();
    mRaceTimer.Stop();
    mLapTimer.Stop();
    mCheckTimer.Stop();
    mFinishedRacing = true;
    mFinishingSpeed = 0.0f;

    if (simable) {
        IVehicle *vehicle;

        if (simable->QueryInterface(&vehicle)) {
            vehicle->SetSpeed(0.0f);
        }
    }
}

void GRacerInfo::BlowEngine() {
    if (mFinishedRacing) {
        return;
    }

    mEngineBlown = true;
    mFinishedRacing = true;
    GRaceStatus::Get().CalculateRankings();
}

void GRacerInfo::AddToPointTotal(float points) {
    mPointTotal = UMath::Max(mPointTotal + points, 0.0f);
}

float GRacerInfo::CalcAverageSpeed() const {
    float time = mRaceTimer.GetTime();

    if (time <= 0.0f) {
        return 0.0f;
    }

    return mDistanceDriven / time;
}

float GRacerInfo::GetHudPctRaceComplete() const {
    float startPercent = 0.0f;

    if (GRaceStatus::Get().GetRaceParameters()) {
        startPercent = GRaceStatus::Get().GetRaceParameters()->GetStartPercent();
    }

    if (mFinishedRacing) {
        return 1.0f;
    }

    return bClamp(startPercent + ((1.0f - startPercent) * mPctRaceComplete), 0.0f, 1.0f);
}

void GRacerInfo::StartRace() {
    GRaceParameters *raceParameters = GRaceStatus::Get().GetRaceParameters();
    float startTime = raceParameters ? raceParameters->GetStartTime() : 0.0f;

    mFinishedRacing = false;
    mRaceTimer.Reset(startTime);
    mRaceTimer.Start();
    StartLap(1);
}

void GRacerInfo::StartLap(int lapIndexOneBased) {
    mLapsCompleted = lapIndexOneBased - 1;
    mCheckpointsHitThisLap = 0;
    mLapTimer.Reset(0.0f);
    mLapTimer.Start();
    StartCheckpoint(0);
}

void GRacerInfo::StartCheckpoint(int checkIndex) {
    if (checkIndex == mCheckpointsHitThisLap + 1) {
        mCheckpointsHitThisLap = checkIndex;
    }

    mCheckTimer.Reset(0.0f);
    mCheckTimer.Start();
    mDistToNextCheckpoint = 0.0f;
    mTimeCrossedLastCheck = mRaceTimer.GetTime();
}

void GRacerInfo::NotifySpeedTrapTriggered(float speed) {
    if (mSpeedTrapsCrossed < 16) {
        mSpeedTrapSpeed[mSpeedTrapsCrossed] = speed;
        mSpeedTrapPosition[mSpeedTrapsCrossed] = mRanking;
    }

    ++mSpeedTrapsCrossed;
}

void GRacerInfo::FinishRace() {
    ISimable *simable;
    UMath::Vector3 linearVelocity;

    mRaceTimer.Stop();
    mFinishedRacing = true;

    simable = GetSimable();
    if (simable) {
        simable->GetLinearVelocity(linearVelocity);
        mFinishingSpeed = UMath::Length(linearVelocity);
    }
}

bool GRacerInfo::IsBehind(const GRacerInfo &rhs) const {
    if (IsFinishedRacing() && rhs.IsFinishedRacing()) {
        return GetRaceTime() > rhs.GetRaceTime();
    }

    if (IsFinishedRacing() != rhs.IsFinishedRacing()) {
        return !IsFinishedRacing();
    }

    if (GetIsKnockedOut() != rhs.GetIsKnockedOut()) {
        return GetIsKnockedOut();
    }

    if (GetIsEngineBlown() != rhs.GetIsEngineBlown()) {
        return GetIsEngineBlown();
    }

    if (GetIsTotalled() != rhs.GetIsTotalled()) {
        return GetIsTotalled();
    }

#ifndef EA_BUILD_A124
    if (mDNF != rhs.mDNF) {
        return mDNF;
    }
#endif

    return GetPctRaceComplete() < rhs.GetPctRaceComplete();
}

bool GRacerInfo::AreStatsReady() const {
    if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_TimeTrial) {
        return true;
    }

    return mFinishedRacing || mEngineBlown || mTotalled;
}

void GRacerInfo::ClearAll() {
    mhSimable = nullptr;
    mGameCharacter = nullptr;
    mName = nullptr;
    mIndex = 0;
    mRanking = 0;
    mAiRanking = 0;
    mPctRaceComplete = 0.0f;
    mKnockedOut = false;
    mTotalled = false;
    mEngineBlown = false;
    mBusted = false;
    mChallengeComplete = false;
    mFinishedRacing = false;
    mCameraDetached = false;
    mPctLapComplete = 0.0f;
    mLapsCompleted = 0;
    mCheckpointsHitThisLap = 0;
    mTollboothsCrossed = 0;
    bMemSet(mTimeRemainingToBooth, 0, sizeof(mTimeRemainingToBooth));
    mSpeedTrapsCrossed = 0;
    bMemSet(mSpeedTrapSpeed, 0, sizeof(mSpeedTrapSpeed));
    bMemSet(mSpeedTrapPosition, 0, sizeof(mSpeedTrapPosition));
    mDistToNextCheckpoint = 0.0f;
    mDistanceDriven = 0.0f;
    mTopSpeed = 0.0f;
    mFinishingSpeed = 0.0f;
    mPoundsNOSUsed = 0.0f;
    mTimeCrossedLastCheck = 0.0f;
    mTotalUpdateTime = 0.0f;
    mNumPerfectShifts = 0;
    mNumTrafficCarsHit = 0;
    mSpeedBreakerTime = 0.0f;
    mPointTotal = 0.0f;
    mZeroToSixtyTime = 0.0f;
    mQuarterMileTime = 0.0f;
#ifndef EA_BUILD_A124
    bMemSet(mSplitTimes, 0, sizeof(mSplitTimes));
    bMemSet(mSplitRankings, 0, sizeof(mSplitRankings));
#endif
    mRaceTimer.Reset(0.0f);
    mLapTimer.Reset(0.0f);
    mCheckTimer.Reset(0.0f);
    mSavedPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    mSavedHeatLevel = 0.0f;
    mSavedDirection = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    mSavedSpeed = 0.0f;
#ifndef EA_BUILD_A124
    mDNF = false;
#endif
}

void GRacerInfo::SaveStartPosition() {
    ISimable *simable = GetSimable();

    if (!simable) {
        return;
    }

    IRigidBody *rigidBody = simable->GetRigidBody();
    if (rigidBody) {
        mSavedPosition = rigidBody->GetPosition();
        rigidBody->GetForwardVector(mSavedDirection);
        mSavedSpeed = rigidBody->GetSpeed();
    }

    mSavedHeatLevel = 0.0f;
    {
        IPerpetrator *perp;

        if (simable->QueryInterface(&perp)) {
            mSavedHeatLevel = perp->GetHeat();
        }
    }
}

void GRacerInfo::RestoreStartPosition() {
    ISimable *simable = GetSimable();
    IRacer *racer;

    if (!simable || !simable->QueryInterface(&racer)) {
        return;
    }

    RacePreparationInfo rpi;
    rpi.Position = mSavedPosition;
    rpi.Speed = mSavedSpeed;
    rpi.Direction = mSavedDirection;
    rpi.HeatLevel = mSavedHeatLevel;
    rpi.Flags = RacePreparationInfo::RESET_DAMAGE;
    racer->PrepareForRace(rpi);
}

void GRacerInfo::ForceStartPosition(const UMath::Vector3 &pos, const UMath::Vector3 &dir) {
    mSavedPosition = pos;
    mSavedDirection = dir;
    mSavedSpeed = 0.0f;
    RestoreStartPosition();
}

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

void GRaceParameters::GenerateIndex(GRaceIndexData *index) {
    unsigned int flags;
    char *indexBytes;
    float topLeft[2];
    float botRight[2];

    if (!index) {
        return;
    }

    indexBytes = reinterpret_cast<char *>(index);
    bMemSet(indexBytes, 0, 0x30);

    *reinterpret_cast<unsigned int *>(indexBytes + 0x00) = GetCollectionKey();
    *reinterpret_cast<unsigned int *>(indexBytes + 0x10) = GetChallengeType();
    *reinterpret_cast<unsigned int *>(indexBytes + 0x14) = GetEventHash();
    *reinterpret_cast<unsigned int *>(indexBytes + 0x18) = 0;
    *reinterpret_cast<float *>(indexBytes + 0x1C) = GetRaceLengthMeters();
    *reinterpret_cast<unsigned short *>(indexBytes + 0x20) = static_cast<unsigned short>(GetLocalizationTag());
    *reinterpret_cast<unsigned short *>(indexBytes + 0x22) = static_cast<unsigned short>(GetCashValue());
    *reinterpret_cast<short *>(indexBytes + 0x26) = static_cast<short>(GetRivalBestTime() * 8.0f);
    indexBytes[0x29] = static_cast<char>(GetRegion());
    indexBytes[0x2A] = static_cast<char>(GetCopDensity());
    indexBytes[0x2B] = static_cast<char>(GetRaceType());

    flags = 0;
    if (GetInitiallyUnlockedQuickRace()) {
        flags |= 1 << 0;
    }
    if (GetInitiallyUnlockedOnline()) {
        flags |= 1 << 1;
    }
    if (GetInitiallyUnlockedChallenge()) {
        flags |= 1 << 2;
    }
    if (GetCanBeReversed()) {
        flags |= 1 << 3;
    }
    if (GetIsDDayRace()) {
        flags |= 1 << 4;
    }
    if (GetIsBossRace()) {
        flags |= 1 << 5;
    }
    if (GetIsMarkerRace()) {
        flags |= 1 << 6;
    }
    if (GetIsPursuitRace()) {
        flags |= 1 << 7;
    }
    if (GetIsLoopingRace()) {
        flags |= 1 << 8;
    }
    if (GetRankPlayersByPoints()) {
        flags |= 1 << 9;
    }
    if (GetRankPlayersByDistance()) {
        flags |= 1 << 10;
    }
    if (GetCopsEnabled()) {
        flags |= 1 << 11;
    }
    if (GetScriptedCopsInRace()) {
        flags |= 1 << 12;
    }
    if (GetTimeOfDay() > 0.8f) {
        flags |= 1 << 13;
    }
    if (GetNeverInQuickRace()) {
        flags |= 1 << 14;
    }
    if (GetIsChallengeSeriesRace()) {
        flags |= 1 << 15;
    }
    if (GetIsCollectorsEditionRace()) {
        flags |= 1 << 16;
    }
    if (GetTimeOfDay() <= 0.8f && GetTimeOfDay() >= 0.0f) {
        flags |= 1 << 17;
    }

    *reinterpret_cast<unsigned short *>(indexBytes + 0x04) = static_cast<unsigned short>(flags);
    bSafeStrCpy(indexBytes + 0x06, GetEventID(), 10);

    GetBoundingBox(*reinterpret_cast<UMath::Vector2 *>(topLeft), *reinterpret_cast<UMath::Vector2 *>(botRight));
    indexBytes[0x2C] = static_cast<char>(topLeft[0] * 255.0f);
    indexBytes[0x2D] = static_cast<char>(topLeft[1] * 255.0f);
    indexBytes[0x2E] = static_cast<char>(botRight[0] * 255.0f);
    indexBytes[0x2F] = static_cast<char>(botRight[1] * 255.0f);
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

void GRaceParameters::GetBoundingBox(UMath::Vector2 &topLeft, UMath::Vector2 &botRight) const {
    float *topLeftValues;
    float *botRightValues;

    topLeftValues = reinterpret_cast<float *>(&topLeft);
    botRightValues = reinterpret_cast<float *>(&botRight);
    if (mIndex) {
        const unsigned char *indexBytes;

        indexBytes = reinterpret_cast<const unsigned char *>(mIndex);
        topLeftValues[0] = indexBytes[0x2C] * (1.0f / 255.0f);
        topLeftValues[1] = indexBytes[0x2D] * (1.0f / 255.0f);
        botRightValues[0] = indexBytes[0x2E] * (1.0f / 255.0f);
        botRightValues[1] = indexBytes[0x2F] * (1.0f / 255.0f);
        return;
    }

    topLeftValues[0] = 0.0f;
    topLeftValues[1] = 0.0f;
    botRightValues[0] = 0.0f;
    botRightValues[1] = 0.0f;
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

GRaceCustom::GRaceCustom(const GRaceParameters &other)
    : GRaceParameters(other.GetCollectionKey(), nullptr), //
      mRaceActivity(nullptr), //
      mNumOpponents(0), //
      mReversed(false), //
      mFreedByOwner(false), //
      mHeatLevel(-1) {
    Attrib::Attribute opponents;
    unsigned int customKey;

    customKey = mRaceRecord->GenerateUniqueKey("GRaceCustom", true);
    mRaceRecord->Modify(customKey, 0);
    mRaceRecord->SetParent(other.GetGameplayObj()->GetCollection());

    opponents = mRaceRecord->Get(0x5839FA1A);
    mNumOpponents = opponents.GetLength();
}

GRaceCustom::~GRaceCustom() {
    if (mRaceActivity) {
        delete mRaceActivity;
        mRaceActivity = nullptr;
    }
}

void GRaceCustom::CreateRaceActivity() {
    if (mReversed && !GetCanBeReversed()) {
        mReversed = false;
    }

    if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career && GetIsBossRace()) {
        mNumOpponents = 0;
    } else {
        mNumOpponents = GetNumOpponents();
    }

    if (mHeatLevel != -1) {
        if (mHeatLevel == 0) {
            SetCopsEnabled(false);
        } else {
            SetForceHeatLevel(mHeatLevel);
        }
    }

    mRaceActivity = new GActivity(mRaceRecord->GetCollection());
    mRaceActivity->AllocateConnectionBuffer(GObjectBlock::CalcNumConnections(mRaceRecord->GetCollection()));
}

GActivity *GRaceCustom::GetRaceActivity() const {
    return mRaceActivity;
}

void GRaceCustom::GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const {
    if (mReversed) {
        index = GetNumCheckpoints() - (index + 1);
    }

    GRaceParameters::GetCheckpointPosition(index, pos);
}

void GRaceCustom::GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const {
    float rotate;

    if (mReversed) {
        index = GetNumCheckpoints() - (index + 1);
        rotate = 180.0f;
    } else {
        rotate = 0.0f;
    }

    EnsureLoaded();
    dir = UMath::Vector3::kZero;
    {
        unsigned int collectionKey;

        collectionKey = mRaceRecord->Checkpoint(index).GetCollectionKey();
        if (collectionKey) {
            Attrib::Gen::gameplay checkpoint(collectionKey, 0, nullptr);

            ExtractDirection(checkpoint, dir, rotate);
        }
    }
}

void GRaceCustom::SetReversed(bool isReverseDir) {
    mReversed = isReverseDir;
}

void GRaceCustom::SetNumLaps(int numLaps) {
    SetAttribute(0x0EBDC165, numLaps, 0);
}

void GRaceCustom::SetTrafficDensity(int density) {
    if (density < 0) {
        density = 0;
    } else if (density > 100) {
        density = 100;
    }

    SetAttribute(0xC64BC341, density, 0);
}

void GRaceCustom::SetNumOpponents(int numOpponents) {
    mNumOpponents = numOpponents;
}

void GRaceCustom::SetDifficulty(GRace::Difficulty difficulty) {
    int value;

    if (difficulty == GRace::kRaceDifficulty_Easy) {
        value = 0x21;
    } else if (difficulty == GRace::kRaceDifficulty_Medium) {
        value = 0x42;
    } else if (difficulty == GRace::kRaceDifficulty_Hard) {
        value = 100;
    } else {
        value = 0;
    }

    SetAttribute(0x88A7E3BE, value, 0);
}

void GRaceCustom::SetCatchUp(bool catchUpEnabled) {
    SetAttribute(0x10DB04E6, catchUpEnabled, 0);
}

void GRaceCustom::SetCopsEnabled(bool copsEnabled) {
    SetAttribute(0x3918E889, copsEnabled, 0);
}

void GRaceCustom::SetForceHeatLevel(int level) {
    SetAttribute(0xE4211F4F, level, 0);
}

template <typename T> void GRaceCustom::SetAttribute(unsigned int key, const T &value, unsigned int index) {
    Attrib::Attribute attribute;
    T *dest;

    attribute = mRaceRecord->Get(key);
    if (!attribute.IsValid()) {
        mRaceRecord->Add(key, 1);
    }

    dest = reinterpret_cast<T *>(const_cast<void *>(mRaceRecord->GetAttributePointer(key, index)));
    if (dest) {
        *dest = value;
    }
}

GRaceStatus::GRaceStatus()
    : UTL::COM::Object(1), //
      IVehicleCache((UTL::COM::Object *)this), //
      mRacerCount(0), //
      mIsLoading(false), //
      mPlayMode(kPlayMode_Racing), //
      mRaceContext(GRace::kRaceContext_Career), //
      mRaceParms(nullptr), //
      mRaceBin(nullptr), //
      mBonusTime(0.0f), //
      mTaskTime(0.0f), //
      mSuddenDeathMode(false), //
      mTimeExpiredMsgSent(false), //
      mActivelyRacing(false), //
      mLastSecondTickSent(0), //
      mCheckpointModel(nullptr), //
      mCheckpointEmitter(nullptr), //
      mQueueBinChange(false), //
      mNumTollbooths(0), //
      mScriptWaitingForLoad(false), //
      mCheckpoints(), //
      mNextCheckpoint(nullptr), //
      fRaceLength(0.0f), //
      fFirstLapLength(0.0f), //
      fSubsequentLapLength(0.0f), //
      fCatchUpIntegral(0.0f), //
      fCatchUpDerivative(0.0f), //
      fCatchUpAdaptiveBonus(0.0f), //
      fAveragePercentComplete(0.0f), //
      nCatchUpSkillEntries(0), //
      nCatchUpSpreadEntries(0), //
      nSpeedTraps(0), //
      mVehicleCacheLocked(false), //
      bRaceRouteError(false), //
      mTrafficDensity(0), //
      mTrafficPattern(0), //
      mHasBeenWon(false) {
    fObj = this;

    bMemSet(mSegmentLengths, 0, sizeof(mSegmentLengths));
    bMemSet(mLapTimes, 0, sizeof(mLapTimes));
    bMemSet(mCheckTimes, 0, sizeof(mCheckTimes));
    bMemSet(aCatchUpSkillData, 0, sizeof(aCatchUpSkillData));
    bMemSet(aCatchUpSpreadData, 0, sizeof(aCatchUpSpreadData));
    bMemSet(aSpeedTraps, 0, sizeof(aSpeedTraps));

    for (int i = 0; i < 16; ++i) {
        mRacerInfo[i].ClearAll();
    }

    ClearTimes();
    SyncronizeAdaptiveBonus();
    MakeDefaultCatchUpData();

    if (!GRaceDatabase::Get().GetStartupRace()) {
        EnterBin(0);
        SetRoaming();
    }
}

void GRaceStatus::Init() {
    if (!fObj) {
        new GRaceStatus();
    }
}

void GRaceStatus::OnRemovedVehicleCache(IVehicle *ivehicle) {}

const char *GRaceStatus::GetCacheName() const {
    return "GRaceStatus";
}

void GRaceStatus::SetRaceContext(GRace::Context context) {
    mRaceContext = context;
}

int GRaceStatus::GetRacerCount() const {
    return mRacerCount;
}

void GRaceStatus::SetActivelyRacing(bool racing) {
    mActivelyRacing = racing;
}

void GRaceStatus::SetHasBeenWon(bool won) {
    mHasBeenWon = won;
}

void GRaceStatus::SetIsLoading(bool loading) {
    mIsLoading = loading;
}

void GRaceStatus::SetTaskTime(float time) {
    mTaskTime = time;
}

void GRaceStatus::Shutdown() {
    if (fObj) {
        delete fObj;
        fObj = nullptr;
    }
}

void GRaceStatus::EnableBinBarriers() {
    EnableBarriers();
}

void GRaceStatus::DisableBinBarriers() {
    DisableBarriers();
}

void GRaceStatus::RefreshBinWhileInGame() {
    mQueueBinChange = true;
}

GRacerInfo &GRaceStatus::GetRacerInfo(int index) {
    return mRacerInfo[index];
}

GRacerInfo *GRaceStatus::GetRacerInfo(ISimable *isim) {
    for (int i = 0; i < mRacerCount; ++i) {
        if (mRacerInfo[i].GetSimable() == isim) {
            return &mRacerInfo[i];
        }
    }

    return nullptr;
}

GRacerInfo *GRaceStatus::GetWinningPlayerInfo() {
    GRacerInfo *winner = nullptr;

    for (int i = 0; i < mRacerCount; ++i) {
        GRacerInfo &info = mRacerInfo[i];

        if (!info.GetIsHuman()) {
            continue;
        }

        if (!winner || info.GetRanking() < winner->GetRanking()) {
            winner = &info;
        }
    }

    return winner;
}

void GRaceStatus::StartMasterTimer() {
    float startTime = 0.0f;

    if (mRaceParms) {
        startTime = mRaceParms->GetStartTime();
    }

    mRaceMasterTimer.Reset(-startTime);
    mRaceMasterTimer.Start();
    mLastSecondTickSent = static_cast<int>(GetRaceTimeElapsed());
    mTimeExpiredMsgSent = false;
}

void GRaceStatus::StopMasterTimer() {
    mRaceMasterTimer.Stop();
}

float GRaceStatus::GetRaceTimeElapsed() const {
    return mRaceMasterTimer.GetTime();
}

float GRaceStatus::GetRaceTimeRemaining() const {
    float limit = mRaceParms ? mRaceParms->GetTimeLimit() : 0.0f;

    if (limit <= 0.0f) {
        return 0.0f;
    }

    return limit + mBonusTime - GetRaceTimeElapsed();
}

void GRaceStatus::ClearRacers() {
    for (int i = 0; i < mRacerCount; ++i) {
        mRacerInfo[i].ClearAll();
    }

    mRacerCount = 0;
}

GRacerInfo &GRaceStatus::AddSimablePlayer(ISimable *isim) {
    int index = mRacerCount;
    GRacerInfo &info = mRacerInfo[index];

    info.ClearAll();
    info.SetSimable(isim);
    info.SetIndex(index);
    info.SetRanking(index);
    ++mRacerCount;
    return info;
}

void GRaceStatus::AddRacer(GRuntimeInstance *racer) {
    GCharacter *character = static_cast<GCharacter *>(racer);
    IVehicle *vehicle = character ? character->GetSpawnedVehicle() : nullptr;
    ISimable *isimable = nullptr;
    int index = mRacerCount;
    GRacerInfo &info = mRacerInfo[index];

    info.ClearAll();
    info.SetIndex(index);
    info.SetRanking(index);
    if (vehicle && vehicle->QueryInterface(&isimable)) {
        info.SetSimable(isimable);
    }

    ++mRacerCount;
}

void GRaceStatus::SetRaceActivity(GActivity *activity) {
    mRaceParms = activity ? GRaceDatabase::Get().GetRaceFromActivity(activity) : nullptr;
    DetermineRaceLength();
}

void GRaceStatus::EnableBarriers() {
    if (mRaceBin) {
        mRaceBin->EnableBarriers();
    }
}

void GRaceStatus::DisableBarriers() {
    if (mRaceBin) {
        mRaceBin->DisableBarriers();
    }
}

void GRaceStatus::AddAvailableEventToMap(GRuntimeInstance *trigger, GRuntimeInstance *activity) {}

void GRaceStatus::AddSpeedTrapToMap(GRuntimeInstance *trigger) {}

void GRaceStatus::AwardBonusTime(float seconds) {
    mBonusTime += seconds;
}

void GRaceStatus::ClearCheckpoints() {
    mCheckpoints.clear();
    mNextCheckpoint = nullptr;
}

void GRaceStatus::AddCheckpoint(GRuntimeInstance *trigger) {
    GTrigger *gtrigger = static_cast<GTrigger *>(trigger);

    if (!gtrigger) {
        return;
    }

    mCheckpoints.push_back(gtrigger);
    if (!mNextCheckpoint) {
        mNextCheckpoint = gtrigger;
    }
}

float GRaceStatus::GetAdaptiveDifficutly() const {
    return fCatchUpAdaptiveBonus;
}

void GRaceStatus::SyncronizeAdaptiveBonus() {
    if (fCatchUpAdaptiveBonus < 0.0f) {
        fCatchUpAdaptiveBonus = 0.0f;
    }
}

void GRaceStatus::MakeDefaultCatchUpData() {
    nCatchUpSkillEntries = 0;
    nCatchUpSpreadEntries = 0;
    fCatchUpIntegral = 0.0f;
    fCatchUpDerivative = 0.0f;
    bMemSet(aCatchUpSkillData, 0, sizeof(aCatchUpSkillData));
    bMemSet(aCatchUpSpreadData, 0, sizeof(aCatchUpSpreadData));
}

void GRaceStatus::MakeCatchUpData() {
    MakeDefaultCatchUpData();
    if (mRaceParms) {
        ParseCatchUpData(mRaceParms->GetCatchUpSkill(), mRaceParms->GetCatchUpSpread());
        fCatchUpIntegral = mRaceParms->GetCatchUpIntegral();
        fCatchUpDerivative = mRaceParms->GetCatchUpDerivative();
    }
}

void GRaceStatus::ParseCatchUpData(const char *skill, const char *spread) {}

int GRaceStatus::GetCheckpointCount() {
    return mCheckpoints.size();
}

GTrigger *GRaceStatus::GetCheckpoint(int index) {
    return mCheckpoints[index];
}

GTrigger *GRaceStatus::GetNextCheckpoint() {
    return mNextCheckpoint;
}

void GRaceStatus::ClearTimes() {
    bMemSet(mLapTimes, 0, sizeof(mLapTimes));
    bMemSet(mCheckTimes, 0, sizeof(mCheckTimes));
}

void GRaceStatus::SetLapTime(int lapIndex, int racerIndex, float time) {
    mLapTimes[lapIndex][racerIndex] = time;
}

float GRaceStatus::GetLapTime(int lapIndex, int racerIndex, bool bCumulativeTimeAtLap) {
    float time = mLapTimes[lapIndex][racerIndex];

    if (bCumulativeTimeAtLap) {
        for (int i = 0; i < lapIndex; ++i) {
            time += mLapTimes[i][racerIndex];
        }
    }

    return time;
}

void GRaceStatus::SetCheckpointTime(int lapIndex, int checkIndex, int racerIndex, float time) {
    mCheckTimes[lapIndex][checkIndex][racerIndex] = time;
}

float GRaceStatus::GetCheckpointTime(int lapIndex, int checkIndex, int racerIndex, bool bCumulative) {
    float time = mCheckTimes[lapIndex][checkIndex][racerIndex];

    if (bCumulative) {
        for (int i = 0; i < checkIndex; ++i) {
            time += mCheckTimes[lapIndex][i][racerIndex];
        }
    }

    return time;
}

int GRaceStatus::GetLapPosition(int lapIndex, int racerIndex, bool bOverallPosition) {
    float racerTime = GetLapTime(lapIndex, racerIndex, bOverallPosition);
    int position = 1;

    for (int i = 0; i < mRacerCount; ++i) {
        if (i == racerIndex) {
            continue;
        }

        float otherTime = GetLapTime(lapIndex, i, bOverallPosition);
        if (otherTime > 0.0f && (racerTime <= 0.0f || otherTime < racerTime)) {
            ++position;
        }
    }

    return position;
}

float GRaceStatus::GetBestLapTime(int racerIndex) {
    float best = 0.0f;
    int lapCount = mRaceParms ? mRaceParms->GetNumLaps() : 10;

    for (int i = 0; i < lapCount && i < 10; ++i) {
        float time = GetLapTime(i, racerIndex, false);
        if (time > 0.0f && (best <= 0.0f || time < best)) {
            best = time;
        }
    }

    return best;
}

float GRaceStatus::GetWorstLapTime(int racerIndex) {
    float worst = 0.0f;
    int lapCount = mRaceParms ? mRaceParms->GetNumLaps() : 10;

    for (int i = 0; i < lapCount && i < 10; ++i) {
        float time = GetLapTime(i, racerIndex, false);
        if (time > worst) {
            worst = time;
        }
    }

    return worst;
}

float GRaceStatus::GetFinishTimeBehind(int racerIndex) {
    GRacerInfo &info = mRacerInfo[racerIndex];
    float finishTime = info.GetRaceTime();
    float bestFinish = finishTime;

    for (int i = 0; i < mRacerCount; ++i) {
        if (i == racerIndex) {
            continue;
        }

        GRacerInfo &opponent = mRacerInfo[i];
        if (opponent.GetRaceTime() < bestFinish) {
            bestFinish = opponent.GetRaceTime();
        }
    }

    return finishTime - bestFinish;
}

int GRaceStatus::GetLapsLed(int racerIndex) {
    int numLed = 0;

    for (int i = 0; i < 10; ++i) {
        if (GetLapPosition(i, racerIndex, true) == 1) {
            ++numLed;
        }
    }

    return numLed;
}

float GRaceStatus::GetRaceSpeedTrapSpeed(int trapIndex, int racerIndex) {
    return mRacerInfo[racerIndex].GetSpeedTrapSpeed(trapIndex);
}

int GRaceStatus::GetRaceSpeedTrapPosition(int trapIndex, int racerIndex) {
    return mRacerInfo[racerIndex].GetSpeedTrapPosition(trapIndex);
}

float GRaceStatus::GetBestSpeedTrapSpeed(int racerIndex) {
    float best = 0.0f;

    for (int i = 0; i < 16; ++i) {
        best = UMath::Max(best, GetRaceSpeedTrapSpeed(i, racerIndex));
    }

    return best;
}

float GRaceStatus::GetWorstSpeedTrapSpeed(int racerIndex) {
    float worst = 0.0f;
    bool found = false;

    for (int i = 0; i < 16; ++i) {
        float speed = GetRaceSpeedTrapSpeed(i, racerIndex);
        if (speed > 0.0f && (!found || speed < worst)) {
            worst = speed;
            found = true;
        }
    }

    return worst;
}

float GRaceStatus::GetRaceTollboothTime(int boothIndex, int racerIndex) {
    return mRacerInfo[racerIndex].GetTollboothTime(boothIndex);
}

void GRaceStatus::RaceAbandoned() {
    if (GetRaceContext() != GRace::kRaceContext_Career) {
        return;
    }

    GManager::Get().SuspendAllActivities();
}

void GRaceStatus::EndStopAll() {
    for (int i = 0; i < mRacerCount; ++i) {
        mRacerInfo[i].ForceStop();
    }
}

void GRaceStatus::FinalizeRaceStats() {
    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        for (int i = 0; i < mRacerCount; ++i) {
            mRacerInfo[i].FinalizeRaceStats();
        }
    }
}

bool GRaceStatus::IsAudioLoading() {
    return false;
}

bool GRaceStatus::IsModelsLoading() {
    return false;
}

bool GRaceStatus::IsLoading() {
    return mIsLoading || IsAudioLoading() || IsModelsLoading();
}

float GRaceStatus::GetSegmentLength(int segment, int lap) {
    if (segment < 0 || segment >= 18) {
        return 0.0f;
    }

    return mSegmentLengths[segment];
}

void GRaceStatus::EnterSuddenDeath() {
    mSuddenDeathMode = true;
}

float GRaceStatus::DetermineRaceSegmentLength(const UMath::Vector4 *positions, const UMath::Vector4 *directions, int start, int end) {
    WRoadNav nav;
    bVector3 delta;
    float pathDistance;

    nav.SetPathType(WRoadNav::kPathChopper);

    bSub(&delta, reinterpret_cast<const bVector3 *>(&positions[start]), reinterpret_cast<const bVector3 *>(&positions[end]));
    pathDistance = bLength(delta);

    nav.InitAtPoint(UMath::Vector4To3(positions[start]), UMath::Vector4To3(directions[start]), true, 1.0f);
    if (!nav.IsValid()) {
        bRaceRouteError = true;
        return pathDistance;
    }

    if (nav.FindPathNow(&UMath::Vector4To3(positions[end]), &UMath::Vector4To3(directions[end]), nullptr)) {
        pathDistance = nav.GetPathDistanceRemaining();
    } else {
        bRaceRouteError = true;
    }

    return pathDistance;
}

void GRaceStatus::DetermineRaceLength() {
    UMath::Vector4 positions[18];
    UMath::Vector4 directions[18];
    UMath::Vector3 pos;
    UMath::Vector3 dir;
    int numCheckpoints;
    int numSegments;

    nSpeedTraps = 0;
    fSubsequentLapLength = 0.0f;
    fRaceLength = 0.0f;
    fFirstLapLength = 0.0f;
    bMemSet(mSegmentLengths, 0, sizeof(mSegmentLengths));
    bRaceRouteError = false;
    WRoadNetwork::Get().ResolveShortcuts();

    if (!mRaceParms || !mRaceParms->HasFinishLine()) {
        return;
    }

    WRoadNetwork::Get().SetRaceFilterValid(true);
    numCheckpoints = mRaceParms->GetNumCheckpoints();
    mRaceParms->GetStartPosition(pos);
    positions[0] = UMath::Vector4Make(pos, 0.0f);
    mRaceParms->GetStartDirection(dir);
    directions[0] = UMath::Vector4Make(dir, 0.0f);

    for (int i = 0; i < numCheckpoints; ++i) {
        mRaceParms->GetCheckpointPosition(i, pos);
        positions[i + 1] = UMath::Vector4Make(pos, 0.0f);
        mRaceParms->GetCheckpointDirection(i, dir);
        directions[i + 1] = UMath::Vector4Make(dir, 0.0f);
    }

    mRaceParms->GetFinishPosition(pos);
    positions[numCheckpoints + 1] = UMath::Vector4Make(pos, 0.0f);
    mRaceParms->GetFinishDirection(dir);
    directions[numCheckpoints + 1] = UMath::Vector4Make(dir, 0.0f);

    numSegments = numCheckpoints + 1;
    if (mRaceParms->GetIsLoopingRace()) {
        numSegments = numCheckpoints + 2;
    }

    for (int i = 0; i < numSegments; ++i) {
        int end = (i % (numCheckpoints + 1)) + 1;
        float segmentLength = DetermineRaceSegmentLength(positions, directions, i, end);

        mSegmentLengths[i] = segmentLength;
        fRaceLength += segmentLength;
    }

    if (mRaceParms->GetIsLoopingRace()) {
        fSubsequentLapLength = fRaceLength - mSegmentLengths[0];
        fFirstLapLength = fRaceLength - mSegmentLengths[numCheckpoints + 1];
        fRaceLength = fSubsequentLapLength * static_cast<float>(mRaceParms->GetNumLaps() - 1) + fFirstLapLength;
    } else {
        fSubsequentLapLength = fRaceLength;
        fFirstLapLength = fRaceLength;
    }

    {
        WRoadNav nav;

        nav.SetPathType(WRoadNav::kPathChopper);
        nav.InitAtPoint(UMath::Vector4To3(positions[numCheckpoints + 1]), UMath::Vector4To3(directions[numCheckpoints + 1]), true, 1.0f);
        if (nav.IsValid()) {
            for (int i = 0; i < 100; ++i) {
                WRoadSegment *segment;

                nav.IncNavPosition(1.0f, UMath::Vector3::kZero, 0.0f);
                segment = const_cast<WRoadSegment *>(nav.GetSegment());
                segment->fFlags |= 0x8000;
                if (nav.GetNodeInd() == 1) {
                    segment->fFlags |= 0x8004;
                } else {
                    segment->fFlags = (segment->fFlags & static_cast<unsigned short>(~4)) | 0x8000;
                }
            }
        }
    }

    nSpeedTraps = 0;
    for (unsigned int i = 0; i < GManager::Get().GetNumSpeedTraps() && nSpeedTraps < 16; ++i) {
        GTrigger *trigger = GManager::Get().GetSpeedTrap(i)->GetTrapTrigger();

        if (trigger) {
            aSpeedTraps[nSpeedTraps] = trigger;
            ++nSpeedTraps;
        }
    }
}

template void GRaceCustom::SetAttribute<int>(unsigned int key, const int &value, unsigned int index);
template void GRaceCustom::SetAttribute<bool>(unsigned int key, const bool &value, unsigned int index);
