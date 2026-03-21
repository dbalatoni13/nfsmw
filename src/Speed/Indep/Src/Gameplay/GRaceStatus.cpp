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
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
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
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

#include <algorithm>

void SetCurrentTimeOfDay(float value);
void SetOverRideRainIntensity(float intensity);
bool DoesStringExist(unsigned int hash);
const char *GetLocalizedString(unsigned int hash);
extern int UnlockAllThings;
extern int SkipFE;
extern const char *SkipFEOpponentPresetRide;
unsigned int bStringHashUpper(const char *text);

struct PresetCar;
struct CarPartDatabase;

HINTERFACE IAudible::_IHandle() {
    return (HINTERFACE)_IHandle;
}

extern CarPartDatabase CarPartDB;

FECustomizationRecord *FECustomizationRecordCtor(FECustomizationRecord *self) __asm__("__21FECustomizationRecord");
PresetCar *FindFEPresetCar(unsigned int key) __asm__("FindFEPresetCar__FUi");
void FECustomizationRecordBecomePreset(FECustomizationRecord *self, PresetCar *preset)
    __asm__("BecomePreset__21FECustomizationRecordP9PresetCar");
void FECustomizationRecordWriteRideIntoRecord(FECustomizationRecord *self, const RideInfo *ride)
    __asm__("WriteRideIntoRecord__21FECustomizationRecordPC8RideInfo");
void RideInfoSetRandomParts(RideInfo *self) __asm__("SetRandomParts__8RideInfo");
CarType CarPartDatabaseGetCarType(CarPartDatabase *self, unsigned int key) __asm__("GetCarType__15CarPartDatabaseUi");

#ifndef DECLARE_GAMEPLAY_MINIMAP_CLASS
#define DECLARE_GAMEPLAY_MINIMAP_CLASS
class Minimap {
  public:
    static void ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track);
};
#endif

struct GRaceStatusCompat {
    unsigned char _pad[0x1AB0];
    GRaceBin *mRaceBin;
};

struct GManagerRestartCompat {
    unsigned char _pad[0x304];
    unsigned int mRestartEventHash;
};

template <> struct GObjectIteratorTraits<GTrigger> {
    enum { kType = kGameplayObjType_Trigger };
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

int NotNumeric(char c) {
    if (c == '-' || c == '.') {
        return 0;
    }

    int numeric = '0';
    if (c > '0') {
        numeric = c;
    }
    if (numeric > '9') {
        numeric = '9';
    }

    return c != numeric;
}

int SplitChars(char *in, char ***array, int (*func)(char)) {
    while (*in != '\0' && func(*in) != 0) {
        in += 1;
    }

    int count = 0;
    char *cursor = in;

    while (*cursor != '\0') {
        if (func(*cursor) == 0) {
            count += 1;
            while (*cursor != '\0' && func(*cursor) == 0) {
                cursor += 1;
            }
        } else {
            cursor += 1;
        }
    }

    *array = new char *[count];
    if (count > 0) {
        int i = 0;

        do {
            char *end = in;

            (*array)[i] = in;
            i += 1;
            while (*end != '\0' && func(*end) == 0) {
                end += 1;
            }

            in = end;
            while (*in != '\0' && func(*in) != 0) {
                in += 1;
            }

            *end = '\0';
        } while (i < count);
    }

    return count;
}

float ParseFloat(char *word) {
    float whole = 0.0f;
    unsigned int index = static_cast<unsigned int>(*word == '-');
    bool pastDecimal = false;
    float scale = 1.0f;

    for (char c = word[index]; c != '\0'; c = word[++index]) {
        if (c == '.') {
            pastDecimal = true;
        } else {
            if (pastDecimal) {
                scale *= 0.1f;
            } else {
                whole *= 10.0f;
            }

            int digit = c - '0';

            if (digit < 0) {
                digit = 0;
            }
            if (digit > 9) {
                digit = 9;
            }

            whole = scale * static_cast<float>(digit) + whole;
        }
    }

    return whole;
}

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
    const char *carName;
    const char *carNameLowMem;
    const char *presetRide;
    FECustomizationRecord customizations;
    unsigned int vehicle_key;

    if (!racerChar) {
        return nullptr;
    }

    carName = racerChar->CarType(0);
    carNameLowMem = racerChar->CarTypeLowMem(0);
    presetRide = racerChar->PresetRide(0);
    FECustomizationRecordCtor(&customizations);

    if (SkipFE && bStrLen(SkipFEOpponentPresetRide) > 0) {
        presetRide = SkipFEOpponentPresetRide;
    }

    vehicle_key = 0;
    if (presetRide) {
        PresetCar *preset = FindFEPresetCar(bStringHashUpper(presetRide));

        if (preset) {
            FECustomizationRecordBecomePreset(&customizations, preset);
            vehicle_key = *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(preset) + 0x54);
        }
    }

    if (vehicle_key == 0) {
        if (carName && *carName) {
            vehicle_key = Attrib::StringKey(carName);
        }

        if (vehicle_key == 0) {
            vehicle_key = default_key;
        }
    }

    Attrib::Gen::pvehicle attributes(vehicle_key, 0, nullptr);

    if (!attributes.IsValid()) {
        return nullptr;
    }

    if (!customizations.IsPreset()) {
        RideInfo ride;
        const char *modelName = attributes.MODEL().GetString();

        if (!modelName) {
            modelName = "";
        }

        ride.Init(CarPartDatabaseGetCarType(&CarPartDB, bStringHashUpper(modelName)), CarRenderUsage_AIRacer, 0, 0);
        RideInfoSetRandomParts(&ride);
        FECustomizationRecordWriteRideIntoRecord(&customizations, &ride);
    }

    Physics::Info::Performance ai_performance(1.0f, 1.0f, 1.0f);
    IVehicleCache *cache = nullptr;
    if (GRaceStatus::Exists()) {
        GRaceStatus *raceStatus = &GRaceStatus::Get();

        cache = reinterpret_cast<IVehicleCache *>(reinterpret_cast<unsigned char *>(raceStatus) + 0x10);
    }

    UMath::Vector3 direction = {0.0f, 0.0f, 1.0f};
    VehicleParams params(cache, DRIVER_RACER, vehicle_key, direction, UMath::Vector3::kZero, 0, &customizations, &ai_performance);
    ISimable *result = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance("PVehicle", params);
    if (result) {
        IVehicle *vehicle;

        if (result->QueryInterface(&vehicle)) {
            SetSimable(result);
            return vehicle;
        }
    }

    return nullptr;
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

void GRacerInfo::ChooseRandomName() {
    char nameBuffer[32];
    const char *name;

    if (!DoesStringExist(Attrib::StringHash32("RACERNAME_000"))) {
        mName = "UNKNOWN";
        return;
    }

    do {
        bool duplicate = false;
        int i;
        const int racerCount = GRaceStatus::Get().GetRacerCount();

        bSPrintf(nameBuffer, "RACERNAME_%03d", bRandom(0x96));
        name = GetLocalizedString(bStringHash(nameBuffer));

        for (i = 0; i < racerCount; ++i) {
            GRacerInfo &info = GRaceStatus::Get().GetRacerInfo(i);

            if (info.mName && bStrCmp(name, info.mName) == 0) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            mName = name;
            return;
        }
    } while (true);
}

bool GRacerInfo::ChooseRacerName() {
    unsigned int nameHash;

    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            if ((nameHash = mGameCharacter->GetName()) != 0) {
                mName = GetLocalizedString(nameHash);
                return true;
            }
        }
    }

    return false;
}

bool GRacerInfo::ChooseBossName() {
    char stringBuffer[32];
    GRaceBin *raceBin;

    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            GRaceParameters *raceParameters = GRaceStatus::Get().GetRaceParameters();

            if (raceParameters) {
                if (raceParameters->GetIsBossRace()) {
                    raceBin = reinterpret_cast<GRaceStatusCompat *>(&GRaceStatus::Get())->mRaceBin;
                    bSNPrintf(stringBuffer, 0x20, "BLACKLIST_RIVAL_%02d_LEADERBOARD", raceBin->GetBinNumber());
                    mName = GetLocalizedString(bStringHash(stringBuffer));
                    return true;
                }
            }
        }
    }

    return false;
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
    if (IsFinishedRacing()) {
        if (!rhs.IsFinishedRacing()) {
            return false;
        }

        return GetRaceTime() > rhs.GetRaceTime();
    }

    if (rhs.IsFinishedRacing()) {
        return true;
    }

    if (GetIsKnockedOut() && rhs.GetIsKnockedOut()) {
        return GetRaceTime() < rhs.GetRaceTime();
    }

    if (GetIsEngineBlown() && rhs.GetIsEngineBlown()) {
        return GetRaceTime() < rhs.GetRaceTime();
    }

    if (GetIsTotalled() && rhs.GetIsTotalled()) {
        return GetRaceTime() < rhs.GetRaceTime();
    }

