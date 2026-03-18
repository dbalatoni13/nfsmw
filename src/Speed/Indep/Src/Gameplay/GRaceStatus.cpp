#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Src/Generated/Messages/MLoadingComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTime.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTimeExpired.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTimeSecTick.h"
#include "Speed/Indep/Src/Generated/Events/EAutoSave.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EReloadHud.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

#include <algorithm>

void SetCurrentTimeOfDay(float value);
void SetOverRideRainIntensity(float intensity);
extern int UnlockAllThings;

class Minimap {
  public:
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
};

GRaceStatus *GRaceStatus::fObj = nullptr;

DECLARE_CONTAINER_TYPE(ID_ROAD_SET);
DECLARE_CONTAINER_TYPE(ID_PATH_SET);

struct PathSegment {
    bool operator<(const PathSegment &rhs) const {
        return Length < rhs.Length;
    }

    float Length;
    UTL::Std::set<short, _type_ID_ROAD_SET> Roads;
};

template <typename T, int TotalBits, unsigned int FractionBits> struct FixedPoint {
    FixedPoint(float val)
        : mValue(static_cast<T>(static_cast<int>(val * static_cast<float>(GetScale())))) {}

    static int GetScale();

    T mValue;
};

template <typename T, int TotalBits, unsigned int FractionBits>
int FixedPoint<T, TotalBits, FractionBits>::GetScale() {
    return 1 << FractionBits;
}

template <typename T, int TotalBits, int ExponentBits, int BiasBits, int MantissaBits>
struct FloatingPoint {
    FloatingPoint(float val) {
        bool neg = false;
        int man = 0;
        int exp = 0;

        if (val < 0.0f) {
            neg = true;
            val = -val;
        }

        if (0.0f < val) {
            while (val < static_cast<float>(GetNormalizedLower())) {
                exp--;
                val *= 10.0f;
            }

            man = static_cast<int>(val);
            while (man >= GetNormalizedUpper()) {
                exp++;
                man /= 10;
            }
        }

        if (neg) {
            man = -man;
        }

        mValue = static_cast<T>(((exp & 0x1F) << MantissaBits) | (man & ((1 << MantissaBits) - 1)));
    }

    static int GetNormalizedLower();

    static int GetNormalizedUpper();

    T mValue;
};

template <typename T, int TotalBits, int ExponentBits, int BiasBits, int MantissaBits>
int FloatingPoint<T, TotalBits, ExponentBits, BiasBits, MantissaBits>::GetNormalizedLower() {
    return 1 << (MantissaBits - 1);
}

template <typename T, int TotalBits, int ExponentBits, int BiasBits, int MantissaBits>
int FloatingPoint<T, TotalBits, ExponentBits, BiasBits, MantissaBits>::GetNormalizedUpper() {
    return 1 << MantissaBits;
}

static const float Tweak_GlueSpreadData_Low[5] = {
    1000.0f,
    900.0f,
    750.0f,
    600.0f,
    800.0f,
};

static const float Tweak_GlueSpreadData_High[5] = {
    300.0f,
    275.0f,
    250.0f,
    150.0f,
    250.0f,
};

static const float Tweak_GlueStrengthData_Low[5] = {
    0.75f,
    0.75f,
    0.75f,
    0.5f,
    0.25f,
};

static const float Tweak_GlueStrengthData_High[5] = {
    0.75f,
    0.75f,
    0.75f,
    0.75f,
    0.25f,
};

static const float Tweak_QuickRaceGlue[3] = {
    0.0f,
    0.5f,
    1.0f,
};

Table Tweak_GlueSpreadTable_Low(Tweak_GlueSpreadData_Low, 5, 0.0f, 100.0f);
Table Tweak_GlueSpreadTable_High(Tweak_GlueSpreadData_High, 5, 0.0f, 100.0f);
Table Tweak_GlueStrengthTable_Low(Tweak_GlueStrengthData_Low, 5, 0.0f, 100.0f);
Table Tweak_GlueStrengthTable_High(Tweak_GlueStrengthData_High, 5, 0.0f, 100.0f);

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

IVehicle *GRacerInfo::CreateVehicle(unsigned int default_key) {
    GCharacter *racerChar = GetGameCharacter();
    unsigned int vehicle_key = default_key;
    FECustomizationRecord customizations;
    UMath::Vector3 direction = UMath::Vector3::kZero;
    UMath::Vector3 position = UMath::Vector3::kZero;
    IVehicleCache *cache = nullptr;
    ISimable *result;
    IVehicle *vehicle = nullptr;

    bMemSet(&customizations, 0, sizeof(customizations));

    if (racerChar) {
        const char *carName = racerChar->CarType(0);

        if (carName && *carName) {
            vehicle_key = Attrib::StringToKey(carName);
        }
    }

    if (GRaceStatus::Exists()) {
        cache = &GRaceStatus::Get();
    }

    result = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance("PVehicle", VehicleParams(cache, DRIVER_RACER, vehicle_key, direction, position, 0, &customizations, nullptr));
    if (result && result->QueryInterface(&vehicle)) {
        SetSimable(result);
    }

    return vehicle;
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

void GRacerInfo::Update(float dT) {
    ISimable *simable;
    IVehicleAI *vehicleAI;
    GRaceStatus &raceStatus = GRaceStatus::Get();
    IEngine *engine;
    IPlayer *player;
    UMath::Vector3 linearVelocity;
    float speed;
    float distance;
    float raceLength;

    if (IsFinishedRacing() || GetIsEngineBlown() || GetIsTotalled() || GetIsKnockedOut()) {
        return;
    }

    simable = GetSimable();
    if (!simable) {
        return;
    }

    engine = nullptr;
    if (simable->QueryInterface(&engine) && engine->IsNOSEngaged()) {
        mPoundsNOSUsed += dT * engine->GetNOSFlowRate();
    }

    player = simable->GetPlayer();
    if (player && player->InGameBreaker()) {
        mSpeedBreakerTime += dT;
    }

    simable->GetLinearVelocity(linearVelocity);
    speed = UMath::Length(linearVelocity);
    if (mTopSpeed < speed) {
        mTopSpeed = speed;
    }

    distance = mDistanceDriven + speed * dT;
    mTotalUpdateTime += dT;
    mDistanceDriven = distance;

#ifndef EA_BUILD_A124
    if (mQuarterMileTime == 0.0f) {
        static const float quarterMileInMeters = 402.335f;

        if (quarterMileInMeters <= distance) {
            mQuarterMileTime = GetRaceTime();
        }
    }

    if (mZeroToSixtyTime == 0.0f) {
        static const float sixtyMphInMetersPerSec = 26.8218f;

        if (sixtyMphInMetersPerSec <= mTopSpeed) {
            mZeroToSixtyTime = GetRaceTime();
        }
    }
#endif

    raceLength = raceStatus.GetRaceLength();
    vehicleAI = nullptr;
    if (simable->QueryInterface(&vehicleAI)) {
        mDistToNextCheckpoint = vehicleAI->GetPathDistanceRemaining();
    } else {
        mDistToNextCheckpoint = 0.0f;
    }

    if (raceLength > 0.0f) {
        float raceDistanceCompleted = 0.0f;
        int lapsCompleted = GetLapsCompleted();
        float lapDistanceCompleted = 0.0f;
        int checkpointsCompleted = GetChecksHitThisLap();
        float distanceToNextCheckpoint;
        float currentSegmentLength;

        if (lapsCompleted > 0) {
            raceDistanceCompleted = raceStatus.GetFirstLapLength();
        }

        if (lapsCompleted > 1) {
            raceDistanceCompleted += raceStatus.GetSubsequentLapLength() * static_cast<float>(lapsCompleted - 1);
        }

        for (int i = 0; i < checkpointsCompleted; ++i) {
            lapDistanceCompleted += raceStatus.GetSegmentLength(i, lapsCompleted);
        }

        distanceToNextCheckpoint = GetDistToNextCheck();
        currentSegmentLength = raceStatus.GetSegmentLength(checkpointsCompleted, lapsCompleted);
        if (distanceToNextCheckpoint != 0.0f) {
            float lapLength;

            lapDistanceCompleted += currentSegmentLength - distanceToNextCheckpoint;
            lapLength = raceStatus.GetLapLength(lapsCompleted);
            if (lapLength > 0.0f) {
                mPctLapComplete = bClamp(lapDistanceCompleted / lapLength, 0.0f, 1.0f);
            } else {
                mPctLapComplete = 1.0f;
            }

            mPctRaceComplete = bClamp((raceDistanceCompleted + lapDistanceCompleted) / raceLength, 0.0f, 1.0f);
        }
    }
}

void GRacerInfo::UpdateSplits() {
#ifndef EA_BUILD_A124
    int split = -1;

    if (mPctRaceComplete >= 1.0f && mSplitTimes[3] == 0.0f) {
        split = 3;
    } else if (mPctRaceComplete >= 0.75f && mSplitTimes[2] == 0.0f) {
        split = 2;
    } else if (mPctRaceComplete >= 0.5f && mSplitTimes[1] == 0.0f) {
        split = 1;
    } else if (mPctRaceComplete >= 0.25f && mSplitTimes[0] == 0.0f) {
        split = 0;
    }

    if (split != -1) {
        mSplitTimes[split] = mRaceTimer.GetTime();
        mSplitRankings[split] = mRanking;
    }
#endif
}

void GRacerInfo::FinalizeRaceStats() {
    GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
    float adjustedRaceTime;
    float pctRaceComplete = mPctRaceComplete;

    if (mFinishedRacing) {
        return;
    }

    adjustedRaceTime = GetRaceTime();
    if (0.1f < pctRaceComplete) {
        adjustedRaceTime = GetRaceTime() / pctRaceComplete;
    }

#ifndef EA_BUILD_A124
    mDNF = false;
#endif

    if (raceParms && raceParms->GetRaceType() == GRace::kRaceType_Drag &&
        (mTotalled || mEngineBlown || pctRaceComplete < 1.0f)) {
        adjustedRaceTime = 0.0f;
#ifndef EA_BUILD_A124
        mDNF = true;
#endif
    }

    if (raceParms && raceParms->GetRaceType() == GRace::kRaceType_SpeedTrap && mGameCharacter) {
        const unsigned int *pointPenalty =
            reinterpret_cast<const unsigned int *>(raceParms->GetGameplayObj()->GetAttributePointer(0x26FD42B0, 0));

        if (!pointPenalty) {
            pointPenalty = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
        }

        if (0.0f < adjustedRaceTime - GetRaceTime()) {
            AddToPointTotal(-((adjustedRaceTime - GetRaceTime()) * static_cast<float>(*pointPenalty)));
        }
    }

    if (raceParms && raceParms->GetIsLoopingRace() && mGameCharacter) {
        int numLaps = raceParms->GetNumLaps();
        float totalLapTime = 0.0f;

        for (int i = 0; i < numLaps; ++i) {
            float lapTime = GRaceStatus::Get().GetLapTime(i, GetIndex(), false);

            if (lapTime == 0.0f) {
                break;
            }

            totalLapTime += lapTime;
        }

        GRaceStatus::Get().SetLapTime(mLapsCompleted, GetIndex(), adjustedRaceTime - totalLapTime);
    }

#ifndef EA_BUILD_A124
    if (mGameCharacter) {
        static const float splitPct[4] = {
            0.25f,
            0.5f,
            0.75f,
            1.0f,
        };
        float effectivePct = mDNF ? pctRaceComplete : 1.0f;

        for (int i = 0; i < 4; ++i) {
            if (mSplitTimes[i] == 0.0f) {
                if (effectivePct < splitPct[i]) {
                    mSplitTimes[i] = 0.0f;
                } else {
                    mSplitTimes[i] = adjustedRaceTime * splitPct[i];
                }
            }

            if (mSplitRankings[i] == 0) {
                mSplitRankings[i] = mRanking;
            }
        }
    }

    if (!mDNF) {
        mRaceTimer.SetTime(adjustedRaceTime);
        FinishRace();
    }
#else
    mRaceTimer.SetTime(adjustedRaceTime);
    FinishRace();
#endif
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
    unsigned int *indexWords;
    char *indexBytes;
    UMath::Vector2 topLeft;
    UMath::Vector2 botRight;
    float timeOfDay;

    if (!index) {
        return;
    }

    indexWords = reinterpret_cast<unsigned int *>(index);
    indexBytes = reinterpret_cast<char *>(index);

    indexWords[0] = GetCollectionKey();
    indexWords[5] = GetEventHash();
    *reinterpret_cast<unsigned short *>(indexBytes + 0x20) = GetLocalizationTag();
    *reinterpret_cast<float *>(indexBytes + 0x1C) = GetRaceLengthMeters();
    *reinterpret_cast<unsigned short *>(indexBytes + 0x26) =
        FixedPoint<unsigned short, 10, 2>(GetRivalBestTime()).mValue;
    *reinterpret_cast<unsigned short *>(indexBytes + 0x24) =
        FloatingPoint<short, 10, 3, 5, 11>(static_cast<float>(GetReputation())).mValue;
    *reinterpret_cast<unsigned short *>(indexBytes + 0x22) =
        FloatingPoint<short, 10, 3, 5, 11>(GetCashValue()).mValue;
    indexWords[4] = GetChallengeType();
    *reinterpret_cast<unsigned short *>(indexBytes + 0x0E) =
        FloatingPoint<short, 10, 3, 5, 11>(GetChallengeGoal()).mValue;
    indexBytes[0x28] = static_cast<char>(GetNumLaps());
    indexBytes[0x2A] = static_cast<char>(GetCopDensity());
    indexBytes[0x2B] = static_cast<char>(GetRaceType());
    indexBytes[0x29] = static_cast<char>(GetRegion());

    flags = 0;
    indexWords[6] = 0;
    if (GetInitiallyUnlockedQuickRace()) {
        flags |= 1 << 0;
    }
    indexWords[6] = flags;
    if (GetInitiallyUnlockedOnline()) {
        flags |= 1 << 1;
    }
    indexWords[6] = flags;
    if (GetInitiallyUnlockedChallenge()) {
        flags |= 1 << 2;
    }
    indexWords[6] = flags;
    if (GetCanBeReversed()) {
        flags |= 1 << 3;
    }
    indexWords[6] = flags;
    if (GetIsDDayRace()) {
        flags |= 1 << 4;
    }
    indexWords[6] = flags;
    if (GetIsBossRace()) {
        flags |= 1 << 5;
    }
    indexWords[6] = flags;
    if (GetIsMarkerRace()) {
        flags |= 1 << 6;
    }
    indexWords[6] = flags;
    if (GetIsPursuitRace()) {
        flags |= 1 << 7;
    }
    indexWords[6] = flags;
    if (GetIsLoopingRace()) {
        flags |= 1 << 8;
    }
    indexWords[6] = flags;
    if (GetRankPlayersByPoints()) {
        flags |= 1 << 9;
    }
    indexWords[6] = flags;
    if (GetRankPlayersByDistance()) {
        flags |= 1 << 10;
    }
    indexWords[6] = flags;
    if (GetCopsEnabled()) {
        flags |= 1 << 11;
    }
    indexWords[6] = flags;
    if (GetScriptedCopsInRace()) {
        flags |= 1 << 12;
    }
    indexWords[6] = flags;
    timeOfDay = GetTimeOfDay();
    if (0.8f < timeOfDay) {
        flags |= 1 << 13;
    }
    indexWords[6] = flags;
    if (GetNeverInQuickRace()) {
        flags |= 1 << 14;
    }
    indexWords[6] = flags;
    if (GetIsChallengeSeriesRace()) {
        flags |= 1 << 15;
    }
    indexWords[6] = flags;
    if (GetIsCollectorsEditionRace()) {
        flags |= 1 << 16;
    }
    indexWords[6] = flags;
    timeOfDay = GetTimeOfDay();
    if (timeOfDay < 0.8f) {
        timeOfDay = GetTimeOfDay();
        if (0.0f <= timeOfDay) {
            flags = indexWords[6] | (1 << 17);
            indexWords[6] = flags;
        }
    } else {
        flags = indexWords[6] | (1 << 13);
        indexWords[6] = flags;
    }

    bMemSet(indexWords + 1, 0, 10);
    bSafeStrCpy(reinterpret_cast<char *>(indexWords + 1), GetEventID(), 10);

    GetBoundingBox(topLeft, botRight);
    indexBytes[0x2C] = static_cast<char>(topLeft.x * 255.0f);
    indexBytes[0x2D] = static_cast<char>(topLeft.y * 255.0f);
    indexBytes[0x2E] = static_cast<char>(botRight.x * 255.0f);
    indexBytes[0x2F] = static_cast<char>(botRight.y * 255.0f);
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
    UMath::Vector3 pos;
    float x1;
    float x2;
    float y1;
    float y2;
    TrackInfo *trackInfo;

    if (mIndex) {
        const unsigned char *indexBytes;

        indexBytes = reinterpret_cast<const unsigned char *>(mIndex);
        topLeft.x = indexBytes[0x2C] * (1.0f / 255.0f);
        topLeft.y = indexBytes[0x2D] * (1.0f / 255.0f);
        botRight.x = indexBytes[0x2E] * (1.0f / 255.0f);
        botRight.y = indexBytes[0x2F] * (1.0f / 255.0f);
        return;
    }

    EnsureLoaded();
    GetStartPosition(pos);
    x1 = pos.z;
    x2 = pos.z;
    y1 = -pos.x;
    y2 = y1;

    if (HasFinishLine()) {
        GetFinishPosition(pos);
        x1 = std::min(x1, pos.z);
        x2 = std::max(x2, pos.z);
        y1 = std::min(y1, -pos.x);
        y2 = std::max(y2, -pos.x);
    }

    for (unsigned int onCheck = 0; onCheck < GetNumCheckpoints(); ++onCheck) {
        GetCheckpointPosition(onCheck, pos);
        x1 = std::min(x1, pos.z);
        x2 = std::max(x2, pos.z);
        y1 = std::min(y1, -pos.x);
        y2 = std::max(y2, -pos.x);
    }

    trackInfo = TrackInfo::GetTrackInfo(2000);
    bVector2 topLeftWorld(x1, y1);
    bVector2 botRightWorld(x2, y2);
    bVector2 topLeftMap;
    bVector2 botRightMap;
    Minimap::ConvertPos(topLeftWorld, topLeftMap, trackInfo);
    Minimap::ConvertPos(botRightWorld, botRightMap, trackInfo);
    topLeft.x = topLeftMap.x;
    topLeft.y = topLeftMap.y;
    botRight.x = botRightMap.x;
    botRight.y = botRightMap.y;
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
    if (mReversed) {
        if (!GetCanBeReversed()) {
            mReversed = false;
        } else {
            const unsigned int *reverse_start = reinterpret_cast<const unsigned int *>(
                mRaceRecord->GetAttributePointer(0xFD945479, 0) ? mRaceRecord->GetAttributePointer(0xFD945479, 0) : Attrib::DefaultDataArea(sizeof(unsigned int)));
            const unsigned int *reverse_finish = reinterpret_cast<const unsigned int *>(
                mRaceRecord->GetAttributePointer(0x7C7CF20F, 0) ? mRaceRecord->GetAttributePointer(0x7C7CF20F, 0) : Attrib::DefaultDataArea(sizeof(unsigned int)));
            Attrib::Attribute race_start = mRaceRecord->Get(0xE43B2CCC);
            Attrib::Attribute race_finish = mRaceRecord->Get(0xB0A24ADC);

            if (race_start.IsValid() && race_start.GetCollection() != mRaceRecord->GetConstCollection()) {
                unsigned int length = race_start.GetLength();
                unsigned int *values = length ? new unsigned int[length] : nullptr;

                for (unsigned int i = 0; i < length; ++i) {
                    const unsigned int *src =
                        reinterpret_cast<const unsigned int *>(mRaceRecord->GetAttributePointer(0xE43B2CCC, i));
                    values[i] = src ? *src : 0;
                }

                if (mRaceRecord->Add(0xE43B2CCC, length)) {
                    for (unsigned int i = 0; i < length; ++i) {
                        unsigned int *dst =
                            reinterpret_cast<unsigned int *>(const_cast<void *>(mRaceRecord->GetAttributePointer(0xE43B2CCC, i)));

                        if (dst) {
                            *dst = values[i];
                        }
                    }

                    race_start = mRaceRecord->Get(0xE43B2CCC);
                }

                delete[] values;
            }

            if (race_start.IsValid()) {
                unsigned int *dst =
                    reinterpret_cast<unsigned int *>(const_cast<void *>(mRaceRecord->GetAttributePointer(0xE43B2CCC, 0)));

                if (dst) {
                    *dst = *reverse_start;
                }
            }

            if (race_finish.IsValid() && race_finish.GetCollection() != mRaceRecord->GetConstCollection()) {
                unsigned int length = race_finish.GetLength();
                unsigned int *values = length ? new unsigned int[length] : nullptr;

                for (unsigned int i = 0; i < length; ++i) {
                    const unsigned int *src =
                        reinterpret_cast<const unsigned int *>(mRaceRecord->GetAttributePointer(0xB0A24ADC, i));
                    values[i] = src ? *src : 0;
                }

                if (mRaceRecord->Add(0xB0A24ADC, length)) {
                    for (unsigned int i = 0; i < length; ++i) {
                        unsigned int *dst =
                            reinterpret_cast<unsigned int *>(const_cast<void *>(mRaceRecord->GetAttributePointer(0xB0A24ADC, i)));

                        if (dst) {
                            *dst = values[i];
                        }
                    }

                    race_finish = mRaceRecord->Get(0xB0A24ADC);
                }

                delete[] values;
            }

            if (race_finish.IsValid()) {
                unsigned int *dst =
                    reinterpret_cast<unsigned int *>(const_cast<void *>(mRaceRecord->GetAttributePointer(0xB0A24ADC, 0)));

                if (dst) {
                    *dst = *reverse_finish;
                }
            }
        }
    }

    {
        unsigned int num_opponents = GetNumOpponents();
        bool boss_race = false;

        if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career && GetIsBossRace()) {
            boss_race = true;
            num_opponents = 0;
        }

        if (mNumOpponents != num_opponents) {
            unsigned int opponent_keys[16];
            unsigned int old_length = 0;

            if (!boss_race) {
                Attrib::Attribute opponents = mRaceRecord->Get(0x5839FA1A);

                old_length = opponents.GetLength();
                for (unsigned int i = 0; i < old_length && i < 16; ++i) {
                    const unsigned int *src = reinterpret_cast<const unsigned int *>(mRaceRecord->GetAttributePointer(0x5839FA1A, i));

                    if (!src) {
                        src = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
                    }

                    opponent_keys[i] = *src;
                }
            }

            if (old_length < mNumOpponents && mNumOpponents <= 16) {
                if (old_length == 0) {
                    for (unsigned int i = 0; i < mNumOpponents; ++i) {
                        opponent_keys[i] = 0x9F3B88C5;
                    }
                } else {
                    for (unsigned int i = old_length; i < mNumOpponents; ++i) {
                        opponent_keys[i] = opponent_keys[i % old_length];
                    }
                }
            }

            mRaceRecord->Add(0x5839FA1A, mNumOpponents);
            if (mNumOpponents != 0) {
                for (unsigned int i = 0; i < mNumOpponents && i < 16; ++i) {
                    unsigned int *dst =
                        reinterpret_cast<unsigned int *>(const_cast<void *>(mRaceRecord->GetAttributePointer(0x5839FA1A, i)));

                    if (dst) {
                        *dst = opponent_keys[i];
                    }
                }
            }
        }
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

GRaceStatus::~GRaceStatus() {
    if (mCheckpointModel) {
        delete mCheckpointModel;
        mCheckpointModel = nullptr;
    }

    if (mCheckpointEmitter) {
        mCheckpointEmitter->UnSubscribe();
        delete mCheckpointEmitter;
        mCheckpointEmitter = nullptr;
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

bool GRaceStatus::CanUnspawnRoamer(const IVehicle *roamer) const {
    const GRacerInfo *info = nullptr;

    if (roamer->IsActive()) {
        for (int onRacer = 0; onRacer < GetRacerCount(); ++onRacer) {
            const GRacerInfo &racerInfo = mRacerInfo[onRacer];

            if (UTL::COM::ComparePtr(racerInfo.GetSimable(), roamer)) {
                info = &racerInfo;
                break;
            }
        }

        if (info) {
            if (roamer->GetOffscreenTime() >= 4.0f) {
                return Sim::DistanceToCamera(roamer->GetPosition()) >= 100.0f;
            }

            return false;
        }
    }

    return true;
}

eVehicleCacheResult GRaceStatus::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    if (mPlayMode == kPlayMode_Racing || mVehicleCacheLocked) {
        for (int i = 0; i < GetRacerCount(); ++i) {
            if (UTL::COM::ComparePtr(mRacerInfo[i].GetSimable(), removethis)) {
                return VCR_WANT;
            }
        }
    } else {
        if (!UTL::COM::ComparePtr(whosasking, ICopMgr::Get()) && !UTL::COM::ComparePtr(whosasking, ITrafficMgr::Get())) {
            return VCR_DONTCARE;
        }

        if (!CanUnspawnRoamer(removethis)) {
            return VCR_WANT;
        }
    }

    return VCR_DONTCARE;
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

void GRaceStatus::EnterBin(unsigned int binNumber) {
    if (mRaceBin) {
        DisableBarriers();
    }

    mRaceBin = GRaceDatabase::Get().GetBinNumber(binNumber);
    if (mRaceBin) {
        EnableBarriers();
        if (mRaceBin->GetChildVault() && !mRaceBin->GetChildVault()->IsLoaded()) {
            mRaceBin->GetChildVault()->LoadSyncTransient();
        }
    }

    if (GManager::Get().GetInGameplay()) {
        GManager::Get().StartWorldActivities(true);
        GManager::Get().StartBinActivity(mRaceBin);
        GManager::Get().RefreshEngageTriggerIcons();
        GManager::Get().RefreshSpeedTrapIcons();
    }
}

void GRaceStatus::SetRacing() {
    mPlayMode = kPlayMode_Racing;
    ClearTimes();

    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_ALL).begin(); iter != IPlayer::GetList(PLAYER_ALL).end(); ++iter) {
        IPlayer *player = *iter;

        if (player->InGameBreaker()) {
            player->ResetGameBreaker(true);
        }
    }

    mNumTollbooths = 0;

    if ((!mRaceParms || !mRaceParms->GetIsDDayRace() || bStrCmp(mRaceParms->GetEventID(), "16.1.0") == 0) && !IsFinalEpicPursuit()) {
        new EAutoSave();
    }

    new EReloadHud();

#ifndef EA_BUILD_A124
    mCaluclatedAdaptiveGain = false;
#endif
}

void GRaceStatus::NotifyScriptWhenLoaded() {
    bool racersLoading = IsLoading();
    bool trackLoading = TheTrackStreamer.IsLoadingInProgress();
    bool copsSpawning = false;

    if (ICopMgr::Get() && ICopMgr::Get()->IsCopSpawnPending()) {
        copsSpawning = true;
    }

    if (trackLoading) {
        if (!racersLoading) {
            if (!copsSpawning) {
                if (!TheTrackStreamer.IsFarLoadingInProgress()) {
                    trackLoading = false;
                }
            }
        }
    }

    if (racersLoading || trackLoading || copsSpawning) {
        new EFadeScreenOn(false);
    }

    mScriptWaitingForLoad = true;
}

void GRaceStatus::SetRoaming() {
    bool skipHudReload = false;

    if (mRaceParms) {
        skipHudReload = bStrCmp(mRaceParms->GetEventID(), "16.2.1") == 0;
    }

    mPlayMode = kPlayMode_Roaming;
    SetRaceContext(GRace::kRaceContext_Career);
    mIsLoading = false;
    mRaceParms = nullptr;
    WRoadNetwork::Get().ResetShortcuts();

    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_ALL).begin(); iter != IPlayer::GetList(PLAYER_ALL).end(); ++iter) {
        IPlayer *player = *iter;

        if (player->InGameBreaker()) {
            player->ResetGameBreaker(true);
        }
    }

    if (!skipHudReload) {
        new EReloadHud();
    }

    new EAutoSave();
    SetOverRideRainIntensity(0.0f);
    SetCurrentTimeOfDay(0.5f);
    GManager::Get().SpawnAllLoadedSectionIcons();
    ICopMgr::mDisableCops = 0;
}