#ifndef EA_BUILD_A124
    if (GetDNF()) {
        if (!rhs.GetDNF()) {
            return GetPctRaceComplete() > rhs.GetPctRaceComplete();
        }
    }
#endif

    if (GetIsKnockedOut() || GetIsEngineBlown() || GetIsTotalled()) {
        return true;
    }

    if (rhs.GetIsKnockedOut() || rhs.GetIsEngineBlown() || rhs.GetIsTotalled()) {
        return false;
    }

    if (mLapsCompleted != rhs.mLapsCompleted) {
        return mLapsCompleted < rhs.mLapsCompleted;
    }

    return GetPctRaceComplete() < rhs.GetPctRaceComplete();
}

bool GRacerInfo::AreStatsReady() const {
    if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_TimeTrial) {
        return true;
    }

    return mFinishedRacing || mEngineBlown || mTotalled;
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

    GRaceStatus &raceStatus = GRaceStatus::Get();

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
    if (speed > mTopSpeed) {
        mTopSpeed = speed;
    }

    distance = mDistanceDriven + speed * dT;
    mTotalUpdateTime += dT;
    mDistanceDriven = distance;

    #ifndef EA_BUILD_A124
    if (mQuarterMileTime == 0.0f) {
        static float quarterMileInMeters = MILE2METERS(0.25f);

        if (distance >= quarterMileInMeters) {
            mQuarterMileTime = GetRaceTime();
        }
    }

    if (mZeroToSixtyTime == 0.0f) {
        static float sixtyMphInMetersPerSec = MPH2MPS(60.0f);

        if (mTopSpeed >= sixtyMphInMetersPerSec) {
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
        float lapDistanceCompleted;
        int checkpointsCompleted;
        float distanceToNextCheckpoint;
        float currentSegmentLength;

        if (lapsCompleted > 0) {
            raceDistanceCompleted = raceStatus.GetFirstLapLength();
        }

        if (lapsCompleted > 1) {
            raceDistanceCompleted += raceStatus.GetSubsequentLapLength() * static_cast<float>(lapsCompleted - 1);
        }

        checkpointsCompleted = GetChecksHitThisLap();
        lapDistanceCompleted = 0.0f;
        for (int i = 0; i < checkpointsCompleted; ++i) {
            lapDistanceCompleted += raceStatus.GetSegmentLength(i, lapsCompleted);
        }

        distanceToNextCheckpoint = GetDistToNextCheck();
        currentSegmentLength = raceStatus.GetSegmentLength(checkpointsCompleted, lapsCompleted);
        if (distanceToNextCheckpoint != 0.0f) {
            float currentDistanceCompleted;
            float lapLength;

            lapDistanceCompleted += currentSegmentLength - distanceToNextCheckpoint;
            raceDistanceCompleted += lapDistanceCompleted;
            currentDistanceCompleted = raceDistanceCompleted;
            lapLength = raceStatus.GetLapLength(lapsCompleted);
            if (lapLength > 0.0f) {
                mPctLapComplete = bClamp(lapDistanceCompleted / lapLength, 0.0f, 1.0f);
            } else {
                mPctLapComplete = 1.0f;
            }

            mPctRaceComplete = bClamp(raceDistanceCompleted / raceLength, 0.0f, 1.0f);
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
    float currentTime;
    float time_now;
    float pctComplete;

    if (mFinishedRacing) {
        return;
    }

    time_now = GetRaceTime();
    pctComplete = GetPctRaceComplete();
    currentTime = time_now;
    if (0.1f < pctComplete) {
        currentTime = currentTime / (pctComplete * 0.01f);
    }

    if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_Drag &&
        (GetIsTotalled() || GetIsEngineBlown() || pctComplete < 1.0f)) {
        currentTime = 0.0f;
#ifndef EA_BUILD_A124
        mDNF = true;
#endif
    }

    if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap && mGameCharacter) {
        GRaceParameters *parameters = GRaceStatus::Get().GetRaceParameters();
        float time_left = currentTime - time_now;
        int penalty = parameters->GetGameplayObj()->OvertimePenaltyPerSec(0);

        if (0.0f < time_left) {
            AddToPointTotal(-(time_left * static_cast<float>(penalty)));
        }
    }

    if (GRaceStatus::Get().GetRaceParameters() &&
        GRaceStatus::Get().GetRaceParameters()->GetIsLoopingRace()) {
        if (mGameCharacter) {
            int totalLaps = GRaceStatus::Get().GetRaceParameters()->GetNumLaps();
            int completedLaps = GetLapsCompleted();
            float elapsedTime = 0.0f;
            int onLap = 0;

            if (onLap < totalLaps) {
                do {
                    float lapTime = GRaceStatus::Get().GetLapTime(onLap, GetIndex(), false);

                    if (lapTime == 0.0f) {
                        completedLaps = onLap;
                        break;
                    }

                    onLap++;
                    elapsedTime += lapTime;
                } while (onLap < totalLaps);
            }

            GRaceStatus::Get().SetLapTime(completedLaps, GetIndex(), currentTime - elapsedTime);
        }
    }

#ifndef EA_BUILD_A124
    if (mGameCharacter) {
        float effectivePct = mDNF ? pctComplete : 1.0f;
        float pct[4];
        int onSplit;

        pct[0] = 0.25f;
        pct[1] = 0.5f;
        pct[2] = 0.75f;
        pct[3] = 1.0f;

        for (onSplit = 0; onSplit <= 3; ++onSplit) {
            bool wasAliveAtPct = effectivePct >= pct[onSplit];

            if (mSplitTimes[onSplit] == 0.0f) {
                if (wasAliveAtPct) {
                    mSplitTimes[onSplit] = currentTime * pct[onSplit];
                } else {
                    mSplitTimes[onSplit] = 0.0f;
                }
            }

            if (mSplitRankings[onSplit] == 0) {
                mSplitRankings[onSplit] = mRanking;
            }
        }
    }

    if (!mDNF) {
        mRaceTimer.SetTime(currentTime);
        FinishRace();
    }
#else
    mRaceTimer.SetTime(currentTime);
    FinishRace();
#endif
}

GRaceParameters::GRaceParameters(unsigned int collectionKey, GRaceIndexData *index)
    : mIndex(nullptr), //
      mRaceRecord(new Attrib::Gen::gameplay(collectionKey, 0, nullptr)), //
      mParentVault(nullptr), //
      mChildVault(nullptr) {
    const char *childVaultName;

    GenerateIndex(index);
    mIndex = index;

    childVaultName = mRaceRecord->gameplayvault(0);
    if (childVaultName) {
        mChildVault = GManager::Get().FindVault(childVaultName);
        mRaceRecord->Num_Children();
    }

    mParentVault = GManager::Get().FindVaultContaining(mRaceRecord->GetCollection());
    if (mParentVault) {
        mParentVault->IsTransient();
    }
}

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
    if (!mRaceRecord) {
        mRaceRecord = new Attrib::Gen::gameplay(reinterpret_cast<unsigned int *>(mIndex)[0], 0, nullptr);
    }
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
    unsigned int collectionKey;

    if (!mIndex) {
        EnsureLoaded();
        collectionKey = mRaceRecord->GetCollection();
    } else {
        collectionKey = reinterpret_cast<const unsigned int *>(mIndex)[0];
    }

    return collectionKey;
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
    if (mIndex) {
        return *reinterpret_cast<const float *>(reinterpret_cast<const char *>(mIndex) + 0x1C);
    }

    const float *raceLength;

    EnsureLoaded();
    raceLength = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x7C11C52E, 0));
    if (!raceLength) {
        raceLength = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *raceLength;
}