void GRaceStatus::CalculateRankings() {
    GRacerInfo *rankings[16];
    bool rankPlayersByPoints = mRaceParms->GetRankPlayersByPoints();
    int aiRacers = 0;

    for (int i = 0; i < mRacerCount; ++i) {
        rankings[i] = &mRacerInfo[i];
    }

    for (int i = mRacerCount - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            bool swap;

            if (!rankPlayersByPoints) {
                swap = rankings[j]->IsBehind(*rankings[i]);
            } else if (rankings[i]->GetPointTotal() + rankings[j]->GetPointTotal() <= 0.0f) {
                swap = rankings[j]->IsBehind(*rankings[i]);
            } else {
                swap = rankings[j]->GetPointTotal() < rankings[i]->GetPointTotal();
            }

            if (swap) {
                GRacerInfo *info = rankings[i];

                rankings[i] = rankings[j];
                rankings[j] = info;
            }
        }
    }

    for (int i = 0; i < mRacerCount; ++i) {
        GRacerInfo *info = rankings[i];

        info->SetRanking(i + 1);
        if (!info->GetGameCharacter()) {
            info->mAiRanking = 0;
            ++aiRacers;
        } else {
            info->mAiRanking = (i + 1) - aiRacers;
        }
    }
}

void GRaceStatus::SortCheckPointRankings() {
    GRacerInfo *rankings[16];

    for (int checkpoint = 0; checkpoint < 16; ++checkpoint) {
        for (int i = 0; i < mRacerCount; ++i) {
            rankings[i] = &mRacerInfo[i];
        }

        for (int i = mRacerCount - 1; i > 0; --i) {
            for (int j = 0; j < i; ++j) {
                GRacerInfo *info = rankings[i];

                if (rankings[j]->GetSpeedTrapSpeed(checkpoint) < info->GetSpeedTrapSpeed(checkpoint)) {
                    rankings[i] = rankings[j];
                    rankings[j] = info;
                }
            }
        }

        for (int i = 0; i < mRacerCount; ++i) {
            rankings[i]->mSpeedTrapPosition[checkpoint] = i + 1;
        }
    }
}