int GRaceParameters::GetReputation() const {
    if (mIndex) {
        unsigned short value = *reinterpret_cast<const unsigned short *>(reinterpret_cast<const char *>(mIndex) + 0x24);
        int exponent = static_cast<int>(static_cast<short>(value)) >> 11;
        unsigned int scale = 1;

        if (exponent < 0) {
            exponent = -exponent;
        }

        while (true) {
            bool done = exponent < 1;

            exponent = exponent - 1;
            if (done) {
                break;
            }

            scale = scale * 10;
        }

        if ((value & 0x8000) != 0) {
            return static_cast<int>(static_cast<float>(static_cast<short>(value << 5) >> 5) /
                                    static_cast<float>(scale));
        }

        return static_cast<int>(static_cast<float>(static_cast<short>(value << 5) >> 5) *
                                static_cast<float>(scale));
    }

    const int *reputation;

    EnsureLoaded();
    reputation = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0x477EC5AA, 0));
    if (!reputation) {
        reputation = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    return *reputation;
}

float GRaceParameters::GetCashValue() const {
    if (mIndex) {
        unsigned short value = *reinterpret_cast<const unsigned short *>(reinterpret_cast<const char *>(mIndex) + 0x22);
        int exponent = static_cast<int>(static_cast<short>(value)) >> 11;
        unsigned int scale = 1;

        if (exponent < 0) {
            exponent = -exponent;
        }

        while (true) {
            bool done = exponent < 1;

            exponent = exponent - 1;
            if (done) {
                break;
            }

            scale = scale * 10;
        }

        if ((value & 0x8000) != 0) {
            return static_cast<float>(static_cast<short>(value << 5) >> 5) / static_cast<float>(scale);
        }

        return static_cast<float>(static_cast<short>(value << 5) >> 5) * static_cast<float>(scale);
    }

    const float *cashValue;

    EnsureLoaded();
    cashValue = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0xD8BAA07B, 0));
    if (!cashValue) {
        cashValue = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *cashValue;
}

int GRaceParameters::GetLocalizationTag() const {
    if (mIndex) {
        return *reinterpret_cast<const short *>(reinterpret_cast<const char *>(mIndex) + 0x20);
    }

    const int *localizationTag;

    EnsureLoaded();
    localizationTag = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0xDB89AB5C, 0));
    if (!localizationTag) {
        localizationTag = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    return *localizationTag;
}

int GRaceParameters::GetNumLaps() const {
    if (mIndex) {
        return reinterpret_cast<const unsigned char *>(mIndex)[0x28];
    }

    const int *numLaps;

    EnsureLoaded();
    numLaps = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0x0EBDC165, 0));
    if (!numLaps) {
        numLaps = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    return *numLaps;
}

const char *GRaceParameters::GetEventID() const {
    if (mIndex) {
        return reinterpret_cast<const char *>(mIndex) + 4;
    }

    const EA::Reflection::Text *eventID;

    EnsureLoaded();
    eventID = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0xA78403EC, 0));
    if (!eventID) {
        eventID = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    return *eventID;
}

float GRaceParameters::GetRivalBestTime() const {
    const float *rivalBestTime;

    if (mIndex) {
        return static_cast<float>(*reinterpret_cast<const unsigned short *>(reinterpret_cast<const char *>(mIndex) + 0x26)) /
               static_cast<float>(FixedPoint<unsigned short, 10, 2>::GetScale());
    }

    EnsureLoaded();
    rivalBestTime = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0xF9120D73, 0));
    if (!rivalBestTime) {
        rivalBestTime = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *rivalBestTime;
}

float GRaceParameters::GetChallengeGoal() const {
    const float *challengeGoal;

    if (mIndex) {
        short recordValue = *reinterpret_cast<const short *>(reinterpret_cast<const char *>(mIndex) + 0x0E);
        int exponent = recordValue >> 11;
        int multiplier = 1;
        int mantissa;

        if (exponent < 0) {
            exponent = -exponent;
        }

        while (exponent > 0) {
            exponent--;
            multiplier *= 10;
        }

        mantissa = recordValue << 21 >> 21;
        if (recordValue & 0x8000) {
            return static_cast<float>(mantissa) / static_cast<float>(multiplier);
        }

        return static_cast<float>(mantissa) * static_cast<float>(multiplier);
    }

    EnsureLoaded();
    challengeGoal = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x4E90219D, 0));
    if (!challengeGoal) {
        challengeGoal = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *challengeGoal;
}

bool GRaceParameters::GetIsPursuitRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 7)) != 0;
    }

    const bool *isPursuitRace;

    EnsureLoaded();
    isPursuitRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x2B1F54F6, 0));
    if (!isPursuitRace) {
        isPursuitRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isPursuitRace;
}

bool GRaceParameters::GetIsLoopingRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 8)) != 0;
    }

    EnsureLoaded();
    return mRaceRecord->IsLoopingRace(0);
}

bool GRaceParameters::GetInitiallyUnlockedQuickRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 0)) != 0;
    }

    const bool *initiallyUnlockedQuickRace;

    EnsureLoaded();
    initiallyUnlockedQuickRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xB39ED8C3, 0));
    if (!initiallyUnlockedQuickRace) {
        initiallyUnlockedQuickRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *initiallyUnlockedQuickRace;
}

bool GRaceParameters::GetInitiallyUnlockedOnline() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 1)) != 0;
    }

    const bool *initiallyUnlockedOnline;

    EnsureLoaded();
    initiallyUnlockedOnline = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x39509746, 0));
    if (!initiallyUnlockedOnline) {
        initiallyUnlockedOnline = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *initiallyUnlockedOnline;
}

bool GRaceParameters::GetInitiallyUnlockedChallenge() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 2)) != 0;
    }

    const bool *initiallyUnlockedChallenge;

    EnsureLoaded();
    initiallyUnlockedChallenge = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xEA855EAF, 0));
    if (!initiallyUnlockedChallenge) {
        initiallyUnlockedChallenge = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *initiallyUnlockedChallenge;
}

bool GRaceParameters::GetIsDDayRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 4)) != 0;
    }

    const bool *isDDayRace;

    EnsureLoaded();
    isDDayRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x8CB01ABF, 0));
    if (!isDDayRace) {
        isDDayRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isDDayRace;
}

bool GRaceParameters::GetIsBossRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 5)) != 0;
    }

    const bool *isBossRace;

    EnsureLoaded();
    isBossRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xFF5EE5D6, 0));
    if (!isBossRace) {
        isBossRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isBossRace;
}

bool GRaceParameters::GetIsMarkerRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 6)) != 0;
    }

    const bool *isMarkerRace;

    EnsureLoaded();
    isMarkerRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xF2FE50D7, 0));
    if (!isMarkerRace) {
        isMarkerRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isMarkerRace;
}

bool GRaceParameters::GetRankPlayersByPoints() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 9)) != 0;
    }

    EnsureLoaded();
    return mRaceRecord->RankPlayersByPoints(0);
}

bool GRaceParameters::GetRankPlayersByDistance() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 10)) != 0;
    }

    EnsureLoaded();
    return mRaceRecord->RankPlayersByDistance(0);
}

bool GRaceParameters::GetScriptedCopsInRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 12)) != 0;
    }

    EnsureLoaded();
    return mRaceRecord->ScriptedCopsInRace(0);
}

bool GRaceParameters::GetCopsEnabled() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 11)) != 0;
    }

    const bool *copsEnabled;

    EnsureLoaded();
    copsEnabled = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x3918E889, 0));
    if (!copsEnabled) {
        copsEnabled = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *copsEnabled;
}

bool GRaceParameters::GetNeverInQuickRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 14)) != 0;
    }

    EnsureLoaded();
    return mRaceRecord->NeverInQuickRace(0);
}

bool GRaceParameters::GetIsChallengeSeriesRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 15)) != 0;
    }

    const bool *isChallengeSeriesRace;

    EnsureLoaded();
    isChallengeSeriesRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x1C650104, 0));
    if (!isChallengeSeriesRace) {
        isChallengeSeriesRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isChallengeSeriesRace;
}

bool GRaceParameters::GetIsCollectorsEditionRace() const {
    if (mIndex) {
        return (reinterpret_cast<const unsigned int *>(mIndex)[6] & (1 << 16)) != 0;
    }

    const bool *isCollectorsEditionRace;

    EnsureLoaded();
    isCollectorsEditionRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x637584FE, 0));
    if (!isCollectorsEditionRace) {
        isCollectorsEditionRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isCollectorsEditionRace;
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
    const bool *useWorldHeatInRace;

    EnsureLoaded();
    useWorldHeatInRace = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0x45F2AD6C, 0));
    if (!useWorldHeatInRace) {
        useWorldHeatInRace = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *useWorldHeatInRace;
}

float GRaceParameters::GetForceHeatLevel() const {
    EnsureLoaded();
    return static_cast<float>(mRaceRecord->ForceHeatLevel(0));
}