void GRaceStatus::Update(float dT) {
    int numRacers = mRacerCount;

#ifndef EA_BUILD_A124
    if (GetPlayMode() == kPlayMode_Racing && mRefreshBinAfterRace) {
        RefreshBinWhileInGame();
        mRefreshBinAfterRace = false;
    }
#endif

    if (GetPlayMode() == kPlayMode_Roaming) {
        if (mQueueBinChange) {
            EnterBin(FEDatabase->GetCareerSettings()->GetCurrentBin());
            mQueueBinChange = false;
        }

#ifndef EA_BUILD_A124
        if (GetPlayMode() == kPlayMode_Roaming && mWarpWhenInFreeRoam && GManager::Get().WarpToMarker(mWarpWhenInFreeRoam, false)) {
            mWarpWhenInFreeRoam = 0;
        }
#endif
    }

    if (GetPlayMode() == kPlayMode_Racing && numRacers > 0) {
        int numAiRacers = 0;
        float floatRacers = 0.0f;
        float percentComplete = 0.0f;
        float elapsed;
        int elapsedSec;

        for (int idx = 0; idx < numRacers; ++idx) {
            GRacerInfo &info = GetRacerInfo(idx);

            if (info.GetGameCharacter()) {
                ++numAiRacers;
            }
        }

        for (int idx = 0; idx < numRacers; ++idx) {
            GRacerInfo &info = GetRacerInfo(idx);

            info.Update(dT);
            if (info.GetSimable()) {
                float weight = 1.0f;

                if (!info.GetGameCharacter()) {
                    weight = bMax(1.0f, static_cast<float>(numAiRacers));
                }

                floatRacers += weight;
                percentComplete += weight * info.GetPctRaceComplete();
            }
        }

        fAveragePercentComplete = percentComplete / bMax(1.0f, floatRacers);
        CalculateRankings();

#ifndef EA_BUILD_A124
        for (int idx = 0; idx < numRacers; ++idx) {
            GetRacerInfo(idx).UpdateSplits();
        }
#endif

        elapsed = GetRaceTimeElapsed();
        MNotifyRaceTime(elapsed, GetIsTimeLimited(), GetRaceTimeRemaining()).Post(UCrc32(0x20D60DBF));

        elapsedSec = static_cast<int>(elapsed);
        if (mLastSecondTickSent < elapsedSec) {
            mLastSecondTickSent = elapsedSec;
            MNotifyRaceTimeSecTick(elapsed).Post(UCrc32(0x20D60DBF));
        }

        if (GetIsTimeLimited()) {
            if (IsChallengeRace()) {
#ifndef EA_BUILD_A124
                if (mPlayerPursuitInCooldown) {
                    if (mRaceMasterTimer.IsRunning()) {
                        mRaceMasterTimer.Stop();
                    }
                } else if (!mRaceMasterTimer.IsRunning()) {
                    mRaceMasterTimer.Start();
                }
#endif
            }

            if (!mTimeExpiredMsgSent && GetRaceTimeRemaining() <= 0.0f) {
                MNotifyRaceTimeExpired().Post(UCrc32(0x20D60DBF));
                mTimeExpiredMsgSent = true;

                for (int idx = 0; idx < mRacerCount; ++idx) {
                    GRacerInfo &info = mRacerInfo[idx];

                    if (!info.GetIsHuman() && !info.IsFinishedRacing()) {
                        info.ForceStop();
                    }
                }
            }
        }
    }

    if (mScriptWaitingForLoad) {
        bool racersLoading = IsLoading();
        bool trackLoading = TheTrackStreamer.IsLoadingInProgress();
        bool copsSpawning = false;
        ICopMgr *copMgr = ICopMgr::Get();

        if (copMgr && copMgr->IsCopSpawnPending()) {
            copsSpawning = true;
        }

        if (!racersLoading && !trackLoading && !copsSpawning) {
            MLoadingComplete().Post(UCrc32(0x20D60DBF));
            mScriptWaitingForLoad = false;
        }
    }
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

void GRaceStatus::SetNextCheckpointPos(GRuntimeInstance *trigger) {
    bool checkpointsVisible = false;

    mNextCheckpoint = static_cast<GTrigger *>(trigger);

    if (mRaceParms) {
        checkpointsVisible = mRaceParms->GetCheckpointsVisible();
    }

    if (!trigger || !checkpointsVisible) {
        if (mCheckpointModel) {
            delete mCheckpointModel;
        }
        mCheckpointModel = nullptr;

        if (mCheckpointEmitter) {
            mCheckpointEmitter->UnSubscribe();
            delete mCheckpointEmitter;
            mCheckpointEmitter = nullptr;
        }

        return;
    }

    if (!mCheckpointModel) {
        mCheckpointModel = new WorldModel(0x738B1F9B, nullptr, false);
    }

    UMath::Vector3 position;
    bMatrix4 matrix;

    trigger->GetPosition(position);
    bIdentity(&matrix);
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(position), reinterpret_cast<bVector3 &>(matrix.v3));
    mCheckpointModel->SetMatrix(&matrix);

    if (mCheckpointEmitter) {
        mCheckpointEmitter->SetLocalWorld(&matrix);
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

void GRaceStatus::UpdateAdaptiveDifficulty(eAdaptiveGainReason reason, ISimable *who) {
    bool update = false;
    GRacerInfo *winning_player = nullptr;
    GRacerInfo *winning_ai = nullptr;
    GRacerInfo *eliminated_player = nullptr;
    float difficulty = fCatchUpAdaptiveBonus;
    const int num_racers = GetRacerCount();

    if (mCaluclatedAdaptiveGain || GetRaceContext() != GRace::kRaceContext_Career || num_racers < 2 || Sim::GetUserMode() != 0 || GetPlayMode() != kPlayMode_Racing ||
        (mRaceParms && mRaceParms->GetNoPostRaceScreen()) || GetRaceLength() <= 0.0f) {
        return;
    }

    for (int i = 0; i < num_racers; ++i) {
        GRacerInfo &info = GetRacerInfo(i);

        if (!info.GetGameCharacter()) {
            if (info.GetIsBusted()) {
                return;
            }

            if (info.IsFinishedRacing() && info.GetRanking() == 1) {
                winning_player = &info;
            } else if (GetRaceType() == GRace::kRaceType_Knockout && info.GetIsKnockedOut()) {
                eliminated_player = &info;
            }
        } else if (info.IsFinishedRacing() && info.GetRanking() == 1) {
            winning_ai = &info;
        }
    }

    if (reason < kAdaptiveGainReason_2) {
        float percent_ai_complete = 0.0f;
        float percent_human_complete = 0.0f;

        if (!who || !who->IsPlayer()) {
            return;
        }

        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo &info = GetRacerInfo(i);

            if (!info.GetIsKnockedOut()) {
                if (!info.GetGameCharacter()) {
                    percent_human_complete = UMath::Max(percent_human_complete, info.GetPctRaceComplete());
                } else {
                    percent_ai_complete = UMath::Max(percent_ai_complete, info.GetPctRaceComplete());
                }
            }
        }

        if (percent_human_complete < percent_ai_complete && percent_human_complete > 0.0f) {
            float lose_margin = ((percent_ai_complete - percent_human_complete) * (GetRaceLength() * 0.01f)) / 300.0f;
            float bonus = bClamp(lose_margin, 0.0f, 1.0f);

            difficulty = bClamp(difficulty + (((bonus * -0.4f) * percent_human_complete) * 0.01f), -1.0f, 1.0f);
            update = true;
        }
    } else if (reason == kAdaptiveGainReason_5) {
        float percent_ai_complete = 0.0f;
        float percent_human_complete = 0.0f;

        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo &info = GetRacerInfo(i);

            if (!info.GetIsKnockedOut() && !info.GetIsTotalled() && !info.GetIsEngineBlown()) {
                if (!info.GetGameCharacter()) {
                    percent_human_complete = UMath::Max(percent_human_complete, info.GetPctRaceComplete());
                } else {
                    percent_ai_complete = UMath::Max(percent_ai_complete, info.GetPctRaceComplete());
                }
            }
        }

        if (percent_human_complete < percent_ai_complete && percent_human_complete > 0.0f) {
            float lose_margin = ((percent_ai_complete - percent_human_complete) * (GetRaceLength() * 0.01f)) / 300.0f;
            float bonus = bClamp(lose_margin, 0.0f, 1.0f);

            difficulty = bClamp(difficulty + (((bonus * -0.4f) * percent_human_complete) * 0.01f), -1.0f, 1.0f);
            update = true;
        }
    } else if (GetRaceType() == GRace::kRaceType_SpeedTrap) {
        float player_points = 0.0f;
        float ai_points = 0.0f;
        float total_points;

        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo &info = GetRacerInfo(i);

            if (!info.IsFinishedRacing()) {
                return;
            }

            if (!info.GetGameCharacter()) {
                player_points = info.GetPointTotal();
            } else {
                ai_points = UMath::Max(ai_points, info.GetPointTotal());
            }
        }

        total_points = UMath::Max(player_points, ai_points);
        if (total_points > 0.0f && player_points > 0.0f && ai_points > 0.0f) {
            float point_spread_ratio = (player_points - ai_points) / total_points;

            if (point_spread_ratio <= 0.0f) {
                float bonus = bClamp((-point_spread_ratio) / 0.2f, 0.0f, 1.0f);

                difficulty = bClamp(difficulty + (bonus * -0.2f), -1.0f, 1.0f);
            } else {
                float bonus = bClamp((point_spread_ratio - 0.05f) / (0.2f - 0.05f), 0.0f, 1.0f);

                difficulty = bClamp(difficulty + (bonus * 0.2f), -1.0f, 1.0f);
            }

            update = true;
        }
    } else if (winning_ai) {
        float max_pct_complete = 0.0f;

        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo *info = &GetRacerInfo(i);

            if (info != winning_ai && !info->IsFinishedRacing() && !info->GetIsTotalled() && !info->GetIsEngineBlown()) {
                max_pct_complete = bMax(max_pct_complete, info->GetPctRaceComplete());
            }
        }

        {
            float win_margin = (GetRaceLength() * (100.0f - max_pct_complete)) * 0.01f;

            if (win_margin > 200.0f && max_pct_complete > 0.0f) {
                float bonus = bClamp((win_margin - 200.0f) / (750.0f - 200.0f), 0.0f, 1.0f);

                difficulty = bClamp(difficulty + (bonus * 0.4f), -1.0f, 1.0f);
                update = true;
            }
        }
    } else if (winning_player) {
        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo *info = &GetRacerInfo(i);

            if (info->GetGameCharacter() && !info->IsFinishedRacing()) {
                float lose_margin = (GetRaceLength() * (100.0f - info->GetPctRaceComplete())) * 0.01f;

                if (lose_margin > 0.0f) {
                    float bonus = bClamp(lose_margin / 300.0f, 0.0f, 1.0f);

                    difficulty = bClamp(difficulty + ((((bonus * -0.3f) + -0.1f) * info->GetPctRaceComplete()) * 0.01f), -1.0f, 1.0f);
                    update = true;
                }
            }
        }
    } else if (eliminated_player) {
        int lap_count = 1;
        float num_laps;
        float race_lose_margin;
        float lose_margin;

        if (mRaceParms) {
            lap_count = bMax(1, mRaceParms->GetNumLaps());
        }

        num_laps = static_cast<float>(lap_count);
        lose_margin = (GetRaceLength() * (100.0f - eliminated_player->GetPctRaceComplete())) * 0.01f;
        race_lose_margin = GetRaceLength() / num_laps;
        lose_margin -= bFloor(lose_margin / race_lose_margin) * race_lose_margin;

        if (lose_margin > 0.0f) {
            float bonus = bClamp(lose_margin / 300.0f, 0.0f, 1.0f);

            difficulty = bClamp(difficulty + ((((bonus * (-0.4f - -0.1f)) + -0.1f) * eliminated_player->GetPctRaceComplete()) * 0.01f), -1.0f, 1.0f);
            update = true;
        }
    }

    if (update) {
        float clamped_difficulty;

        mCaluclatedAdaptiveGain = true;
        clamped_difficulty = bClamp(difficulty, -1.0f, 1.0f);

        if (FEDatabase) {
            CareerSettings *career = FEDatabase->GetCareerSettings();

            if (career) {
                *reinterpret_cast<short *>(reinterpret_cast<char *>(career) + 0x10) = static_cast<short>(clamped_difficulty * 32767.0f);
            }
        }

        fCatchUpAdaptiveBonus = difficulty;
    }
}

bool GRaceStatus::ComputeCatchUpSkill(GRacerInfo *racer_info, PidError *pid, float *output, float *skill, bool off_world) {
    float glue_level = 0.5f;
    bool is_boss = false;
    bool use_race_override = false;
    float percent_complete = racer_info->GetPctRaceComplete();
    float glue_skill;
    float glue_spread;
    float glue_integral;
    float glue_derivative;
    float glue_p = pid->GetErrorIntegral();
    float glue_error = pid->GetErrorDerivative();
    float glue_output;

    if (!off_world) {
        if (GetRaceContext() == GRace::kRaceContext_QuickRace) {
            if (!mRaceParms || !mRaceParms->GetCatchUp()) {
                return false;
            }

            glue_level = Tweak_QuickRaceGlue[mRaceParms->GetDifficulty()];
        } else {
            if (GetRaceContext() != GRace::kRaceContext_Career) {
                return false;
            }

            glue_level = bClamp((GetAdaptiveDifficutly() + 1.0f) * 0.5f, 0.0f, 1.0f);

            if (mRaceParms) {
                use_race_override = mRaceParms->GetCatchUpOverride();
                if (mRaceParms->GetIsBossRace()) {
                    is_boss = true;
                    glue_level = ((1.0f - glue_level) * 0.5f) + glue_level;
                }
            }
        }
    } else {
        glue_level = 1.0f;
    }

    if (use_race_override) {
        Table glue_skill_table(aCatchUpSkillData, nCatchUpSkillEntries, 0.0f, 100.0f);
        Table glue_spread_table(aCatchUpSpreadData, nCatchUpSpreadEntries, 0.0f, 100.0f);

        glue_skill = glue_skill_table.GetValue(percent_complete);
        glue_spread = glue_spread_table.GetValue(percent_complete);
        glue_integral = fCatchUpIntegral;
        glue_derivative = fCatchUpDerivative;
    } else {
        glue_spread = ((Tweak_GlueSpreadTable_High.GetValue(percent_complete) - Tweak_GlueSpreadTable_Low.GetValue(percent_complete)) * glue_level) +
                      Tweak_GlueSpreadTable_Low.GetValue(percent_complete);
        glue_skill = ((Tweak_GlueStrengthTable_High.GetValue(percent_complete) - Tweak_GlueStrengthTable_Low.GetValue(percent_complete)) * glue_level) +
                     Tweak_GlueStrengthTable_Low.GetValue(percent_complete);
        glue_integral = 5.0e-5f;
        glue_derivative = 0.01f;
    }

    glue_spread = bMax(glue_spread, 100.0f);
    glue_output = bClamp((2.0f / glue_spread) * pid->GetError() + glue_p * glue_integral + glue_error * glue_derivative, -1.0f, 1.0f);
    *skill = glue_skill * glue_output;

    if (is_boss) {
        glue_output = 1.0f;
    }

    *output = glue_output;
    return true;
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
    for (int i = 0; i < GetRacerCount(); ++i) {
        ISimable *simable = GetRacerInfo(i).GetSimable();

        if (simable) {
            IAudible *iaudible;

            if (simable->QueryInterface(&iaudible) && !iaudible->IsAudible()) {
                return true;
            }
        }
    }

    return false;
}