float GRaceParameters::GetMaxRaceHeatLevel() const {
    const float *maxRaceHeatLevel;

    EnsureLoaded();
    maxRaceHeatLevel = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0xF5A03629, 0));
    if (!maxRaceHeatLevel) {
        maxRaceHeatLevel = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *maxRaceHeatLevel;
}

float GRaceParameters::GetInitialPlayerSpeed() const {
    EnsureLoaded();
    return mRaceRecord->InitialPlayerSpeed(0);
}

bool GRaceParameters::GetIsRollingStart() const {
    const bool *isRollingStart;

    EnsureLoaded();
    isRollingStart = reinterpret_cast<const bool *>(mRaceRecord->GetAttributePointer(0xB809D19C, 0));
    if (!isRollingStart) {
        isRollingStart = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
    }

    return *isRollingStart;
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
    const EA::Reflection::Text *photoFinishCamera;

    EnsureLoaded();
    photoFinishCamera = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x62DFC259, 0));
    if (!photoFinishCamera) {
        photoFinishCamera = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    return *photoFinishCamera;
}

const char *GRaceParameters::GetPhotoFinishTexture() const {
    const EA::Reflection::Text *photoFinishTexture;

    EnsureLoaded();
    photoFinishTexture = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x038A3B53, 0));
    if (!photoFinishTexture) {
        photoFinishTexture = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    return *photoFinishTexture;
}

const char *GRaceParameters::GetTrafficPattern() const {
    const EA::Reflection::Text *trafficPattern;

    EnsureLoaded();
    trafficPattern = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x6319B692, 0));
    if (!trafficPattern) {
        trafficPattern = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    return *trafficPattern;
}

float GRaceParameters::GetTimeOfDay() const {
    const float *timeOfDay;

    EnsureLoaded();
    timeOfDay = reinterpret_cast<const float *>(mRaceRecord->GetAttributePointer(0x9DFF3C3D, 0));
    if (!timeOfDay) {
        timeOfDay = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    return *timeOfDay;
}

unsigned int GRaceParameters::GetChallengeType() const {
    const EA::Reflection::Text *challengeType;

    if (mIndex) {
        return *reinterpret_cast<const unsigned int *>(reinterpret_cast<const char *>(mIndex) + 0x10);
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    challengeType = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x704F72E8, 0));
    if (!challengeType) {
        challengeType = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    if (!*challengeType || !**challengeType) {
        return 0;
    }

    return Attrib::StringHash32(*challengeType);
}

GRace::Type GRaceParameters::GetRaceType() const {
    static const struct {
        const char *mTypeName;
        GRace::Type mType;
    } typeTable[11] = {
        {"circuit", GRace::kRaceType_Circuit},
        {"p2p", GRace::kRaceType_P2P},
        {"drag", GRace::kRaceType_Drag},
        {"knockout", GRace::kRaceType_Knockout},
        {"tollbooth", GRace::kRaceType_Tollbooth},
        {"speedtrap", GRace::kRaceType_SpeedTrap},
        {"cashgrab", GRace::kRaceType_CashGrab},
        {"checkpointrace", GRace::kRaceType_Checkpoint},
        {"challenge", GRace::kRaceType_Challenge},
        {"speedtrapjump", GRace::kRaceType_JumpToSpeedTrap},
        {"milestonejump", GRace::kRaceType_JumpToMilestone},
    };
    const char *eventType;

    if (mIndex) {
        return static_cast<GRace::Type>(static_cast<signed char>(reinterpret_cast<const char *>(mIndex)[0x2B]));
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    {
        const EA::Reflection::Text *eventTypePtr =
            reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0x0F6BCDE1, 0));

        if (!eventTypePtr) {
            eventTypePtr = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
        }

        eventType = *eventTypePtr;
    }

    {
        int onType;

        for (onType = 0; onType < 11; ++onType) {
            if (bStrCmp(eventType, typeTable[onType].mTypeName) == 0) {
                return typeTable[onType].mType;
            }
        }
    }

    return GRace::kRaceType_None;
}

unsigned int GRaceParameters::GetRegion() const {
    static const struct {
        const char *mName;
        unsigned int mRegion;
    } kRaceRegions[3] = {
        {"city", 0},
        {"coastal", 1},
        {"college_town", 2},
    };
    unsigned int i;
    const EA::Reflection::Text *regionName;

    if (mIndex) {
        return reinterpret_cast<const unsigned char *>(mIndex)[0x29];
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    regionName = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0xCB01E454, 0));
    if (!regionName) {
        regionName = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    for (i = 0; i < 3; i++) {
        if (bStrCmp(*regionName, kRaceRegions[i].mName) == 0) {
            return kRaceRegions[i].mRegion;
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
    UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
    UMath::Vector3 forward = {0.0f, 0.0f, 1.0f};
    const float *rotation = reinterpret_cast<const float *>(collection.GetAttributePointer(0x5A6A57C6, 0));

    if (!rotation) {
        rotation = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    MATRIX4_multyrot(&rotMat, -(*rotation + rotate) * 0.0027777778f, &rotMat);
    VU0_MATRIX3x4_vect3mult(forward, rotMat, forward);

    dir.x = forward.x;
    dir.y = forward.y;
    dir.z = forward.z;
}

unsigned int GRaceParameters::GetEventHash() const {
    const EA::Reflection::Text *eventID;

    if (mIndex) {
        return *reinterpret_cast<const unsigned int *>(reinterpret_cast<const char *>(mIndex) + 0x14);
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    eventID = reinterpret_cast<const EA::Reflection::Text *>(mRaceRecord->GetAttributePointer(0xA78403EC, 0));
    if (!eventID) {
        eventID = reinterpret_cast<const EA::Reflection::Text *>(Attrib::DefaultDataArea(sizeof(EA::Reflection::Text)));
    }

    return Attrib::StringHash32(*eventID);
}

bool GRaceParameters::GetIsAvailable(GRace::Context context) const {
    if (UnlockAllThings) {
        return true;
    }

    switch (context) {
    case GRace::kRaceContext_QuickRace:
        return !GetNeverInQuickRace() &&
               GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_QuickRace);

    case GRace::kRaceContext_TimeTrial:
        return GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_Online);

    case GRace::kRaceContext_Career:
        if (!GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kUnlocked_Career)) {
            return false;
        }
        return !GRaceDatabase::Get().CheckRaceScoreFlags(GetEventHash(), GRaceDatabase::kCompleted_ContextCareer);
    }

    return false;
}

int GRaceParameters::GetTrafficDensity() const {
    const int *trafficDensity;

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    trafficDensity = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0xC64BC341, 0));
    if (!trafficDensity) {
        trafficDensity = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    return *trafficDensity;
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
    const int *difficulty;
    GRace::Difficulty raceDifficulty;

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    difficulty = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0x88A7E3BE, 0));
    if (!difficulty) {
        difficulty = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    if (*difficulty < 0x22) {
        return GRace::kRaceDifficulty_Easy;
    }

    raceDifficulty = GRace::kRaceDifficulty_Medium;
    if (*difficulty > 0x42) {
        raceDifficulty = GRace::kRaceDifficulty_Hard;
    }

    return raceDifficulty;
}