bool GRaceStatus::IsModelsLoading() {
    for (int i = 0; i < GetRacerCount(); ++i) {
        ISimable *simable = GetRacerInfo(i).GetSimable();

        if (simable) {
            IRenderable *irenderable;

            if (simable->QueryInterface(&irenderable) && !irenderable->IsRenderable()) {
                return true;
            }
        }
    }

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
    float segmentDistance = 0.0f;
    UTL::Std::set<PathSegment, _type_ID_PATH_SET> pathSegments;
    char shortcutAllowed[32];

    nav.SetPathType(WRoadNav::kPathChopper);

    bSub(&delta, reinterpret_cast<const bVector3 *>(&positions[start]), reinterpret_cast<const bVector3 *>(&positions[end]));
    pathDistance = bLength(delta);

    nav.InitAtPoint(UMath::Vector4To3(positions[start]), UMath::Vector4To3(directions[start]), true, 1.0f);
    if (!nav.IsValid()) {
        bRaceRouteError = true;
        return pathDistance;
    }

    if (start == end) {
        short segment = nav.GetSegmentInd();

        do {
            float distance = static_cast<float>(nav.GetSegment()->nLength) * 0.015259022f * (1.0f - nav.GetSegTime());

            if (distance < 0.01f) {
                distance = 0.01f;
            }

            segmentDistance += distance;
            nav.IncNavPosition(distance, UMath::Vector4To3(directions[end]), 0.0f);
        } while (segment == nav.GetSegmentInd());
    }

    bMemSet(shortcutAllowed, 1, sizeof(shortcutAllowed));

    {
        bool noShortcuts = true;

        do {
            PathSegment pathSegment;

            nav.FindPathNow(&UMath::Vector4To3(positions[end]), &UMath::Vector4To3(directions[end]), shortcutAllowed);
            if (nav.GetNavType() != WRoadNav::kTypePath) {
                break;
            }

            WRoadNetwork::Get().AddRaceSegments(&nav);
            pathDistance = nav.GetPathDistanceRemaining();

            {
                unsigned char shortcut = nav.FirstShortcutInPath();

                noShortcuts = shortcut == 0xFF;
                if (!noShortcuts) {
                    shortcutAllowed[shortcut] = 0;
                }
            }

            pathSegment.Length = pathDistance;
            for (int i = 0; i < nav.GetNumPathSegments(); ++i) {
                const WRoadSegment *roadSegment = WRoadNetwork::Get().GetSegment(nav.GetPathSegment(i));

                if (!(roadSegment->fFlags & 1)) {
                    pathSegment.Roads.insert(roadSegment->fRoadID);
                }
            }

            pathSegments.insert(pathSegment);
        } while (!noShortcuts);

        pathDistance += segmentDistance;
        if (noShortcuts && pathSegments.size() == 0) {
            bRaceRouteError = true;
        }
    }

    if (pathSegments.size() > 1) {
        PathSegment *sortedPaths[32];
        int count = 0;

        for (UTL::Std::set<PathSegment, _type_ID_PATH_SET>::iterator it = pathSegments.begin(); it != pathSegments.end(); ++it) {
            sortedPaths[count++] = const_cast<PathSegment *>(&*it);
        }

        for (int i = 1; i < count; ++i) {
            PathSegment *current = sortedPaths[i];
            PathSegment *previous = sortedPaths[i - 1];
            UTL::Std::set<short, _type_ID_ROAD_SET> roadDiff;
            float roadDiffLength = 0.0f;

            std::set_difference(
                previous->Roads.begin(),
                previous->Roads.end(),
                current->Roads.begin(),
                current->Roads.end(),
                std::insert_iterator<UTL::Std::set<short, _type_ID_ROAD_SET> >(roadDiff, roadDiff.begin()));

            for (UTL::Std::set<short, _type_ID_ROAD_SET>::iterator it = roadDiff.begin(); it != roadDiff.end(); ++it) {
                roadDiffLength += static_cast<float>(WRoadNetwork::Get().GetRoad(*it)->nLength) * 0.061036088f;
            }

            if (roadDiffLength > 0.0f) {
                for (UTL::Std::set<short, _type_ID_ROAD_SET>::iterator it = roadDiff.begin(); it != roadDiff.end(); ++it) {
                    WRoad *road = const_cast<WRoad *>(WRoadNetwork::Get().GetRoad(*it));
                    int scale = static_cast<int>((((current->Length - previous->Length) + roadDiffLength) / roadDiffLength) * 65536.0f);

                    road->nScale = static_cast<unsigned short>(scale >> 8);
                }
            }
        }
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