int GRaceParameters::GetCopDensity() const {
    const int *copDensity;
    int raceCopDensity;
    int density;

    if (mIndex) {
        return static_cast<signed char>(reinterpret_cast<const char *>(mIndex)[0x2A]);
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

    copDensity = reinterpret_cast<const int *>(mRaceRecord->GetAttributePointer(0xDBC08D32, 0));
    if (!copDensity) {
        copDensity = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    density = *copDensity;
    if (density == 0 && !GetCopsEnabled()) {
        return 0;
    }
    if (density < 0x22) {
        return 1;
    }

    raceCopDensity = 2;
    if (density > 0x42) {
        raceCopDensity = 3;
    }

    return raceCopDensity;
}

bool GRaceParameters::GetCanBeReversed() const {
    if (mIndex) {
        return (*reinterpret_cast<const unsigned int *>(reinterpret_cast<const char *>(mIndex) + 0x18) >> 3) & 1;
    }

    if (mParentVault && !mParentVault->IsLoaded()) {
        mParentVault->LoadSyncTransient();
    }

    if (mChildVault && !mChildVault->IsLoaded()) {
        mChildVault->LoadSyncTransient();
    }

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

int ParseArray(const char *string, float *data, int max) {
    int len = bStrLen(string);
    char *copy = new (__FILE__, __LINE__) char[len + 1];
    char **words;
    int num;
    float *temp = nullptr;
    int i;

    bMemCpy(copy, string, len + 1);
    num = SplitChars(copy, &words, NotNumeric);
    if (num > max) {
        temp = new (__FILE__, __LINE__) float[num];
        i = 0;
        if (num > 0) {
            do {
                temp[i] = ParseFloat(words[i]);
                i += 1;
            } while (i < num);
        }

        {
            Table table(temp, num, 0.0f, static_cast<float>(max - 1));

            i = 0;
            if (max > 0) {
                do {
                    data[i] = table.GetValue(static_cast<float>(i));
                    i += 1;
                } while (i < max);
            }
        }
    } else {
        i = 0;
        if (num > 0) {
            do {
                data[i] = ParseFloat(words[i]);
                i += 1;
            } while (i < num);
        }

        max = num;
        if (num == 1) {
            max = 2;
            data[1] = *data;
        }
    }

    if (temp) {
        delete[] temp;
    }
    if (words) {
        delete[] words;
    }
    if (copy) {
        delete[] copy;
    }

    return max;
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
    int existingOpponents;
    bool bossINQuickRace;

    if (mReversed) {
        if (GetCanBeReversed()) {
            GCollectionKey startReverse = mRaceRecord->racestartReverse(0);
            GCollectionKey finishReverse = mRaceRecord->racefinishReverse(0);

            mRaceRecord->Set_racestart(startReverse);
            mRaceRecord->Set_racefinish(finishReverse);
        } else {
            mReversed = false;
        }
    }

    existingOpponents = GetNumOpponents();
    bossINQuickRace = false;
    if (GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_Career && GetIsBossRace()) {
        bossINQuickRace = true;
        existingOpponents = 0;
    }

    if (mNumOpponents != static_cast<unsigned int>(existingOpponents)) {
        unsigned int opponentKey[16];
        unsigned int currOpponents = 0;

        if (!bossINQuickRace) {
            Attrib::Attribute attribute(mRaceRecord->Get(0x5839FA1A));

            currOpponents = attribute.GetLength();
            if (currOpponents != 0) {
                for (unsigned int onOpp = 0; onOpp < currOpponents; onOpp++) {
                    opponentKey[onOpp] = mRaceRecord->Opponents(onOpp).GetCollectionKey();
                }
            }
        }

        if (currOpponents < mNumOpponents) {
            if (currOpponents == 0) {
                for (unsigned int onSet = 0; onSet < mNumOpponents; onSet++) {
                    opponentKey[onSet] = 0x9F3B88C5;
                }
            } else {
                for (unsigned int onCopy = currOpponents; onCopy < mNumOpponents; onCopy++) {
                    opponentKey[onCopy] = opponentKey[onCopy % currOpponents];
                }
            }
        }

        mRaceRecord->Add(0x5839FA1A, mNumOpponents);
        {
            Attrib::Attribute attribute(mRaceRecord->Get(0x5839FA1A));

            if (mNumOpponents != 0) {
                for (unsigned int onSet = 0; onSet < mNumOpponents; onSet++) {
                    attribute.Set(onSet, GCollectionKey(opponentKey[onSet]));
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
      IVehicleCache((UTL::COM::Object *)this) {
    unsigned char currentBin;

    for (int i = 0; i < 16; ++i) {
        mRacerInfo[i].ClearAll();
    }

    currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();

    mNextCheckpoint = nullptr;
    mCheckpointModel = nullptr;
    mCheckpointEmitter = nullptr;
    mIsLoading = false;
    mActivelyRacing = false;
    mRaceParms = nullptr;
    mRacerCount = 0;
    nSpeedTraps = 0;
    mRaceContext = GRace::kRaceContext_Career;
    mRaceBin = GRaceDatabase::Get().GetBinNumber(currentBin);
    mCaluclatedAdaptiveGain = false;
    mQueueBinChange = false;
    mNumTollbooths = 0;
#ifndef EA_BUILD_A124
    mPlayerPursuitInCooldown = false;
#endif
    mBonusTime = 0.0f;
    mTaskTime = 0.0f;
    mSuddenDeathMode = false;
    mTimeExpiredMsgSent = false;
    mLastSecondTickSent = 0;
#ifndef EA_BUILD_A124
    mRefreshBinAfterRace = false;
    mWarpWhenInFreeRoam = 0;
#endif
    mVehicleCacheLocked = false;
    bRaceRouteError = false;
    mTrafficDensity = 0;
    mTrafficPattern = 0;
    mScriptWaitingForLoad = false;
    mHasBeenWon = false;
    fSubsequentLapLength = 0.0f;
    mPlayMode = kPlayMode_Racing;
    fRaceLength = 0.0f;
    fFirstLapLength = 0.0f;
    fObj = this;

    ClearTimes();
    SyncronizeAdaptiveBonus();
    MakeDefaultCatchUpData();

    if (!GRaceDatabase::Get().GetStartupRace()) {
        EnterBin(currentBin);
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
    if (mPlayMode != kPlayMode_Racing && !mVehicleCacheLocked) {
        if (!UTL::COM::ComparePtr(whosasking, ICopMgr::Get())) {
            if (!UTL::COM::ComparePtr(whosasking, ITrafficMgr::Get())) {
                return VCR_DONTCARE;
            }
        }

        if (!CanUnspawnRoamer(removethis)) {
            return VCR_WANT;
        }
    } else {
        for (int onRacer = 0; onRacer < GetRacerCount(); ++onRacer) {
            const GRacerInfo &racerInfo = mRacerInfo[onRacer];

            if (UTL::COM::ComparePtr(racerInfo.GetSimable(), removethis)) {
                return VCR_WANT;
            }
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
    IPlayer *player;

    mPlayMode = kPlayMode_Racing;
    ClearTimes();

    player = IPlayer::First(PLAYER_ALL);
    while (player) {
        if (player->InGameBreaker()) {
            player->ResetGameBreaker(true);
        }

        {
            const IPlayer *currentPlayer = player;
            IPlayer::List::const_iterator iter =
                std::find(IPlayer::GetList(PLAYER_ALL).begin(), IPlayer::GetList(PLAYER_ALL).end(), currentPlayer);
            IPlayer::List::const_iterator end = IPlayer::GetList(PLAYER_ALL).end();

            if (iter == end) {
                player = nullptr;
            } else {
                ++iter;
                player = iter == end ? nullptr : *iter;
            }
        }
    }

    mNumTollbooths = 0;

    {
        GObjectIterator<GTrigger> iter(0x800);

        while (iter.IsValid()) {
            if (iter.GetInstance()->GetTriggerEnabled()) {
                mNumTollbooths++;
            }

            iter.Advance();
        }
    }

    if ((!mRaceParms || !mRaceParms->GetIsDDayRace() || bStrCmp(mRaceParms->GetEventID(), "16.1.0") == 0) &&
        !FEDatabase->IsFinalEpicChase()) {
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
    bool lastDDay = false;
    IPlayer *player;
    bool dDay = false;

    if (mRaceParms) {
        lastDDay = bStrCmp(mRaceParms->GetEventID(), "16.2.1") == 0;
        const Attrib::Gen::gameplay *gameplayObj = mRaceParms->GetGameplayObj();
        const unsigned int *startNewGame =
            reinterpret_cast<const unsigned int *>(gameplayObj->GetAttributePointer(0x64273C71, 0));

        if (!startNewGame) {
            startNewGame = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
        }

        if (!*startNewGame) {
            g_pEAXSound->StartNewGamePlay();
        }
    } else {
        g_pEAXSound->StartNewGamePlay();
    }

    mPlayMode = kPlayMode_Roaming;
    SetRaceContext(GRace::kRaceContext_Career);
    mIsLoading = false;
    mRaceParms = nullptr;
    WRoadNetwork::Get().ResetShortcuts();

    player = IPlayer::First(PLAYER_ALL);
    while (player) {
        ISimable *simable;
        IVehicle *vehicle;
        IVehicleAI *vehicleAI;

        if (player->InGameBreaker()) {
            player->ResetGameBreaker(true);
        }

        simable = player->GetSimable();
        if (simable && simable->QueryInterface(&vehicle)) {
            vehicle->ForceStopOff(vehicle->GetForceStop());

            if (vehicle->QueryInterface(&vehicleAI) && !vehicleAI->GetPursuit()) {
                ICopMgr *copMgr = ICopMgr::Get();

                if (copMgr) {
                    copMgr->ResetCopsForRestart(true);
                }
            }
        }

        {
            IPlayer::List::const_iterator iter =
                std::find(IPlayer::GetList(PLAYER_ALL).begin(), IPlayer::GetList(PLAYER_ALL).end(), player);
            IPlayer::List::const_iterator end = IPlayer::GetList(PLAYER_ALL).end();

            if (iter == end) {
                player = nullptr;
            } else {
                ++iter;
                player = iter == end ? nullptr : *iter;
            }
        }
    }

    if (!lastDDay && reinterpret_cast<GManagerRestartCompat *>(&GManager::Get())->mRestartEventHash == 0) {
        new EReloadHud();
    }

    new EAutoSave();
    if (mRaceParms) {
        dDay = mRaceParms->GetIsDDayRace();
    }

    if (!dDay) {
        SetOverRideRainIntensity(0.0f);
    }

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

    if (GetPlayMode() == kPlayMode_Racing) {
        int numRacers = mRacerCount;

        if (numRacers > 0) {
            int numAiRacers = 0;
            float floatRacers = 0.0f;
            float percentComplete = 0.0f;
            float elapsed;
            int elapsedSec;

            for (int idx = 0; idx < numRacers; ++idx) {
                GRacerInfo &info = GetRacerInfo(idx);

                if (!info.GetIsHuman()) {
                    ++numAiRacers;
                }
            }

            for (int idx = 0; idx < numRacers; ++idx) {
                GRacerInfo &info = GetRacerInfo(idx);

                info.Update(dT);
                ISimable *simable = info.GetSimable();

                if (simable) {
                    float weight = 1.0f;

                    if (info.GetIsHuman()) {
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

            UCrc32 gameplayMessage(0x20D60DBF);
            elapsed = GetRaceTimeElapsed();
            bool isTimeLimited;

            if (mRaceParms) {
                isTimeLimited = mRaceParms->GetTimeLimit() > 0.0f;
            } else {
                isTimeLimited = false;
            }
            float timeRemaining = GetRaceTimeRemaining();

            MNotifyRaceTime(elapsed, isTimeLimited, timeRemaining).Post(gameplayMessage);

            elapsed = GetRaceTimeElapsed();
            elapsedSec = static_cast<int>(elapsed);
            if (elapsedSec > mLastSecondTickSent) {
                mLastSecondTickSent = elapsedSec;
                MNotifyRaceTimeSecTick(elapsed).Post(gameplayMessage);
            }

            if (GetIsTimeLimited()) {
                if (IsChallengeRace()) {
#ifndef EA_BUILD_A124
                    if (mPlayerPursuitInCooldown == 1) {
                        if (mRaceMasterTimer.IsRunning()) {
                            mRaceMasterTimer.Stop();
                        }
                    } else if (!mRaceMasterTimer.IsRunning()) {
                        mRaceMasterTimer.Start();
                    }
#endif
                }

                if (!mTimeExpiredMsgSent && GetRaceTimeRemaining() <= 0.0f) {
                    MNotifyRaceTimeExpired().Post(gameplayMessage);
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
    GRacerInfo &info = mRacerInfo[mRacerCount];
    const char *name;

    ++mRacerCount;

    info.mhSimable = nullptr;
    info.mGameCharacter = nullptr;
    info.mName = nullptr;
    info.mIndex = -1;
    info.mSavedHeatLevel = 0.0f;
    info.mSavedSpeed = 0.0f;
    info.mSavedPosition = UMath::Vector3::kZero;
    info.mSavedDirection = UMath::Vector3::kZero;
    info.mRanking = 0;
    info.mAiRanking = 0;
    info.mKnockedOut = false;
    info.mTotalled = false;
    info.mEngineBlown = false;
    info.mBusted = false;
    info.mChallengeComplete = false;
    info.mFinishedRacing = false;
    info.mCameraDetached = false;
    info.mLapsCompleted = 0;
    info.mPctRaceComplete = 0.0f;
    info.mPctLapComplete = 0.0f;
    info.mCheckpointsHitThisLap = 0;
    info.mDistToNextCheckpoint = 0.0f;
    info.mDistanceDriven = 0.0f;
    info.mTopSpeed = 0.0f;
    info.mFinishingSpeed = 0.0f;
    info.mPoundsNOSUsed = 0.0f;
    info.mTimeCrossedLastCheck = 0.0f;
    info.mTotalUpdateTime = 0.0f;
    info.mPointTotal = 0.0f;
    info.mZeroToSixtyTime = 0.0f;
    info.mQuarterMileTime = 0.0f;
    info.mSpeedBreakerTime = 0.0f;
#ifndef EA_BUILD_A124
    info.mDNF = false;
#endif
    info.mTollboothsCrossed = 0;
    info.mSpeedTrapsCrossed = 0;
    info.mNumPerfectShifts = 0;
    info.mNumTrafficCarsHit = 0;

    info.mRaceTimer.Stop();
    info.mRaceTimer.Reset(0.0f);
    info.mLapTimer.Stop();
    info.mLapTimer.Reset(0.0f);
    info.mCheckTimer.Stop();
    info.mCheckTimer.Reset(0.0f);

    for (int i = 0; i < 16; ++i) {
        info.mTimeRemainingToBooth[i] = 0.0f;
    }

    for (int i = 0; i < 16; ++i) {
        info.mSpeedTrapSpeed[i] = 0.0f;
        info.mSpeedTrapPosition[i] = -1;
    }

#ifndef EA_BUILD_A124
    for (int i = 0; i < 4; ++i) {
        info.mSplitTimes[i] = 0.0f;
        info.mSplitRankings[i] = 0;
    }
#endif

    info.SetSimable(isim);
    info.SetIndex(mRacerCount - 1);

    if (Sim::GetUserMode() == 1) {
        if (mRacerCount == 1) {
            name = GetLocalizedString(0x7B070984);
        } else {
            name = GetLocalizedString(0x7B070985);
        }
    } else {
        UserProfile *userProfile = FEDatabase->CurrentUserProfiles[isim->GetPlayer()->GetSettingsIndex()];

        if (!userProfile) {
            name = GetLocalizedString(0xF760EABE);
            info.SetName(name);
            return info;
        }

        name = userProfile->GetProfileName();
    }

    info.SetName(name);
    return info;
}

inline void GRacerInfo::ClearAll() {
    ClearRaceStats();
}

void GRaceStatus::AddRacer(GRuntimeInstance *racer) {
    GRacerInfo &info = mRacerInfo[mRacerCount];

    ++mRacerCount;
    info.ClearAll();

    info.mGameCharacter = static_cast<GCharacter *>(racer);
    info.SetIndex(mRacerCount - 1);

    if (!info.ChooseBossName() && !info.ChooseRacerName()) {
        info.ChooseRandomName();
    }
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
    for (int i = 0; i < mRacerCount; ++i) {
        GRacerInfo &info = mRacerInfo[i];

        info.mTimeRemainingToBooth[info.mTollboothsCrossed] = GRaceStatus::Get().GetRaceTimeRemaining();
        info.mTollboothsCrossed++;
    }

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
    GTrigger *gtrigger = static_cast<GTrigger *>(trigger);
    bool checkpointVisible = false;

    mNextCheckpoint = gtrigger;

    if (mRaceParms) {
        checkpointVisible = mRaceParms->GetCheckpointsVisible();
    }

    if (gtrigger && checkpointVisible) {
        UMath::Vector3 triggerPos;
        bVector3 triggerPosSwiz;
        bMatrix4 mat;

        if (!mCheckpointModel) {
            mCheckpointModel = new WorldModel(0x738B1F9B, nullptr, false);
        }

        gtrigger->GetPosition(triggerPos);
        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(triggerPos), triggerPosSwiz);
        bIdentity(&mat);
        bCopy(&mat.v3, &triggerPosSwiz, 1.0f);
        mCheckpointModel->SetMatrix(&mat);

        if (mCheckpointEmitter) {
            mCheckpointEmitter->SetLocalWorld(&mat);
        }
    } else {
        if (mCheckpointModel) {
            delete mCheckpointModel;
        }
        mCheckpointModel = nullptr;

        if (mCheckpointEmitter) {
            mCheckpointEmitter->UnSubscribe();
            delete mCheckpointEmitter;
            mCheckpointEmitter = nullptr;
        }
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
    int num_racers;
    bool update;
    GRacerInfo *winning_player;
    GRacerInfo *winning_ai;
    GRacerInfo *eliminated_player;
    float difficulty;

    if (mCaluclatedAdaptiveGain || GetRaceContext() != GRace::kRaceContext_Career || GetRacerCount() <= 1 || Sim::GetUserMode() != 0 || GetPlayMode() != kPlayMode_Racing ||
        (mRaceParms && mRaceParms->GetNoPostRaceScreen()) || GetRaceLength() <= 0.0f) {
        return;
    }

    difficulty = fCatchUpAdaptiveBonus;
    num_racers = GetRacerCount();
    update = false;
    winning_player = nullptr;
    winning_ai = nullptr;
    eliminated_player = nullptr;

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

    if (static_cast<unsigned int>(reason) < 2) {
        if (!who || !who->IsPlayer()) {
            return;
        }

        float percent_ai_complete = 0.0f;
        float percent_human_complete = 0.0f;

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

        if (percent_ai_complete > percent_human_complete && percent_human_complete > 0.0f) {
            float lose_margin = ((percent_ai_complete - percent_human_complete) * (GetRaceLength() * 0.01f)) / 300.0f;
            float t = UMath::Ramp(lose_margin, 0.0f, 1.0f);
            float bonus = UMath::Lerp(0.0f, -0.4f, t);

            difficulty = bClamp(difficulty + ((bonus * percent_human_complete) * 0.01f), -1.0f, 1.0f);
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

        if (percent_ai_complete > percent_human_complete && percent_human_complete > 0.0f) {
            float lose_margin = ((percent_ai_complete - percent_human_complete) * (GetRaceLength() * 0.01f)) / 300.0f;
            float t = UMath::Ramp(lose_margin, 0.0f, 1.0f);
            float bonus = UMath::Lerp(0.0f, -0.4f, t);

            difficulty = bClamp(difficulty + ((bonus * percent_human_complete) * 0.01f), -1.0f, 1.0f);
            update = true;
        }
    } else if (GetRaceType() == GRace::kRaceType_SpeedTrap) {
        float player_points = 0.0f;
        float ai_points = 0.0f;
        float total_points;

        for (int i = 0; i < num_racers; ++i) {
            if (!GetRacerInfo(i).IsFinishedRacing()) {
                return;
            }

            if (GetRacerInfo(i).GetGameCharacter()) {
                ai_points = UMath::Max(ai_points, GetRacerInfo(i).GetPointTotal());
            } else {
                player_points = GetRacerInfo(i).GetPointTotal();
            }
        }

        total_points = UMath::Max(player_points, ai_points);
        if (total_points > 0.0f && player_points > 0.0f && ai_points > 0.0f) {
            float point_spread_ratio = (player_points - ai_points) / total_points;

            if (point_spread_ratio > 0.0f) {
                float lose_margin = point_spread_ratio;
                float t = UMath::Ramp(lose_margin, 0.05f, 0.2f);
                float bonus = UMath::Lerp(0.0f, 0.2f, t);

                difficulty = bClamp(difficulty + bonus, -1.0f, 1.0f);
            } else {
                float win_margin = -point_spread_ratio;
                float t = UMath::Ramp(win_margin, 0.0f, 0.2f);
                float bonus = UMath::Lerp(0.0f, -0.2f, t);

                difficulty = bClamp(difficulty + bonus, -1.0f, 1.0f);
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
                float t = UMath::Ramp(win_margin, 200.0f, 750.0f);
                float bonus = UMath::Lerp(0.0f, 0.4f, t);

                difficulty = bClamp(difficulty + bonus, -1.0f, 1.0f);
                update = true;
            }
        }
    } else if (winning_player) {
        for (int i = 0; i < num_racers; ++i) {
            GRacerInfo *info = &GetRacerInfo(i);

            if (info->GetGameCharacter() && !info->IsFinishedRacing()) {
                float lose_margin = (GetRaceLength() * (100.0f - info->GetPctRaceComplete())) * 0.01f;

                if (lose_margin > 0.0f) {
                    float t = UMath::Ramp(lose_margin / 300.0f, 0.0f, 1.0f);
                    float bonus = UMath::Lerp(-0.1f, -0.4f, t);

                    difficulty = bClamp(difficulty + ((bonus * info->GetPctRaceComplete()) * 0.01f), -1.0f, 1.0f);
                    update = true;
                }
            }
        }
    } else if (eliminated_player) {
        float num_laps;
        float race_lose_margin;
        float lose_margin;

        num_laps = static_cast<float>(bMax(1, GetRaceParameters()->GetNumLaps()));

        lose_margin = (GetRaceLength() * (100.0f - eliminated_player->GetPctRaceComplete())) * 0.01f;
        race_lose_margin = GetRaceLength() / num_laps;
        lose_margin = UMath::Mod(lose_margin, race_lose_margin);

        if (lose_margin > 0.0f) {
            float t = UMath::Ramp(lose_margin / 300.0f, 0.0f, 1.0f);
            float bonus = UMath::Lerp(-0.1f, -0.4f, t);

            difficulty = bClamp(difficulty + ((bonus * eliminated_player->GetPctRaceComplete()) * 0.01f), -1.0f, 1.0f);
            update = true;
        }
    }

    if (update) {
        float clamped_difficulty;

        mCaluclatedAdaptiveGain = true;
        clamped_difficulty = bClamp(difficulty, -1.0f, 1.0f);

        if (FEDatabase) {
            FEDatabase->GetCareerSettings()->SetAdaptiveDifficulty(clamped_difficulty);
        }

        fCatchUpAdaptiveBonus = clamped_difficulty;
    }
}

bool GRaceStatus::ComputeCatchUpSkill(GRacerInfo *racer_info, PidError *pid, float *output, float *skill, bool off_world) {
    float glue_level = 0.5f;
    bool is_boss = false;
    bool use_race_override = false;
    float percent_complete;
    float glue_skill;
    float glue_spread;
    float glue_integral;
    float glue_derivative;
    float glue_p;
    float glue_error;
    float glue_output;

    if (off_world) {
        glue_level = 1.0f;
    } else {
        if (GetRaceContext() != GRace::kRaceContext_QuickRace) {
            if (GetRaceContext() != GRace::kRaceContext_Career) {
                return false;
            }

            glue_level = UMath::Clamp((GetAdaptiveDifficutly() + 1.0f) * 0.5f, 0.0f, 1.0f);
            if (mRaceParms) {
                if (mRaceParms->GetCatchUpOverride()) {
                    use_race_override = true;
                }

                if (mRaceParms->GetIsBossRace()) {
                    is_boss = true;
                    glue_level = UMath::Lerp(glue_level, 1.0f, 0.5f);
                }
            }
        } else {
            if (!mRaceParms) {
                return false;
            }

            if (!mRaceParms->GetCatchUp()) {
                return false;
            }

            glue_level = Tweak_QuickRaceGlue[mRaceParms->GetDifficulty()];
        }
    }

    percent_complete = racer_info->GetPctRaceComplete();
    glue_p = pid->GetErrorIntegral();
    glue_error = pid->GetErrorDerivative();

    if (!use_race_override) {
        glue_spread = UMath::Lerp(Tweak_GlueSpreadTable_Low.GetValue(percent_complete), Tweak_GlueSpreadTable_High.GetValue(percent_complete), glue_level);
        glue_skill = UMath::Lerp(Tweak_GlueStrengthTable_Low.GetValue(percent_complete), Tweak_GlueStrengthTable_High.GetValue(percent_complete), glue_level);
        glue_integral = 5.0e-5f;
        glue_derivative = 0.01f;
    } else {
        {
            Table glue_table(aCatchUpSkillData, nCatchUpSkillEntries, 0.0f, 100.0f);

            glue_skill = glue_table.GetValue(percent_complete);
        }
        {
            Table glue_table(aCatchUpSpreadData, nCatchUpSpreadEntries, 0.0f, 100.0f);

            glue_spread = glue_table.GetValue(percent_complete);
        }
        glue_derivative = fCatchUpDerivative;
        glue_integral = fCatchUpIntegral;
    }

    glue_spread = bMax(100.0f, glue_spread);
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
    UMath::Vector3 delta;
    float pathDistance;
    float segmentDistance = 0.0f;

    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.SetDecisionFilter(true);
    nav.SetPathType(WRoadNav::kPathChopper);

    mRaceParms->GetNumCheckpoints();
    VU0_v3sub(UMath::Vector4To3(positions[start]), UMath::Vector4To3(positions[end]), delta);
    pathDistance = VU0_sqrt(VU0_v3lengthsquare(delta));

    nav.InitAtPoint(UMath::Vector4To3(positions[start]), UMath::Vector4To3(directions[start]), true, 1.0f);
    if (nav.IsValid()) {
        if (start == end) {
            short segment = nav.GetSegmentInd();
            float segLenScale = static_cast<float>(nav.GetSegment()->nLength) * 0.015259022f;

            do {
                float lengthDelta = UMath::Max(segLenScale * (1.0f - nav.GetSegTime()), 0.01f);

                segmentDistance += lengthDelta;
                nav.IncNavPosition(lengthDelta, UMath::Vector4To3(directions[end]), 0.0f);
            } while (segment == nav.GetSegmentInd());
        }

        UTL::Std::set<PathSegment, _type_ID_PATH_SET> pathSegments;
        char shortcutAllowed[32];
        bool noShortcuts = true;
        WRoadNetwork *roadNetwork = &WRoadNetwork::Get();

        bool foundPath;

        bMemSet(shortcutAllowed, 1, sizeof(shortcutAllowed));
        do {
            nav.SetNavType(WRoadNav::kTypeDirection);
            nav.FindPathNow(&UMath::Vector4To3(positions[end]), &UMath::Vector4To3(directions[end]), shortcutAllowed);

            foundPath = nav.GetNavType() == WRoadNav::kTypePath;
            if (foundPath) {
                roadNetwork->AddRaceSegments(&nav);
                pathDistance = nav.GetPathDistanceRemaining();

                unsigned char shortcut = nav.FirstShortcutInPath();

                noShortcuts = shortcut == 0xFF;
                if (!noShortcuts) {
                    shortcutAllowed[shortcut] = 0;
                }
                PathSegment pathSegment;

                pathSegment.Length = pathDistance;
                int numSegments = nav.GetNumPathSegments();

                for (int i = 0; i < numSegments; ++i) {
                    const WRoadSegment *roadSegment = roadNetwork->GetSegment(nav.GetPathSegment(i));

                    if ((roadSegment->fFlags & 1) == 0) {
                        pathSegment.Roads.insert(roadSegment->fRoadID);
                    }
                }
                pathSegments.insert(pathSegment);
            }
        } while (foundPath && !noShortcuts);

        int numPaths = 0;

        if (noShortcuts) {
            numPaths = pathSegments.size();
        }

        pathDistance += segmentDistance;

        if (numPaths == 0) {
            bRaceRouteError = true;
        }

        if (numPaths > 1) {
            PathSegment *sortedPaths[32];
            int count = 0;

            for (UTL::Std::set<PathSegment, _type_ID_PATH_SET>::iterator it = pathSegments.begin(); it != pathSegments.end(); ++it) {
                sortedPaths[count++] = const_cast<PathSegment *>(&*it);
            }

            for (int i = 1; i < count; ++i) {
                PathSegment *longer_path = sortedPaths[i];
                PathSegment *shorter_path = sortedPaths[i - 1];
                UTL::Std::set<short, _type_ID_ROAD_SET> unique_roads;

                std::set_difference(
                    shorter_path->Roads.begin(),
                    shorter_path->Roads.end(),
                    longer_path->Roads.begin(),
                    longer_path->Roads.end(),
                std::insert_iterator<UTL::Std::set<short, _type_ID_ROAD_SET> >(unique_roads, unique_roads.begin()));
                float unique_length = 0.0f;
                float longer_by = longer_path->Length - shorter_path->Length;

                for (UTL::Std::set<short, _type_ID_ROAD_SET>::iterator it = unique_roads.begin(); it != unique_roads.end(); ++it) {
                    unique_length += static_cast<float>(roadNetwork->GetRoad(*it)->nLength) * 0.061036088f;
                }

                if (unique_length > 0.0f) {
                    float road_scale = (longer_by + unique_length) / unique_length;

                    for (UTL::Std::set<short, _type_ID_ROAD_SET>::iterator it = unique_roads.begin(); it != unique_roads.end(); ++it) {
                        WRoad *road = const_cast<WRoad *>(roadNetwork->GetRoad(*it));
                        int scale = static_cast<int>(road_scale * 65536.0f);

                        road->nScale = static_cast<unsigned short>(scale >> 8);
                    }
                }
            }
        }
    } else {
        bRaceRouteError = true;
        mRaceParms->GetNumCheckpoints();
    }

    return pathDistance;
}

void GRaceStatus::DetermineRaceLength() {
    WRoadNetwork &rn = WRoadNetwork::Get();
    GRaceParameters *race_parameters = mRaceParms;

    nSpeedTraps = 0;
    fSubsequentLapLength = 0.0f;
    fRaceLength = 0.0f;
    fFirstLapLength = 0.0f;
    bMemSet(mSegmentLengths, 0, sizeof(mSegmentLengths));
    bRaceRouteError = false;
    rn.ResolveShortcuts();

    if (!race_parameters || !race_parameters->HasFinishLine()) {
        return;
    }

    {
        int numCheckpoints;
        int numPathPoints;
        UMath::Vector4 positions[18];
        UMath::Vector4 directions[18];
        float totalDistance;
        bool raceLoops;
        int numSegmentLengths;
        int j;
        const bool forceCentreLane = true;
        const float directionWeight = 1.0f;

        rn.SetRaceFilterValid(true);
        numCheckpoints = race_parameters->GetNumCheckpoints();
        numPathPoints = numCheckpoints + 2;
        race_parameters->GetStartPosition(UMath::Vector4To3(positions[0]));
        positions[0].w = 0.0f;
        race_parameters->GetStartDirection(UMath::Vector4To3(directions[0]));
        directions[0].w = 0.0f;
        race_parameters->GetFinishPosition(UMath::Vector4To3(positions[numPathPoints - 1]));
        positions[numPathPoints - 1].w = 0.0f;
        race_parameters->GetFinishDirection(UMath::Vector4To3(directions[numPathPoints - 1]));
        directions[numPathPoints - 1].w = 0.0f;

        for (int i = 0; i < numCheckpoints; ++i) {
            race_parameters->GetCheckpointPosition(i, UMath::Vector4To3(positions[i + 1]));
            positions[i + 1].w = 0.0f;
            race_parameters->GetCheckpointDirection(i, UMath::Vector4To3(directions[i + 1]));
            directions[i + 1].w = 0.0f;
        }

        totalDistance = 0.0f;
        raceLoops = race_parameters->GetIsLoopingRace();
        numSegmentLengths = numPathPoints - 1;
        if (raceLoops) {
            numSegmentLengths = numPathPoints;
        }

        for (j = 0; j < numSegmentLengths; ++j) {
            mSegmentLengths[j] = DetermineRaceSegmentLength(positions, directions, j, (j % (numPathPoints - 1)) + 1);
            totalDistance += mSegmentLengths[j];
        }

        if (!raceLoops) {
            fSubsequentLapLength = totalDistance;
            fRaceLength = totalDistance;
            fFirstLapLength = totalDistance;
        } else {
            fSubsequentLapLength = totalDistance - mSegmentLengths[0];
            fFirstLapLength = totalDistance - mSegmentLengths[numPathPoints - 1];
            fRaceLength = fSubsequentLapLength * static_cast<float>(race_parameters->GetNumLaps() - 1) + fFirstLapLength;
        }

        WRoadNav nav;

        nav.SetPathType(static_cast<WRoadNav::EPathType>(6));
        nav.InitAtPoint(UMath::Vector4To3(positions[numPathPoints - 1]), UMath::Vector4To3(directions[numPathPoints - 1]), forceCentreLane, directionWeight);
        if (nav.IsValid()) {
            for (int i = 0; i < 100; ++i) {
                int segmentNumber;
                WRoadSegment *segment;

                nav.IncNavPosition(1.0f, UMath::Vector3::kZero, 0.0f);
                segmentNumber = nav.GetSegmentInd();
                segment = rn.GetSegmentNonConst(segmentNumber);
                segment->SetInRace(true);
                segment->SetRaceRouteForward(nav.GetNodeInd() == 1);
            }
        }

        GObjectIterator<GTrigger> iter(0x100);
        int numSpeedTraps = 0;

        while (iter.IsValid()) {
            GTrigger *trigger = iter.GetInstance();

            if (!trigger->OpenWorldSpeedTrap(0)) {
                aSpeedTraps[numSpeedTraps] = trigger;
                ++numSpeedTraps;
            }

            iter.Advance();
        }

        nSpeedTraps = numSpeedTraps;
    }
}

template void GRaceCustom::SetAttribute<int>(unsigned int key, const int &value, unsigned int index);
template void GRaceCustom::SetAttribute<bool>(unsigned int key, const bool &value, unsigned int index);
