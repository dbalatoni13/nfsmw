#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPursuitLength.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/EAXAirSupport.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

// El ELF carga aqui 1/2.237f (0x3EE4E0C2) y lo multiplica por 70.0f SIN plegar
// las dos constantes: es una inline, no una expresion constante. MPH2MPS de
// ConversionUtil.hpp da 0.44703001f, que es otro valor.
static inline float PursuitMPH2MPS(float mph) {
    return mph / 2.237f;
}

PursuitFormation::PursuitFormation()
    : mMinFinisherCops(1), //
      mMaxCops(0),         //
      mHasFinisher(false) {
    Reset();
}

PursuitFormation::~PursuitFormation() {
    Reset();
}

void PursuitFormation::Reset() {
    mTargetOffsets.clear();
}

void PursuitFormation::AddTargetOffset(const UMath::Vector3 &targetOffset, int minTargets, UCrc32 ipg, const UMath::Vector3 &inPositionOffset) {
    mTargetOffsets.push_back(TargetOffset(targetOffset, inPositionOffset, minTargets, ipg));
}

BoxInFormation::BoxInFormation(int copcount, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib) {
        tightness = pursuitLevelAttrib->BoxinTightness();
        finishertime = pursuitLevelAttrib->BoxinDuration();
    } else {
        tightness = 0.5f;
        finishertime = 2.0f;
    }

    UMath::Vector3 pos;
    UMath::Vector3 fpos;
    float foff = 2.0f - (tightness * 5.0f);
    float fscale = 0.7f - (tightness * 0.5f);

    getPosition(0, 1.0f, pos);
    getPosition(3, fscale, fpos);
    fpos.z = foff;
    AddTargetOffset(pos, 1, "AIGoalRam", fpos);

    getPosition(1, 1.0f, pos);
    getPosition(1, fscale, fpos);
    AddTargetOffset(pos, 2, "AIGoalRam", fpos);

    getPosition(2, 1.0f, pos);
    getPosition(2, fscale, fpos);
    AddTargetOffset(pos, 2, "AIGoalRam", fpos);

    getPosition(3, 1.0f, pos);
    getPosition(3, fscale, fpos);
    AddTargetOffset(pos, 4, "AIGoalRam", fpos);

    SetMaxCops(4);
    SetMinFinisherCops(2);
    SetHasFinisher(true);
}

void BoxInFormation::getPosition(int idx, float scale, UMath::Vector3 &pos) {
    struct vec3 {
        UMath::Vector3 v;
    };
    static const vec3 base_pos[4] = {{0.0f, 0.0f, 14.0f}, {-3.5f, 0.0f, 0.0f}, {3.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -7.5f}};

    UMath::Scale(base_pos[idx].v, scale, pos);
}

// Functionally matching
void BoxInFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = (tightness * 0.2f) + 0.2f;
    float scale = (finisher * ftight) + (1.0f - ftight);

    for (int i = 0; i < 4; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

RollingBlockFormation::RollingBlockFormation(int numCops, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib) {
        tightness = pursuitLevelAttrib->RollingBlockTightness();
        finishertime = pursuitLevelAttrib->RollingBlockDuration();
    } else {
        tightness = 0.5f;
        finishertime = 2.0f;
    }

    float fscale = 1.0f - (tightness * 0.8f);
    float foff = 2.0f - (tightness * 5.0f);
    static const int priority[5] = {1, 2, 2, 3, 3};

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        UMath::Vector3 fpos;

        getPosition(i, 1.0f, pos);
        getPosition(i, fscale, fpos);
        fpos.z = foff;
        AddTargetOffset(pos, priority[i], "AIGoalRam", fpos);
    }

    SetMaxCops(4);
    SetMinFinisherCops(2);
    SetHasFinisher(true);
}

void RollingBlockFormation::getPosition(int idx, float scale, UMath::Vector3 &pos) {
    struct vec3 {
        UMath::Vector3 v;
    };
    static const vec3 base_pos[5] = {{0.0f, 0.0f, 14.0f}, {-2.5f, 0.0f, 14.0f}, {2.5f, 0.0f, 10.0f}, {-5.0f, 0.0f, 14.0f}, {5.0f, 0.0f, 14.0f}};

    UMath::Scale(base_pos[idx].v, scale, pos);
}

// Functionally matching
void RollingBlockFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = tightness * 0.4f;
    float scale = (finisher * ftight) + (1.0f - ftight);

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

FollowFormation::FollowFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(6);
    SetHasFinisher(false);
}

// total size: 0x20
class StaggerFollowFormation : public PursuitFormation {
  public:
    StaggerFollowFormation(int copcount);

    // Overrides: PursuitFormation
};

StaggerFollowFormation::StaggerFollowFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(6);
    SetHasFinisher(false);
}

PitFormation::PitFormation(int copcount) {
    UMath::Vector3 stupid_hack;
    UMath::Vector3 stupid_hack1;

    stupid_hack = UMath::Vector3Make(4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(-10.0f, 0.0f, -2.7f);
    AddTargetOffset(stupid_hack, 1, "AIGoalPit", stupid_hack1);

    stupid_hack = UMath::Vector3Make(-4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(10.0f, 0.0f, -2.7f);
    AddTargetOffset(stupid_hack, 1, "AIGoalPit", stupid_hack1);

    SetMaxCops(1);
    SetHasFinisher(true);
}

HerdFormation::HerdFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 0.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 5.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, -5.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(3);
    SetHasFinisher(false);
}

void HerdFormation::Update(float dT, IPursuit *pursuit) {
    AITarget *target = pursuit->GetTarget();
    if (!target) {
        return;
    }

    IVehicleAI *vehicleai;
    if (!target->QueryInterface(&vehicleai)) {
        return;
    }
    WRoadNav *roadnav = vehicleai->GetDriveToNav();
    if (!roadnav) {
        return;
    }

    WRoadNav queryNav;
    UMath::Vector3 targetforward;
    target->GetForwardVector(targetforward);
    queryNav.InitAtPoint(target->GetPosition(), targetforward, true, 0.0f);

    UMath::Vector3 roadpos = queryNav.GetPosition();
    UMath::Vector3 roaddir = queryNav.GetForwardVector();
    UMath::Normalize(roaddir);

    UMath::Vector3 roadside = UMath::Vector3Make(roaddir.z, roaddir.y, -roaddir.x);

    UMath::Vector3 roadoff;
    UMath::Sub(target->GetPosition(), roadpos, roadoff);
    float roadoffset = UMath::Dot(roadoff, roadside);

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(queryNav.GetSegmentInd());
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, queryNav.GetNodeInd());
    if (!profile || profile->fNumZones == 0) {
        return;
    }

    UMath::Vector3 segmentForwardVector;
    roadNetwork.GetSegmentForwardVector(queryNav.GetSegmentInd(), segmentForwardVector);

    bool inverted = UMath::Dot(segmentForwardVector, targetforward) < 0.0f;
    int rightmostlaneindex = profile->GetNumLanes(true, inverted);
    float rightmostlaneoffset = 0.0f;
    for (int i = 0; i < rightmostlaneindex; i++) {
        if (profile->GetLaneType(i, inverted) == 1) {
            rightmostlaneoffset = UMath::Max(rightmostlaneoffset, profile->GetLaneOffset(i, inverted));
        }
    }

    float crowddistance = UMath::Max(1.0f, UMath::Min(3.0f, (roadoffset - rightmostlaneoffset) + 2.0f));

    for (TargetOffsetList::iterator i = mTargetOffsets.begin(); i != mTargetOffsets.end(); ++i) {
        i->mOffset.x = -crowddistance;
    }
}

void GroundSupportRequest::Reset() {
    bool bAddToContingent = true;
    if (mSupportRequestStatus == ACTIVE && mHeavySupport && mHeavySupport->HeavyStrategy == HEAVY_ROADBLOCK) {
        bAddToContingent = false;
    }
    mSupportRequestStatus = NOT_ACTIVE;
    mHeavySupport = nullptr;
    mLeaderSupport = nullptr;
    mSupportTimer = 0.0f;

    if (bAddToContingent) {
        for (IVehicle::List::iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
            IVehicle *iv = *iter;
            IPursuitAI *ipv;
            if (iv->QueryInterface(&ipv)) {
                ipv->SetSupportGoal((const char *)nullptr);
                if (iv->IsActive()) {
                    IVehicleAI *ivai;
                    // unchecked
                    ipv->QueryInterface(&ivai);
                    IPursuit *ip = ivai->GetPursuit();
                    if (ip) {
                        ip->AddVehicleToContingent(iv);
                    }
                }
            }
        }
    }
    mIVehicleList.clear();
}

void GroundSupportRequest::Update(float dT) {
    if (mSupportRequestStatus != NOT_ACTIVE) {
        mSupportTimer -= dT;
        if (mSupportTimer < 0.0f) {
            Reset();
        }
    }
}

AIPursuit::AIPursuit(Sim::Param params)
    : Sim::Activity(1),                  //
      IPursuit(this),                    //
      mCoolDownTimeRequired(60.0f),      //
      mTarget(nullptr),                  //
      mFormation(nullptr),               //
      mRoadBlock(nullptr),               //
      mTimeSinceSetupSpeech(),           //
      mBustedTimer(0.0f),                //
      mBustedIncrement(0.0f),            //
      mBustedHUDTime(0.0f),              //
      mIsPerpBusted(false),              //
      mIsPursuitBailed(false),           //
      mMostRecentCopDestroyedType(),     //
      mEvadeLevel(0.0f),                 //
      mCoolDownTimeRemaining(0.0f),      //
      mPercentOfContingentEngaged(0.0f), //
      mNumCopsFullyEngaged(0),           //
      mPursuitMeter(0.0f),               //
      mIsPerpInSight(true),              //
      mHiddenZoneTime(0.0f),             //
      mRepPointsPerMinute(0),            //
      mTotalCopsInvolved(0),             //
      mCopsDestroyed(0),                 //
      mNumCopsRequiredToEvade(0),        //
      mNumCopsToTriggerBackupTime(0),    //
      mNumFullyEngagedCopsEvaded(0),     //
      mNumHeliSpawns(0),                 //
      mNumRoadblocksDodged(0),           //
      mNumRoadblocksDeployed(0),         //
      mNumCopsDamaged(0),                //
      mNumCopsNeeded(0),                 //
      mCrossState(CROSS_AVAILABLE),      //
      mNumTrafficCarsHit(0),             //
      mNumSpikeStripsDodged(0),          //
      mFastSpawnNext(false),             //
      mPropertyDamageValue(0),           //
      mPropertyDamageCount(0),           //
      mNumSpikeStripsDeployed(0),        //
      mNumHeliSpikeStripsDeployed(0),    //
      mNumCopCarsDeployed(0),            //
      mNumSupportVehiclesDeployed(0),    //
      mNumSupportVehiclesActive(0),      //
      mNextRoadblockRequest(false),      //
      mGroundSupportRequest(),           //
      mTimeSinceAnyCopSawPerp(-5.0f),    //
      mEnterSafehouseOnDestruct(false),  //
      mPursuitStatus(PS_INITIAL_CHASE),  //
      mBackupCountdownTimer(0.0f) {
    mSimulateTask = AddTask("AIPursuit", 0.25f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    mBustedTimerTask = AddTask("AIPursuit", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mSimulateTask, "AIPursuit");

    mIVehicleList.clear();
    mIVehicleList.reserve(10);

    mNearestCopInRoadblock = nullptr;
    mRoadBlockTimer = 0.0f;
    mDistanceToNearestCopInRoadblock = 0.0f;
    mTarget = new AITarget(nullptr);
    mTarget->Clear();

    mInFormationTimer = 0.0f;
    mTotalPursuitTime = 0.0f;
    mBreakerTimer = -1.0f;
    mCollapseActive = false;
    mFormationAttemptCount = 0;

    mLastKnownLocation = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    mCopContingent.reserve(5);

    mAllowStatsToAccumulate = false;
    if (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::IsFinalEpicPursuit()) {
            mBaseHeat = mMaximumHeat = 6.0f;
        } else {
            mBaseHeat = GRaceStatus::Get().GetBinBaseHeat();
            mMaximumHeat = GRaceStatus::Get().GRaceStatusGetBinMaxHeat();
        }
    } else {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_QuickRace && !GRaceStatus::IsChallengeRace()) {
            mBaseHeat = 1.0f;
            mMaximumHeat = 5.0f;
        } else {
            mBaseHeat = GRaceStatus::Get().GetBinBaseHeat();
            mMaximumHeat = GRaceStatus::Get().GRaceStatusGetBinMaxHeat();
            mHeatScale = GRaceStatus::Get().GetBinHeatScale();

            bool useWorldHeat = false;
            if (GRaceStatus::Exists()) {
                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                if (parms) {
                    useWorldHeat = parms->GetUseWorldHeatInRace();
                }
                if (useWorldHeat) {
                    mBaseHeat = parms->GetForceHeatLevel();
                    mMaximumHeat = parms->GetMaxRaceHeatLevel();
                    mHeatScale = 1.0f;
                }
            }
            if (!useWorldHeat) {
                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                if (parms) {
                    if (parms->GetMaxHeatLevel() < mMaximumHeat) {
                        mMaximumHeat = parms->GetMaxHeatLevel();
                    }
                }
            }
        }
    }

    mCurrentPursuitLevel = 0;
    mActiveFormationTime = 0.0f;
    mActiveFormation = STAGGER_FOLLOW;
    InitFormation(0);
    mSpawnCopTimer = 0.0f;
    mSpawnHeliTimer = 10.0f;
    mDoTestForHeliSearch = false;
    mForceHeliSpawnNext = false;
    mCopDestroyedBonusTimer = 0.0f;
    mCopDestroyedBonusMultiplier = 1;
    mMostRecentCopDestroyedRepPoints = 0;
    mMostRecentCopDestroyedType = (const char *)nullptr;
    mCoolDownMeterDisplayed = false;
    mPursuitMeterModeTimer = 0.0f;
    mSupportCheckTimer = 10.0f;
    mSupportPriorityCheckDone = false;
    mGroundSupportRequest.Reset();

    mJerkLagPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    mJerkLagDistance = 1000.0f;
    mJerkLagSpeed = 0.0f;
    mIsAJerk = false;
    mNumRBCopsAdded = 0;
    mMinDistanceToTarget = 100000.0f;
}

AIPursuit::~AIPursuit() {
    this->DetachAll();
    this->RemoveTask(this->mSimulateTask);
    this->RemoveTask(this->mBustedTimerTask);

    delete this->mFormation;
    delete this->mTarget;
}

Sim::IActivity *AIPursuit::Construct(Sim::Param params) {
    return new AIPursuit(params);
}

Attrib::Gen::pursuitlevels *AIPursuit::GetPursuitLevelAttrib() const {
    Attrib::Gen::pursuitlevels *plevels = nullptr;
    IPerpetrator *perp;
    if (GetTarget()) {
        if (GetTarget()->QueryInterface(&perp)) {
            plevels = perp->GetPursuitLevelAttrib();
        }
    } else {
        plevels = nullptr;
    }
    return plevels;
}

Attrib::Gen::pursuitsupport *AIPursuit::GetPursuitSupportAttrib() const {
    Attrib::Gen::pursuitsupport *ps = nullptr;
    IPerpetrator *perp;
    if (GetTarget()) {
        if (GetTarget()->QueryInterface(&perp)) {
            ps = perp->GetPursuitSupportAttrib();
        }
    } else {
        ps = nullptr;
    }
    return ps;
}

void AIPursuit::LockInPursuitAttribs() {
    Attrib::Gen::pursuitlevels *ps = GetPursuitLevelAttrib();
    if (ps) {
        mNumCopsRequiredToEvade = ps->FullEngagementCopCount();
        mNumCopsToTriggerBackupTime = ps->NumCopsToTriggerBackup();
        mCoolDownTimeRequired = ps->evadetimeout();
        mNumFullyEngagedCopsEvaded = 0;
    }
}

uint32 AIPursuit::CalcTotalCostToState() const {
    uint32 total = mCopsDestroyed * 5000;
    total += mNumHeliSpawns * 2000;
    total += mNumRoadblocksDeployed * 500;
    total += mNumCopsDamaged * 250;
    total += mNumTrafficCarsHit * 500;
    total += mNumSpikeStripsDeployed * 250;
    total += mNumHeliSpikeStripsDeployed * 225;
    total += mNumCopCarsDeployed * 250;
    total += mNumSupportVehiclesDeployed * 450;
    total += mPropertyDamageValue;

    return total;
}

void AIPursuit::AddVehicleToContingent(IVehicle *ivehicle) {
    UCrc32 hash = ivehicle->GetVehicleName();
    for (ContingentVector::iterator i = mCopContingent.begin();; ++i) {
        if (i == mCopContingent.end()) {
            mCopContingent.push_back(CopContingent(hash));
            break;
        } else if (i->mType == hash) {
            i->mCount++;
            break;
        }
    }
}

void AIPursuit::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        IPursuitAI *ipv;
        IPerpetrator *iperp;
        if (ivehicle->QueryInterface(&iperp)) {
            mTarget->Aquire(ivehicle->GetSimable());
            mJerkLagPosition = mTarget->GetPosition();

            if (IsPlayerPursuit()) {
                CameraAI::MaybeDoPursuitCam(ivehicle);
                PostRacePursuitScreen::GetPursuitData().ClearData();
                GInfractionManager::Get().PursuitStarted();
                GManager::Get().NotifyPursuitStarted();
            }

            float heat = iperp->GetHeat();
            if (heat < mBaseHeat) {
                heat = mBaseHeat;
            }
            iperp->SetHeat(heat);
            iperp->ClearPendingRepPoints();
        } else if (ivehicle->QueryInterface(&ipv)) {
            mIVehicleList.push_back(ivehicle);

            Attrib::Gen::pursuitlevels *plevels = GetPursuitLevelAttrib();
            if (plevels) {
                if (mTotalCopsInvolved < 3 && mPursuitStatus != PS_COOL_DOWN) {
                    mSpawnCopTimer = plevels->TimeBetweenFirstFourSpawn();
                } else {
                    mSpawnCopTimer = plevels->TimeBetweenCopSpawn();
                    if (mNumCopsNeeded > 2) {
                        if (mFastSpawnNext) {
                            mFastSpawnNext = false;
                            mSpawnCopTimer = 0.2f;
                        } else {
                            mFastSpawnNext = true;
                        }
                    }
                }
            } else {
                mSpawnCopTimer = 0.0f;
            }
            mTotalCopsInvolved++;

            const UCrc32 crossName = "copcross";
            const UCrc32 suv = "copsuv";
            const UCrc32 suvl = "copsuvl";
            const UCrc32 hench = "copsporthench";
            const UCrc32 vname = ivehicle->GetVehicleName();

            if (vname == suv || vname == suvl || vname == crossName || vname == hench) {
                mNumSupportVehiclesDeployed++;
                if (vname == crossName) {
                    mCrossState = CROSS_SPAWNED;
                }
            } else {
                if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                    mForceHeliSpawnNext = false;
                    mNumHeliSpawns++;
                } else {
                    mNumCopCarsDeployed++;
                }
            }
            GManager::Get().TrackValue("total_cops_in_pursuit", mTotalCopsInvolved);

            IPerpetrator *iperp;
            if (mTarget->QueryInterface(&iperp) && mRepPointsPerMinute == 0) {
                int perpHeat = static_cast<int>(iperp->GetHeat());
                if (plevels) {
                    mRepPointsPerMinute = plevels->RepPointsPerMinute();
                }
            }

            ipv->StartPursuit(mTarget, nullptr);
            if (IsSupportVehicle(ivehicle)) {
                ipv->StartSupportGoal();
                mNumSupportVehiclesActive++;
            }
            AddVehicleToContingent(ivehicle);
        }
    }
    TrackVehicleCounts();
    Activity::OnAttached(pOther);
}

void AIPursuit::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;

    if (UTL::COM::ComparePtr(pOther, mRoadBlock)) {
        mRoadBlock = nullptr;
    } else {
        if (GetTarget()->IsValid() && UTL::COM::ComparePtr(GetTarget()->GetSimable(), pOther)) {
            ISimable *defaultsimable = IPlayer::First(PLAYER_LOCAL)->GetSimable();

            for (IVehicle::List::iterator i = mIVehicleList.begin(); i != mIVehicleList.end(); ++i) {
                IVehicle *ivehicle = *i;
                ivehicle->GetAIVehiclePtr()->GetTarget()->Aquire(defaultsimable);
            }
            mTarget->Clear();
        } else if (pOther->QueryInterface(&ivehicle)) {
            const UCrc32 crossName = "copcross";
            bool isCross = ivehicle->GetVehicleName() == crossName;

            if (ivehicle->IsDestroyed()) {
                IncNumCopsDestroyed(ivehicle);
                if (isCross) {
                    mCrossState = CROSS_DISABLED;
                }
            } else if (isCross) {
                mCrossState = CROSS_AVAILABLE;
            }

            IAIHelicopter *aih;
            if (ivehicle->QueryInterface(&aih)) {
                Attrib::Gen::pursuitlevels *plevels = GetPursuitLevelAttrib();
                if (plevels) {
                    mSpawnHeliTimer = plevels->TimeBetweenHeliActive();
                }
            }

            IVehicle::List::iterator iter = std::find(mIVehicleList.begin(), mIVehicleList.end(), ivehicle);
            if (iter != mIVehicleList.end()) {
                bool bIsSupport = IsSupportVehicle(ivehicle);
                if (bIsSupport) {
                    mNumSupportVehiclesActive--;
                    if (mNumSupportVehiclesActive == 0) {
                        mGroundSupportRequest.Reset();
                    }
                }
                mIVehicleList.erase(iter);

                IPursuitAI *ipv;
                if (ivehicle->QueryInterface(&ipv)) {
                    if (ipv->WasWithinEngagementRadius() && !bIsSupport && mAllowStatsToAccumulate) {
                        mNumFullyEngagedCopsEvaded++;
                    }
                    ipv->EndPursuit();
                }

                UCrc32 hash = ivehicle->GetVehicleName();
                for (ContingentVector::iterator i = mCopContingent.begin();; i++) {
                    if (i->mType == hash) {
                        i->mCount--;
                        break;
                    }
                }
            }
        }
    }

    TrackVehicleCounts();
}

void AIPursuit::IncNumCopsDestroyed(IVehicle *ivehicle) {
    if (!mAllowStatsToAccumulate) {
        return;
    }
    IVehicleAI *ivai = ivehicle->GetAIVehiclePtr();
    if (ivai) {
        mMostRecentCopDestroyedRepPoints = ivai->GetAttributes().RepPointsForDestroying(mCurrentPursuitLevel);
        mMostRecentCopDestroyedType = ivehicle->GetVehicleName();

        int multiplier = 1;
        if (mCopDestroyedBonusTimer > 0.0f) {
            if (mCopDestroyedBonusMultiplier < 3) {
                mCopDestroyedBonusMultiplier++;
            }
            multiplier = mCopDestroyedBonusMultiplier;
        } else {
            mCopDestroyedBonusTimer = 0.0f;
            mCopDestroyedBonusMultiplier = 1;
        }
        int repForDestruction = mMostRecentCopDestroyedRepPoints * multiplier;

        Attrib::Gen::pursuitlevels *plevel = GetPursuitLevelAttrib();
        if (plevel) {
            mCopDestroyedBonusTimer = plevel->DestroyCopBonusTime();
        }
        IPerpetrator *iperp;
        if (mTarget->QueryInterface(&iperp)) {
            iperp->AddToPendingRepPointsFromCopDestruction(repForDestruction);
        }
    }

    if (mRoadBlock) {
        if (mRoadBlock->IsComprisedOf(ivehicle->GetSimable()->GetOwnerHandle())) {
            mRoadBlock->IncNumCopsDestroyed();
        }
    }
    mCopsDestroyed++;
    GManager::Get().TrackValue("cops_destroyed_in_pursuit", mCopsDestroyed);
}

void AIPursuit::TrackVehicleCounts() {
    int copCarCount = 0;
    int chopperCount = 0;

    for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;
        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;
        if (bIsChopper) {
            chopperCount++;
        } else {
            copCarCount++;
        }
    }
    if (GManager::Exists() && mAllowStatsToAccumulate) {
        GManager::Get().TrackValue("cops_in_pursuit", copCarCount);
        GManager::Get().TrackValue("helis_in_pursuit", chopperCount);
    }
}

FormationType AIPursuit::GetFormationType() const {
    return mActiveFormation;
}

void AIPursuit::InitFormation(int numCops) {
    delete mFormation;

    switch (mActiveFormation) {
        case PIT:
            mFormation = new PitFormation(numCops);
            break;
        case BOX_IN:
            mFormation = new BoxInFormation(numCops, this);
            break;
        case ROLLING_BLOCK:
            mFormation = new RollingBlockFormation(numCops, this);
            break;
        case FOLLOW:
            mFormation = new FollowFormation(numCops);
            break;
        case HERD:
            mFormation = new HerdFormation(numCops);
            break;
        case HELI_PURSUIT:
            mFormation = new FollowFormation(numCops);
            break;
        case STAGGER_FOLLOW:
            mFormation = new StaggerFollowFormation(numCops);
            break;
        default:
            break;
    }
}

void AIPursuit::EndCurrentFormation() {
    mActiveFormationTime = 0.0f;
    mBreakerTimer = -1.0f;
}

void AIPursuit::AssignCopOffset(int cop, Pursuers &assignCopList, const UMath::Vector3 &pursuitOffset, const UMath::Vector3 &inPositionOffset,
                                const UCrc32 &ipg, bool information) {
    int numCops = assignCopList.size();
    if (cop < numCops) {
        IPursuitAI *ipv = assignCopList[cop];
        ipv->SetInPositionOffset(inPositionOffset);
        ipv->SetPursuitOffset(pursuitOffset);
        ipv->SetInFormation(information);
        ipv->SetInPositionGoal(ipg);
    }
}

void AIPursuit::AssignChopperGoal(IPursuitAI *pursuitChopper) {
    if (IsAttemptingRoadBlock())
        return;

    IVehicleAI *via;
    pursuitChopper->QueryInterface(&via);

    if (via->IsCurrentGoal("AIGoalHeliExit"))
        return;

    pursuitChopper->SetInPositionGoal("AIGoalHeliPursuit");
    pursuitChopper->SetInFormation(true);
    if (!via->IsCurrentGoal(pursuitChopper->GetInPositionGoal())) {
        pursuitChopper->DoInPositionGoal();
    }
}

DECLARE_CONTAINER_TYPE(AIPursuitEvenOutOffsetsSourceOffsets);

// Functionally matching I think
void AIPursuit::EvenOutOffsets(Vector3List &copRelativePositions, FormationTargetList &formationOffsets) {
    typedef UTL::Std::vector<const PursuitFormation::TargetOffset *, _type_AIPursuitEvenOutOffsetsSourceOffsets> SourceVector;

    const PursuitFormation::TargetOffsetList &offsetList = mFormation->GetTargetOffsets();

    SourceVector source_offsets;
    source_offsets.reserve(offsetList.size());

    for (PursuitFormation::TargetOffsetList::const_iterator i = offsetList.begin(); i != offsetList.end(); ++i) {
        source_offsets.push_back(&*i);
    }

    while (copRelativePositions.size() > formationOffsets.size() && formationOffsets.size() < mFormation->GetMaxCops()) {
        int bestPriority = 0;
        float bestDistance = 0.0f;
        SourceVector::iterator bestOffset = source_offsets.end();

        for (SourceVector::iterator i = source_offsets.begin(); i != source_offsets.end(); ++i) {
            // TODO does this .end belong here?
            if (*i && (bestOffset == source_offsets.end() || (*i)->mMinTargets <= bestPriority)) {
                UMath::Vector3 offsetPosition = (*i)->mOffset;
                float combined_distance = 0.0f;

                for (Vector3List::const_iterator c = copRelativePositions.begin(); c != copRelativePositions.end(); ++c) {
                    UMath::Vector3 copPosition = *c;
                    combined_distance += UMath::Distance(copPosition, offsetPosition);
                }

                if (bestOffset == source_offsets.end() || combined_distance <= bestDistance) {
                    bestOffset = i;
                    bestDistance = combined_distance;
                    bestPriority = (*bestOffset)->mMinTargets;
                }
            }
        }

        // TODO
        if (bestOffset == source_offsets.end())
            break;

        formationOffsets.push_back(FormationTarget((*bestOffset)->mOffset, (*bestOffset)->mInPositionOffset, (*bestOffset)->mInPositionGoal));
        *bestOffset = 0;
    }
}

DECLARE_CONTAINER_TYPE(AIPursuitAssignClosestOffsetsDistances);
DECLARE_CONTAINER_TYPE(AIPursuitAssignClosestOffsetsMaximums);

// UNSOLVED, 1.684 B al 99,66746 %: quedan OCHO filas y son DOS cosas.
//  a) SEIS filas: permutacion limpia r28<->r29. El objetivo deja el `@ha` del
//     literal -1.0f en r28 y el temporal `copsToAssignOffsets - 1` en r29;
//     nosotros al reves. regmap dice IDENTICO (31 locales, mismo reparto), o sea
//     que los dos pseudos son TEMPORALES del compilador y no hay nada que pinchar.
//  b) DOS filas y CUATRO BYTES (1.680 contra 1.684): el objetivo emite
//     `mr r9,r29` + `cmpwi r29,0` y nosotros `mr. r9,r28`. Es combine.c:1699 (el
//     caso "arithmetic operation and set the condition code"): i3 es
//     `(set cc (compare cTAO 0))`, i2 es la copia `(set cTAO t)` y como el
//     operando del compare ES i2dest, combine mete i2src en el compare y forma el
//     PARALLEL `or.`. Somos una instruccion MAS CORTOS.
//     Y esto ultimo dice que la FUENTE no es `--x > 0`: `expand_increment` de
//     expr.c (rama del final, `temp = copy_rtx (value = op0); ...; return temp;`)
//     devuelve SIEMPRE op0 --la variable-- para un predecremento, o sea que con
//     `--x` el compare no puede leer el temporal. El objetivo compara el
//     temporal (`cmpwi r29,0` con `mr r9,r29` al lado y `subi r29,r9,1` en el
//     tope del bucle, 0x2e6dc): su fuente tiene que ser otra construccion.
//
// r36c, banco de 28 s (mini-TU = zAI.cpp truncado tras AIPursuit.cpp):
//   `copsToAssignOffsets--;` + `while (copsToAssignOffsets > 0)`  -> 1.684 B
//   EXACTOS (rompe la fusion: el compare deja de leer i2dest) y ademas ACIERTA la
//   permutacion r28/r29 de la (a). Pero 11 filas en vez de 8: al desaparecer el
//   `mr.` el reparto cambia entero y el `subi` del contador se va del TOPE del
//   bucle (`subi r29,r9,1` en 0x2e6dc, con la copia `mr r9,r29` abajo) al CUERPO
//   (`subi r5,r29,1` + `mr r29,r5`), con `copsToAssignOffsets` viviendo en un
//   solo registro. Ahi esta lo que falta: la forma que rompa la fusion SIN mover
//   la actualizacion del biv.
// Vedadas y medidas en el mismo banco: `(copsToAssignOffsets -= 1) > 0` y
//   `0 < --copsToAssignOffsets` (BINARIO IDENTICO a la base); `__asm__("")`,
//   `__asm__("" : "+r"(x))` y `__asm__("" : : "r"(x))` tras el decremento (los
//   tres dan el MISMO objeto que `x--`, 1.684 B / 11 filas); `__asm__` antes del
//   `--x` (99,050 %, 10); temporal explicito `int remaining` declarado fuera del
//   do (1.692 B, 21); el decremento al PRINCIPIO del do (1.700 B, 21 -- y ademas
//   es INCORRECTO: los `continue` del cuerpo se lo saltarian).
void AIPursuit::AssignClosestOffsets(Vector3List &copRelativePositions, Pursuers &assignCopList, FormationTargetList &formationOffsets,
                                     bool information) {
    int numRows = copRelativePositions.size();
    int numCols = formationOffsets.size();

    UTL::Std::vector<float, _type_AIPursuitAssignClosestOffsetsDistances> copOffsetDistance;
    UTL::Std::vector<float, _type_AIPursuitAssignClosestOffsetsMaximums> copOffsetMaximums;

    copOffsetDistance.reserve(numRows * numCols);
    copOffsetMaximums.reserve(numRows);

    for (int i = 0; i < numRows; ++i) {
        UMath::Vector3 copPosition = copRelativePositions[i];
        const float zScale = 0.25f;
        copPosition.z *= zScale;

        float maxDistance = 0.0f;
        for (int j = 0; j < numCols; ++j) {
            UMath::Vector3 offsetPosition = formationOffsets[j].Offset;
            offsetPosition.z *= zScale;

            float distance = UMath::Distancexz(offsetPosition, copPosition);
            copOffsetDistance.push_back(distance);

            maxDistance = UMath::Max(distance, maxDistance);
        }
        copOffsetMaximums.push_back(maxDistance);
    }

    const float INDEX_ASSIGNED = -1.0f;
    int copsToAssignOffsets = formationOffsets.size();
    do {
        int currentCop = -1;
        int currentOffset;
        float furthestDistance = 0.0f;

        for (int i = 0; i < numRows; ++i) {
            float distance = copOffsetMaximums[i];
            if (distance != INDEX_ASSIGNED && distance > furthestDistance) {
                furthestDistance = distance;
                currentCop = i;
            }
        }
        {
            int j;
            if (currentCop >= 0) {
                goto search_offsets;
            }
            continue;

        found_offset:
            currentOffset = j;
            goto searched_offsets;

        search_offsets:
            currentOffset = -1;
            // TODO issue with this loop
            for (j = 0; j < numCols; ++j) {
                float distance = copOffsetDistance[currentCop * numCols + j];
                if (distance != INDEX_ASSIGNED && distance == furthestDistance) {
                    goto found_offset;
                }
            }

        searched_offsets:;
        }
        if (currentOffset < 0) {
            continue;
        }

        currentCop = -1;
        float closestDistance = 100000.0f;
        for (int i = 0; i < numRows; ++i) {
            float distance = copOffsetDistance[i * numCols + currentOffset];
            if (distance != INDEX_ASSIGNED && copOffsetMaximums[i] != INDEX_ASSIGNED && distance < closestDistance) {
                closestDistance = distance;
                currentCop = i;
            }
        }
        if (currentCop < 0) {
            continue;
        }

        AssignCopOffset(currentCop, assignCopList, formationOffsets[currentOffset].Offset, formationOffsets[currentOffset].InPositionOffset,
                        formationOffsets[currentOffset].Goal, information);
        copOffsetMaximums[currentCop] = INDEX_ASSIGNED;
        for (int i = 0; i < numRows; ++i) {
            copOffsetDistance[i * numCols + currentOffset] = INDEX_ASSIGNED;

            if (copOffsetMaximums[i] < 0.0f) {
                continue;
            }

            float maxDistance = 0.0f;
            for (int j = 0; j < numCols; ++j) {
                float distance = copOffsetDistance[i * numCols + j];

                maxDistance = UMath::Max(distance, maxDistance);
            }
            copOffsetMaximums[i] = maxDistance;
        }
    } while (--copsToAssignOffsets > 0);
}

static int CopAndAngleSortPredicate(const void *l, const void *r) {
    if (reinterpret_cast<const CopAndAngle *>(l)->angle <= reinterpret_cast<const CopAndAngle *>(r)->angle) {
        return -1;
    } else {
        return 1;
    }
}

static int CopAndAngleDistanceSortPredicate(const void *l, const void *r) {
    if (reinterpret_cast<const CopAndAngle *>(l)->distance <= reinterpret_cast<const CopAndAngle *>(r)->distance) {
        return -1;
    } else {
        return 1;
    }
}

DECLARE_CONTAINER_TYPE(AIPursuitSetupCollapseCopAngles);

// UNSOLVED
inline float cheap_atan_like_function(float f, float s) {
    if (f > 0.0f) {
        if (s > 0.0f) {
            return f > s ? s / f : 2.0f - f / s;
        } else {
            return f > -s ? s / f : -2.0f - f / s;
        }
    } else {
        if (s > 0.0f) {
            return -f > s ? s / f + 4.0f : 2.0f - f / s;
        } else {
            return -f > -s ? s / f + -4.0f : -2.0f - f / s;
        }
    }
}

// Functionally matching
bool AIPursuit::SetupCollapse(const Pursuers &cops, int max_inner, float inner_radius, float outer_radius) {
    typedef UTL::Std::vector<CopAndAngle, _type_AIPursuitSetupCollapseCopAngles> CopAngleVector;

    inner_radius = bMax(3.0f, inner_radius);
    outer_radius = bMax(inner_radius + 1.0f, outer_radius);

    CopAngleVector copangles; // r1+0x8
    copangles.reserve(cops.size());

    AITarget *target = GetTarget();

    UMath::Vector3 front; // r1+0x20
    if (target->GetSpeed() < KPH2MPS(5.0f)) {
        target->GetForwardVector(front);
    } else {
        front = target->GetLinearVelocity();
    }
    UMath::Normalize(front);

    UMath::Vector3 side; // r1+0x30
    side = UMath::Vector3Make(front.z, 0.0f, -front.x);
    UMath::Normalize(side);

    UMath::Vector3 pos = target->GetPosition(); // r1+0x40

    UCrc32 fleegoal("AIGoalFleePursuit");

    Pursuers::const_iterator pursuitIter;
    for (pursuitIter = cops.begin(); pursuitIter != cops.end(); ++pursuitIter) {
        IVehicleAI *iai;
        IPursuitAI *ipv = *pursuitIter;
        if (!ipv->QueryInterface(&iai)) {
            continue;
        }
        UMath::Vector3 off;
        if (UMath::Distance(iai->GetVehicle()->GetPosition(), mTarget->GetPosition()) > 60.0f) {
            continue;
        }
        if (!iai->GetDrivableToTargetPos()) {
            continue;
        }
        if (ipv->GetSupportGoal() != UCrc32::kNull || iai->GetGoalName() == fleegoal) {
            continue;
        }
        if (iai->GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
            continue;
        }
        UMath::Sub(iai->GetVehicle()->GetPosition(), pos, off);
        float d = UMath::Length(off);
        float f = UMath::Dot(front, off);
        float s = UMath::Dot(side, off);
        float angle = cheap_atan_like_function(f, s);

        copangles.push_back(CopAndAngle(ipv, angle, d));
    }

    if (copangles.size() == 0) {
        return false;
    }

    int inneroffset = 0;
    int numinner = copangles.size();
    if ((int)copangles.size() > max_inner) {
        qsort(&copangles[0], copangles.size(), sizeof(CopAndAngle), CopAndAngleDistanceSortPredicate);
        inneroffset = copangles.size() - max_inner;
        numinner = max_inner;
        AssignCopsInCircle(&copangles[0], inneroffset, outer_radius, front, side);
    }
    AssignCopsInCircle(&copangles[inneroffset], numinner, inner_radius, front, side);

    return true;
}

static const UCrc32 kPullOverGoal = "AIGoalPullOver";

void AIPursuit::AssignCopsInCircle(CopAndAngle *copangles, int num, float radius, const UMath::Vector3 &front, const UMath::Vector3 &side) {
    qsort(copangles, num, sizeof(CopAndAngle), CopAndAngleSortPredicate);

    int frontmostCop = 0;
    float smallestAngle = 4.0f;
    float step;

    for (int i = 0; i < num; i++) {
        float a = UMath::Abs(copangles[i].angle);

        if (a < smallestAngle) {
            frontmostCop = i;
            smallestAngle = a;
        }
    }

    step = 6.283185f / num;

    for (int i = 0; i < num; i++) {
        float angle = i * step;
        float c = UMath::Cosr(angle);
        float s = UMath::Sinr(angle);
        unsigned int index = (i + frontmostCop) % num;

        copangles[index].cop->SetInPositionOffset(UMath::Vector3Make(s * radius, 0.0f, c * radius));
        copangles[index].cop->SetInPositionGoal(kPullOverGoal);
        copangles[index].cop->DoInPositionGoal();
    }
}

void AIPursuit::UpdateFormation(float dT) {
    if (!mTarget->IsValid())
        return;

    IVehicleAI *targetvehicleai;
    if (!mTarget->QueryInterface(&targetvehicleai))
        return;

    IRigidBody *itargetRB;
    if (!mTarget->QueryInterface(&itargetRB))
        return;

    mFormation->Update(dT, this);

    Pursuers assignCopList;
    Vector3List copRelativePositions;

    assignCopList.reserve(mIVehicleList.size());
    copRelativePositions.reserve(mIVehicleList.size());

    float formationCandidateLimit = 60.0f;
    formationCandidateLimit =
        UMath::Distance(mTarget->GetPosition(), targetvehicleai->GetCurrentRoad()->GetPosition()) + formationCandidateLimit;

    for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;

        IPursuitAI *ipursuitai;
        if (!ivehicle->QueryInterface(&ipursuitai))
            continue;

        if (IsSupportVehicle(ivehicle))
            continue;

        bool ischopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;

        UMath::Vector3 copRelativePosition = ivehicle->GetSimable()->GetPosition();
        UMath::Sub(copRelativePosition, mTarget->GetPosition(), copRelativePosition);

        ipursuitai->SetInFormation(false);

        if (ischopper) {
            if (!mIsPerpBusted && !mIsPursuitBailed) {
                AssignChopperGoal(ipursuitai);
            }
            continue;
        }

        if (!ivehicle->GetAIVehiclePtr()->GetDrivableToTargetPos())
            continue;

        if (UMath::Length(copRelativePosition) > formationCandidateLimit)
            continue;

        itargetRB->ConvertWorldToLocal(copRelativePosition, false);

        assignCopList.push_back(ipursuitai);
        copRelativePositions.push_back(copRelativePosition);
    }

    FormationTargetList formationOffsets;
    formationOffsets.reserve(mFormation->GetTargetOffsets().size());
    EvenOutOffsets(copRelativePositions, formationOffsets);

    if (copRelativePositions.size() && formationOffsets.size()) {
        AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, true);
    }

    UpdateOutOfFormationOffsets();

    int i = 0;
    int countInFormation = 0;
    unsigned int countInPosition = 0;
    float grossDistanceToTarget = 0.0f;

    for (Pursuers::const_iterator pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter, ++i) {
        IPursuitAI *ipursuitai = *pursuitIter;

        if (ipursuitai->GetInFormation()) {
            countInFormation++;

            UMath::Vector3 pursuitOffset = ipursuitai->GetPursuitOffset();
            UMath::Vector3 copPosition = copRelativePositions[i];

            float distance = UMath::Distancexz(copPosition, pursuitOffset);
            if (distance < 4.0f) {
                grossDistanceToTarget += distance;
                ipursuitai->SetInPosition(true);
                countInPosition++;
                continue;
            }
        }

        ipursuitai->SetInPosition(false);
    }

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;

    IPerpetrator *iperp;
    if (mTarget->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    float collapsespeed = KPH2MPS(pursuitLevelAttrib->CollapseSpeed());
    if (mIsAJerk) {
        collapsespeed = KPH2MPS(125.0f);
    }

    if (mBreakerTimer >= 0.0f && mBreakerTimer < mFormation->GetFinisherTime() && !mIsPerpBusted && !mIsPursuitBailed) {
        mBreakerTimer += dT;
    } else if (pursuitLevelAttrib && mTarget->GetSpeed() < collapsespeed && countInFormation > 0 && !mIsPerpBusted && mIsPerpInSight &&
               !mIsPursuitBailed) {
        mCollapseActive = SetupCollapse(assignCopList, pursuitLevelAttrib->MaxCopsCollapsing(), pursuitLevelAttrib->CollapseInnerRadius(),
                                        pursuitLevelAttrib->CollapseOuterRadius());
        if (mCollapseActive) {
            if (mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE && mGroundSupportRequest.mHeavySupport) {
                for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end();
                     ++vehicleIter) {
                    IVehicle *ivehicle = *vehicleIter;
                    if (IsSupportVehicle(ivehicle)) {
                        IPursuitAI *ipursuitai;
                        if (ivehicle->QueryInterface(&ipursuitai)) {
                            ipursuitai->StartFlee();
                        }
                    }
                }
                mGroundSupportRequest.Reset();
            }
        }
    } else if (!mIsPerpBusted && (mBreakerTimer >= 0.0f || mCollapseActive)) {
        mBreakerTimer = -1.0f;
        mCollapseActive = false;
        mInFormationTimer = 0.0f;

        for (Pursuers::const_iterator pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter) {
            IPursuitAI *ipursuitai = *pursuitIter;
            if (ipursuitai) {
                ipursuitai->SetInPositionGoal(UCrc32::kNull);
                ipursuitai->StartPursuit(mTarget, nullptr);
            }
        }

        for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
            IVehicle *ivehicle = *vehicleIter;

            IPursuitAI *ipursuitai;
            if (ivehicle->QueryInterface(&ipursuitai)) {
                UCrc32 goalName = ivehicle->GetAIVehiclePtr()->GetGoalName();
                UCrc32 inPositionGoal = ipursuitai->GetInPositionGoal();

                if (goalName == inPositionGoal || goalName == kPullOverGoal) {
                    ipursuitai->SetInPositionGoal(UCrc32::kNull);
                    ipursuitai->StartPursuit(mTarget, nullptr);
                }
            }
        }
    } else if (mFormation->GetHasFinisher() && countInPosition > 0 && !mCollapseActive && !mIsPerpBusted && !mIsPursuitBailed) {
        float averageDistance = grossDistanceToTarget / static_cast<float>(countInPosition);
        float tolerance = mFormation->GetFinisherTolerance() * 4.0f;

        mInFormationTimer += bClamp(((tolerance + tolerance) - averageDistance) / tolerance, -1.0f, 1.0f) * dT;
        if (mInFormationTimer < 0.0f) {
            mInFormationTimer = 0.0f;
        }

        if (mInFormationTimer >= mFormation->GetTimeToFinisher()) {
            if (countInPosition >= mFormation->GetMinFinisherCops()) {
                mBreakerTimer = 0.0f;

                for (Pursuers::const_iterator pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter) {
                    IPursuitAI *ipursuitai = *pursuitIter;
                    if (ipursuitai && ipursuitai->GetInFormation() && ipursuitai->GetInPositionGoal() != UCrc32::kNull) {
                        ipursuitai->DoInPositionGoal();
                    }
                }
            } else {
                mInFormationTimer = mFormation->GetTimeToFinisher() - 0.01f;
            }
        }
    } else {
        mInFormationTimer = 0.0f;
    }
}

void AIPursuit::UpdateOutOfFormationOffsets() {
    IRigidBody *itargetRB;
    mTarget->QueryInterface(&itargetRB);

    Pursuers assignCopList;
    Vector3List copRelativePositions;

    assignCopList.reserve(mIVehicleList.size());
    copRelativePositions.reserve(mIVehicleList.size());

    for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;

        IPursuitAI *ipursuitai;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER || !ivehicle->QueryInterface(&ipursuitai))
            continue;

        if (ipursuitai->GetInFormation() || IsSupportVehicle(ivehicle))
            continue;

        UMath::Vector3 copRelativePosition = ivehicle->GetSimable()->GetPosition();
        UMath::Sub(copRelativePosition, mTarget->GetPosition(), copRelativePosition);

        if (itargetRB) {
            itargetRB->ConvertWorldToLocal(copRelativePosition, false);
        }

        ipursuitai->SetInPosition(false);

        assignCopList.push_back(ipursuitai);
        copRelativePositions.push_back(copRelativePosition);
    }

    if (assignCopList.size() == 0) {
        return;
    }

    int i = 0;

    FormationTargetList formationOffsets;
    formationOffsets.reserve(assignCopList.size());

    for (Pursuers::const_iterator pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter, ++i) {
        int ring = i / 6;
        int side = 1 - 2 * (i % 2);
        int column = ((i / 2 + 1) % 3) - 1;

        float offsetX = column * 3.5f;
        float offsetZ = side * (ring * 5.0f + 25.0f);

        UMath::Vector3 offset = UMath::Vector3Make(offsetX, 0.0f, offsetZ);

        formationOffsets.push_back(FormationTarget(offset, UMath::Vector3Make(0.0f, 0.0f, 0.0f), UCrc32::kNull));
    }

    AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, false);
}

// AIPursuit::AttemptingToReAquire esta EN LA CLASE (AIPursuit.h).

bool AIPursuit::IsPlayerPursuit() const {
    return GetTarget() && GetTarget()->GetSimable() && GetTarget()->GetSimable()->GetPlayer();
}

bool AIPursuit::ContingentHasActiveCops() const {
    for (ContingentVector::const_iterator i = mCopContingent.begin(); i != mCopContingent.end(); ++i) {
        if (i->mCount > 0) {
            return true;
        }
    }
    return false;
}

float kBustedHUDTime = 3.0f;

bool AIPursuit::OnTask(HSIMTASK htask, float dT) {
    if (htask == mBustedTimerTask) {
        mBustedTimer += mBustedIncrement;
        if (mBustedTimer < 0.0f) {
            mBustedTimer = 0.0f;
        }
        return true;
    }

    if (htask != mSimulateTask) {
        return true;
    }

    if (!mTarget->IsValid()) {
        return true;
    }

    mAllowStatsToAccumulate = !GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing ||
                              (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace());

    UpdateJerk(dT);

    UpdateFormation(dT);

    float pursuitTimeBeforeUpdate = mTotalPursuitTime;

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;

    bool is_player_perp = IsPlayerPursuit();

    IPerpetrator *iperp;
    if (mTarget->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();

        if (!mNumCopsRequiredToEvade) {
            LockInPursuitAttribs();
        }

        if (GetPursuitStatus() != PS_COOL_DOWN) {
            if (mAllowStatsToAccumulate) {
                mTotalPursuitTime += dT;

                GManager::Get().TrackValue("pursuit_length", mTotalPursuitTime);
            }

            if (static_cast<int>(pursuitTimeBeforeUpdate) != static_cast<int>(mTotalPursuitTime)) {
                MNotifyPursuitLength(mTarget->GetSimable()->GetOwnerHandle(), mTotalPursuitTime).Post(0x20d60dbf);
            }

            float heat = iperp->GetHeat();

            if (pursuitLevelAttrib) {
                float timePerHeatLevel = pursuitLevelAttrib->TimePerHeatLevel();

                mCoolDownTimeRequired = pursuitLevelAttrib->evadetimeout();

                if (FEDatabase->GetCareerSettings()) {
                    int bin = GRaceStatus::IsChallengeRace() ? 14 : FEDatabase->GetCareerSettings()->GetCurrentBin();
                    if (bin > 14) {
                        bin = 14;
                    }
                    timePerHeatLevel *= pursuitLevelAttrib->ScaleEscalationPerBucket(bin);
                }

                heat += dT / timePerHeatLevel;
            }

            iperp->SetHeat(bClamp(heat, mBaseHeat, mMaximumHeat));
            float newHeat = iperp->GetHeat();
            if (static_cast<int>(newHeat) != mCurrentPursuitLevel) {
                mActiveFormationTime = 0.0f;
                mCurrentPursuitLevel = static_cast<int>(newHeat);

                mSupportPriorityCheckDone = false;
                mRepPointsPerMinute = iperp->GetPursuitLevelAttrib()->RepPointsPerMinute();
            }
        }

        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    mRoadBlockTimer -= dT;
    mActiveFormationTime -= dT;
    mSpawnCopTimer -= dT;
    mSpawnHeliTimer -= dT;
    mSupportCheckTimer -= dT;
    mCopDestroyedBonusTimer -= dT;
    mPursuitMeterModeTimer += dT;

    mGroundSupportRequest.Update(dT);

    SoundAI *copspeech = UTL::Collections::Singleton<SoundAI>::Get();
    if (copspeech && copspeech->GetFocus() == 1) {
        mTimeSinceSetupSpeech = WorldTimer;
    }

    float t_speech_finished = (WorldTimer - mTimeSinceSetupSpeech).GetSeconds();

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters()) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    bool speech_finished;
    if (!pursuitRace && IsSpeechEnabled) {
        speech_finished = t_speech_finished > 15.0f;
    } else {
        speech_finished = true;
    }

    if (mActiveFormationTime <= 0.0f && pursuitLevelAttrib && !IsFinisherActive() && !mIsPerpBusted && !mIsPursuitBailed &&
        speech_finished) {
        int numFormations = pursuitLevelAttrib->Num_CopFormations();
        if (numFormations > 0) {
            FormationType formation = STAGGER_FOLLOW;
            float formationTime = pursuitLevelAttrib->StaggerFormationTime();

            if (mRoadBlock && !mRoadBlock->GetDodged() && !mRoadBlock->GetNumCopsDamaged() && !mRoadBlock->GetNumCopsDestroyed()) {
                formation = FOLLOW;
            } else if (mFormationAttemptCount & 1) {
                float totalFrequency = 0.0f;
                int i = 0;
                while (i < numFormations) {
                    totalFrequency += pursuitLevelAttrib->CopFormations(i).Frequency;
                    i++;
                }

                float pick = Sim::GetRandom()._SimRandom_FloatRange(totalFrequency);
                for (i = 0; i < numFormations; i++) {
                    const CopFormationRecord &record = pursuitLevelAttrib->CopFormations(i);
                    pick -= record.Frequency;
                    if (pick <= 0.0f) {
                        formation = record.Formation;
                        formationTime = record.Duration;
                        break;
                    }
                }
            }

            mFormationAttemptCount++;
            if (formation != mActiveFormation) {
                mActiveFormation = formation;
                InitFormation(GetNumCops());
            }

            mActiveFormationTime = formationTime + Sim::GetRandom()._SimRandom_FloatRange(3.0f);
        }
    }

    RemoveUnwantedVehicles();

    float MinDistanceToTarget3 = 3.402823466e+38f;
    float MinDistanceToTargetxz = MinDistanceToTarget3;

    mTimeSinceAnyCopSawPerp += dT;

    float engageRadius = pursuitLevelAttrib ? pursuitLevelAttrib->FullEngagementRadius() : 150.0f;

    int numVehiclesInRadius = 0;

    for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;
        if (ivehicle->IsActive() && !ivehicle->IsDestroyed()) {
            IPursuitAI *ipursuitai;
            if (ivehicle->QueryInterface(&ipursuitai)) {
                float timeSinceSeen = ipursuitai->GetTimeSinceTargetSeen();
                if (timeSinceSeen < mTimeSinceAnyCopSawPerp) {
                    mTimeSinceAnyCopSawPerp = timeSinceSeen;
                }
            }

            float heightDifference = bAbs(ivehicle->GetPosition().y - mTarget->GetPosition().y);
            float distance = UMath::Distance(ivehicle->GetPosition(), mTarget->GetPosition());
            const UMath::Vector3 &vehiclePosition = ivehicle->GetPosition();
            float distancexz = UMath::Distancexz(mTarget->GetPosition(), vehiclePosition);

            if (heightDifference < 1.5f && distancexz < MinDistanceToTargetxz) {
                MinDistanceToTargetxz = distancexz;
            }
            if (distance < MinDistanceToTarget3) {
                MinDistanceToTarget3 = distance;
            }

            if (distance < engageRadius) {
                if (!IsSupportVehicle(ivehicle)) {
                    numVehiclesInRadius++;
                }
                if (ipursuitai) {
                    if (mPursuitStatus != PS_COOL_DOWN) {
                        ipursuitai->SetWithinEngagementRadius();
                    }
                }
            }
        }
    }

    mNumCopsFullyEngaged = numVehiclesInRadius;
    int remainingCopsToEvade = mNumCopsRequiredToEvade - mNumFullyEngagedCopsEvaded;
    if (remainingCopsToEvade <= 0) {
        mNumCopsRequiredToEvade++;
        remainingCopsToEvade++;
    }

    int dif = mNumCopsFullyEngaged - remainingCopsToEvade;
    if (dif > 0) {
        mNumCopsRequiredToEvade += dif;
    }

    if (mNumCopsRequiredToEvade != 0 && mPursuitStatus == PS_INITIAL_CHASE && remainingCopsToEvade <= mNumCopsToTriggerBackupTime) {
        mPursuitStatus = PS_BACKUP_REQUESTED;
        mBackupCountdownTimer = pursuitLevelAttrib->BackupCallTimer();
    }

    if (GRaceStatus::IsFinalEpicPursuit()) {
        mTimeSinceAnyCopSawPerp = 0.0f;
    }

    if (mRoadBlock) {
        if (!iperp->IsHiddenFromCars()) {
            float roadblockDistancexz;
            float roadblockDistance = mRoadBlock->GetMinDistanceToTarget(dT, roadblockDistancexz, &mNearestCopInRoadblock);
            if (roadblockDistance < MinDistanceToTarget3) {
                MinDistanceToTarget3 = roadblockDistance;
                if (MinDistanceToTarget3 < 60.0f) {
                    mIsPerpInSight = true;
                    mTimeSinceAnyCopSawPerp = 0.0f;
                }
            }
            if (roadblockDistancexz < MinDistanceToTargetxz) {
                MinDistanceToTargetxz = roadblockDistancexz;
            }
            mDistanceToNearestCopInRoadblock = roadblockDistance;
        }

        if (mRoadBlock->IsPerpCheating() && !mNumRBCopsAdded) {
            if (mNearestCopInRoadblock) {
                if (mRoadBlock->RemoveVehicle(mNearestCopInRoadblock)) {
                    AddVehicle(mNearestCopInRoadblock);

                    mNumCopsRequiredToEvade++;

                    mNumRBCopsAdded++;
                }
            }
        }
    } else {
        mDistanceToNearestCopInRoadblock = 0.0f;
        mNearestCopInRoadblock = nullptr;
    }

    mMinDistanceToTarget = MinDistanceToTarget3;

    float bustedSpeedLimit = KPH2MPS(pursuitLevelAttrib->BustSpeed());

    if (!mIsPerpBusted) {
        if (mIsPerpInSight && !mIsPursuitBailed) {
            bool invulnerable = false;
            IRBVehicle *irbvehicle;
            if (mTarget->QueryInterface(&irbvehicle) && irbvehicle->GetInvulnerability() == 1)
                invulnerable = true;
            float bustedDistance = 15.0f;
            if (invulnerable)
                bustedDistance *= 6.0f;

            if ((invulnerable || mTarget->GetSpeed() < bustedSpeedLimit) && MinDistanceToTargetxz < bustedDistance) {
                mBustedIncrement = dT * 0.25f;

                if (invulnerable) {
                    mBustedIncrement *= 4.0f;
                }
            } else {
                mBustedIncrement = dT * -0.5f;
            }

            if (UTL::Collections::Singleton<INIS>::Exists() && UTL::Collections::Singleton<INIS>::Get()->IsWorldMomement()) {
                mBustedIncrement = 0.0f;
            }

            int repMinutesBefore = static_cast<int>(pursuitTimeBeforeUpdate * 0.1f);
            int repMinutesNow = static_cast<int>(mTotalPursuitTime * 0.1f);
            if (repMinutesBefore != repMinutesNow) {
                iperp->AddToPendingRepPointsNormal(mRepPointsPerMinute);
            }

            if (mPursuitStatus == PS_BACKUP_REQUESTED) {
                mBackupCountdownTimer -= dT;
                if (mBackupCountdownTimer < 0.0f) {
                    mPursuitStatus = PS_INITIAL_CHASE;
                    LockInPursuitAttribs();
                }
            }
        } else {
            mBustedIncrement = dT * -0.5f;
        }
    } else {
        mBustedIncrement = dT * 0.25f;
    }

    if (!mIsPerpBusted && !mIsPursuitBailed && mBustedTimer > 5.0f) {
        mIsPerpBusted = true;
        mPursuitStatus = PS_BUSTED;

        if (is_player_perp) {
            for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end();
                 ++vehicleIter) {
                IVehicle *ivehicle = *vehicleIter;
                IPursuitAI *ipursuitai;
                if (ivehicle->IsActive() && !ivehicle->IsDestroyed() && ivehicle->QueryInterface(&ipursuitai)) {
                    ivehicle->GlareOff(VehicleFX::LIGHT_COPS);
                    ipursuitai->SetInPositionGoal(UCrc32("AIGoalStopShort"));
                    ipursuitai->DoInPositionGoal();
                }
            }
            MPerpBusted(mTarget->GetSimable()->GetOwnerHandle()).Send(0x20d60dbf);
        } else {
            BailPursuit();
            if (GRaceStatus::Exists()) {
                GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(mTarget->GetSimable());
                if (racerInfo) {
                    racerInfo->Busted();
                    racerInfo->ForceStop();

                    MPerpBusted(mTarget->GetSimable()->GetOwnerHandle()).Send("AIRacerBusted");
                }
            }
        }
    } else if (mIsPerpBusted && is_player_perp) {
        float bustedHUDTimeBefore = mBustedHUDTime;
        mBustedHUDTime += dT;
        if (bustedHUDTimeBefore <= kBustedHUDTime && mBustedHUDTime > kBustedHUDTime) {
            MPerpBusted(mTarget->GetSimable()->GetOwnerHandle()).Send(0xfea34c0a);
        }
    }

    if (mIsPursuitBailed) {
        for (IVehicle::List::const_iterator vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); ++vehicleIter) {
            IVehicle *ivehicle = *vehicleIter;
            IPursuitAI *ipursuitai;
            if (ivehicle->IsActive() && !ivehicle->IsDestroyed() && ivehicle->QueryInterface(&ipursuitai)) {
                ipursuitai->StartFlee();
            }
        }
    }

    mIsPerpInSight = mTimeSinceAnyCopSawPerp < 7.0f;

    if (iperp) {
        bool hidden = iperp->IsHiddenFromCars() || iperp->IsHiddenFromHelicopters();
        if (hidden && !mIsPerpInSight) {
            float hiddenMultiplier = pursuitLevelAttrib ? pursuitLevelAttrib->HiddenZoneTimeMultiplier() : 3.0f;
            mHiddenZoneTime += dT * hiddenMultiplier;
        }

        if (mIsPerpInSight) {
            mHiddenZoneTime = 0.0f;
            mLastKnownLocation = mTarget->GetPosition();
        }
    }

    float sumTimeElapsed = mTimeSinceAnyCopSawPerp + mHiddenZoneTime;

    if (mTimeSinceAnyCopSawPerp > 7.0f) {
        mEvadeLevel = sumTimeElapsed / mCoolDownTimeRequired;

        bool evaded = false;
        mPursuitMeter = -1.0f;
        if (!mCoolDownMeterDisplayed) {
            mEvadeLevel = 0.0f;
            if (mPursuitMeterModeTimer > 2.5f) {
                mPursuitMeterModeTimer = 0.0f;
                mCoolDownMeterDisplayed = true;
                mPursuitStatus = PS_COOL_DOWN;
                mSpawnCopTimer = bMin(mSpawnCopTimer, pursuitLevelAttrib->TimeBetweenCopSpawn());

                mBackupCountdownTimer = 0.0f;
                mDoTestForHeliSearch = true;

                if (IsPlayerPursuit()) {
                    GInfractionManager::Get().ReportResistingArrest();
                }
            }
        } else if (mEvadeLevel < 0.05f) {
            mEvadeLevel = 0.05f;
        } else if (mEvadeLevel >= 1.0f) {
            evaded = true;
        }

        if (evaded) {
            mEvadeLevel = 1.0f;
            mPursuitStatus = PS_EVADED;

            if (IsPlayerPursuit() && ICopMgr::Exists()) {
                ICopMgr::Get()->LockoutCops(true);
            }
        }
    } else {
        if (mTimeSinceAnyCopSawPerp > 0.29f) {
            mPursuitMeter = bClamp(-0.5f - mTimeSinceAnyCopSawPerp * 0.0714285746f, -1.0f, -0.5f);
        } else if (pursuitLevelAttrib) {
            float bustedDeadZone = pursuitLevelAttrib->MeterDeadZoneBustedDistance();
            float evadeDeadZone = pursuitLevelAttrib->MeterDeadZoneEvadeDist();
            float losDistance = pursuitLevelAttrib->frontLOSdistance();

            mPursuitMeter = 0.0f;
            if (MinDistanceToTarget3 > evadeDeadZone) {
                mPursuitMeter =
                    bClamp(-0.1f - ((MinDistanceToTarget3 - evadeDeadZone) / (losDistance - evadeDeadZone)) * 0.4f, -0.5f, -0.1f);
            } else if (MinDistanceToTarget3 < bustedDeadZone) {
                if (mTarget->GetSpeed() > PursuitMPH2MPS(70.0f)) {
                    mPursuitMeter = 0.0f;
                } else {
                    float distanceRatio = bClamp((bustedDeadZone - MinDistanceToTarget3) / (bustedDeadZone - 15.0f), 0.0f, 1.0f);
                    float speedRatio =
                        bClamp((KPH2MPS(100.0f) - mTarget->GetSpeed()) / (KPH2MPS(100.0f) - bustedSpeedLimit), 0.0f, 1.0f);
                    mPursuitMeter = (distanceRatio * 0.3f + speedRatio * 0.7f) * 0.4f + 0.1f;
                }
            }
        }

        if (mCoolDownMeterDisplayed) {
            mEvadeLevel *= 0.93f;
            if (mEvadeLevel < 0.05f) {
                mEvadeLevel = 0.05f;
            }

            if (mPursuitMeterModeTimer > 2.5f) {
                mPursuitMeterModeTimer = 0.0f;
                mCoolDownMeterDisplayed = false;
                mPursuitStatus = PS_INITIAL_CHASE;
                mDoTestForHeliSearch = false;
            }
        } else {
            mEvadeLevel = 0.0f;
        }
    }

    mCoolDownTimeRemaining = UMath::Max(0.0f, mCoolDownTimeRequired - sumTimeElapsed);
    if (mCoolDownTimeRemaining > GetCoolDownTimeRequired()) {
        mCoolDownTimeRemaining = GetCoolDownTimeRequired();
    }

    if (mIsPerpBusted || mIsPursuitBailed) {
        mEvadeLevel = 0.0f;
    }

    return true;
}

bool AIPursuit::IsHeliInPursuit() const {
    for (IVehicle::List::const_iterator i = mIVehicleList.begin(); i != mIVehicleList.end(); ++i) {
        IVehicle *ivehicle = *i;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            return true;
        }
    }
    return false;
}

bool AIPursuit::ShouldEnd() const {
    if (!mTarget->IsValid() ||    //
        mEvadeLevel >= 1.0f ||    //
        mPursuitStatus == PS_EVADED || ((mIsPerpBusted || mIsPursuitBailed) && !GetNumCops())) {
        return true;
    }

    return false;
}

static UCrc32 heliHash1("copheli");

void AIPursuit::GetAdjustedCopCounts(CopCountRecord *counts, int &numcounts) {
    numcounts = 0;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetGlobalPursuitLevelAttrib();
    if (!pursuitLevelAttrib) {
        return;
    }

    int max_cops = 0x7ffffffd;
    bool is_player_pursuit = IsPlayerPursuit();

    if (!is_player_pursuit) {
        max_cops = 3;
        if (ICopMgr::Get()->IsPlayerPursuitActive()) {
            max_cops = 2;
        }
    }

    int min_cops = 0;
    int nominal_cops;
    int want_cops;

    if (mPursuitStatus == PS_COOL_DOWN) {
        Attrib::Gen::pursuitlevels *myLevelAttrib = GetPursuitLevelAttrib();
        min_cops = myLevelAttrib->NumPatrolCars();
        min_cops = bMin(min_cops, max_cops);
        max_cops = min_cops;
    } else {
        max_cops = bClamp(mNumCopsRequiredToEvade - mNumFullyEngagedCopsEvaded, 0, max_cops);
    }

    nominal_cops = 0;

    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);
        if (copcount.CopType.GetHash32() != heliHash1.GetValue()) {
            nominal_cops += copcount.Count;
        }
    }

    want_cops = bClamp(nominal_cops, min_cops, max_cops);

    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);
        int count;
        int adjustedcount;

        if (copcount.CopType.GetHash32() == heliHash1.GetValue()) {
            counts[numcounts] = copcount;
            numcounts++;
        } else {
            count = copcount.Count;
            adjustedcount = static_cast<int>(static_cast<float>(count * want_cops) / static_cast<float>(nominal_cops) + 0.5f);
            if (adjustedcount != 0) {
                counts[numcounts] = copcount;
                counts[numcounts].Count = adjustedcount;
                want_cops -= adjustedcount;
                nominal_cops -= count;
                numcounts++;
            }
        }
    }
}

void AIPursuit::RemoveUnwantedVehicles() {
    CopCountRecord counts[8];
    int numcounts;
    GetAdjustedCopCounts(counts, numcounts);

    UCrc32 fleeType = UCrc32::kNull;
    int remainingWanted = 0;
    int fleeCount = 0;

    for (ContingentVector::iterator iter = mCopContingent.begin(); iter != mCopContingent.end(); ++iter) {
        int extra = iter->mCount;
        for (int i = 0; i < numcounts; i++) {
            const CopCountRecord &copcount = counts[i];
            if (UCrc32(copcount.CopType) == iter->mType) {
                extra = iter->mCount - copcount.Count;
                break;
            }
        }
        if (extra > 0) {
            if (Sim::GetRandom()._SimRandom_FloatRange(1.0f) >=
                static_cast<float>(remainingWanted) / static_cast<float>(remainingWanted + extra)) {
                fleeType = iter->mType;
                fleeCount = extra;
                break;
            }
            remainingWanted += extra;
        }
    }

    if (fleeType != UCrc32::kNull) {
        FleeCopOfType(fleeType, bMin(fleeCount, 2));
    }

    if (!mIsPerpInSight && mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE && mGroundSupportRequest.mHeavySupport) {
        for (IVehicle::List::iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
            IVehicle *iv = *iter;
            if (IsSupportVehicle(iv)) {
                IPursuitAI *ipv;
                if (iv->QueryInterface(&ipv)) {
                    ipv->StartFlee();
                }
            }
        }
        mGroundSupportRequest.Reset();
    }
}

void AIPursuit::FleeCopOfType(UCrc32 type, int fleecount) {
    float d2 = 0.0f;
    float distance = 0.0f;
    IVehicle *furthest = nullptr;
    IVehicle *secondfurthest = nullptr;
    int num_can_see_you = 0;
    int already_fleeing = 0;
    UCrc32 fleegoal("AIGoalFleePursuit");

    for (IVehicle::List::iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            continue;
        }

        IPursuitAI *ipv;
        IVehicleAI *iai;

        if (ivehicle->QueryInterface(&ipv) && ipv->GetSupportGoal().GetValue() != 0) {
            continue;
        }
        if (ivehicle->IsDestroyed()) {
            continue;
        }
        if (ivehicle->QueryInterface(&iai) && iai->GetGoalName() == fleegoal) {
            already_fleeing++;
            continue;
        }

        bool can_see_you = ipv->GetTimeSinceTargetSeen() <= 0.0f;
        if (can_see_you) {
            num_can_see_you++;
        }

        if (UCrc32(ivehicle->GetVehicleName()) != type) {
            continue;
        }

        float dist = UMath::Distance(ivehicle->GetPosition(), mTarget->GetPosition());
        if (!can_see_you) {
            dist += 40.0f;
        }

        if (dist > distance || !furthest) {
            d2 = distance;
            secondfurthest = furthest;

            distance = dist;
            furthest = ivehicle;
        } else if (dist > d2) {
            d2 = dist;
            secondfurthest = ivehicle;
        }
    }

    IPursuitAI *ipursuitai;
    if (furthest && furthest->QueryInterface(&ipursuitai) && fleecount > already_fleeing) {
        bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;
        if (!can_see_you || num_can_see_you > 2) {
            if (can_see_you) {
                num_can_see_you--;
            }
            ipursuitai->StartFlee();
        }
    }

    if (secondfurthest && secondfurthest->QueryInterface(&ipursuitai) && fleecount > already_fleeing + 1) {
        bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;
        if (!can_see_you || num_can_see_you > 2) {
            ipursuitai->StartFlee();
        }
    }
}

const char *AIPursuit::CopRequest() {
    if (mIsPerpBusted || mSpawnCopTimer >= 0.0f || mIsPursuitBailed) {
        return nullptr;
    }

    Attrib::Gen::pursuitlevels *plevels = GetPursuitLevelAttrib();
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();

    bool allowHeli = mSpawnHeliTimer < 0.0f && ps && ps->MinimumSupportDelay() < mTotalPursuitTime;

    if (allowHeli && mDoTestForHeliSearch) {
        mDoTestForHeliSearch = false;

        if (!mIsPerpInSight && !HeliVehicleActive()) {
            float heliSearchChance = plevels->SearchModeHeliSpawnChance();
            float rand = Sim::GetRandom()._SimRandom_FloatRange(100.0f);
            if (rand <= heliSearchChance) {
                mForceHeliSpawnNext = true;

                SoundAI *copspeech = SoundAI::Get();
                if (copspeech && copspeech->GetHeli()) {
                    copspeech->GetHeli()->Quadrant();
                }
            }
        }
    }

    if (mForceHeliSpawnNext) {
        return "copheli";
    }

    CopCountRecord adjustedCounts[8];
    int numCopTypesToChooseFrom;
    GetAdjustedCopCounts(adjustedCounts, numCopTypesToChooseFrom);

    const char *request = nullptr;

    struct {
        unsigned int type;
        int needed;
        int weight;
    } currentlyActive[10];

    int totalNeeded = 0;
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        const CopCountRecord &copcount = adjustedCounts[i];
        currentlyActive[i].type = copcount.CopType.GetHash32();
        currentlyActive[i].needed = copcount.Count;

        for (ContingentVector::const_iterator j = mCopContingent.begin(); j != mCopContingent.end(); ++j) {
            if (currentlyActive[i].type == j->mType.GetValue()) {
                currentlyActive[i].needed = bMax(0, currentlyActive[i].needed - static_cast<int>(j->mCount));
                break;
            }
        }

        totalNeeded += currentlyActive[i].needed;
    }

    mNumCopsNeeded = totalNeeded;
    if (totalNeeded == 0) {
        return nullptr;
    }

    int totalWeight = 0;
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        const CopCountRecord &copcount = adjustedCounts[i];
        currentlyActive[i].weight = copcount.Chance != 0 ? copcount.Chance : 100;

        if (currentlyActive[i].type == heliHash1.GetValue() && !allowHeli) {
            currentlyActive[i].weight = 0;
        }
        if (currentlyActive[i].needed == 0) {
            currentlyActive[i].weight = 0;
        }

        totalWeight += currentlyActive[i].weight;
    }

    int rand = Sim::GetRandom()._SimRandom_IntRange(totalWeight);
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        rand -= currentlyActive[i].weight;
        if (rand < 0) {
            request = adjustedCounts[i].CopType.GetString();
            break;
        }
    }

    return request;
}

int AIPursuit::RequestRoadBlock() {
    if (mIsPerpBusted || mIsPursuitBailed || mRoadBlock) {
        return 0;
    }
    if (mRoadBlockTimer >= 0.0f) {
        return 0;
    }

    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();
    if (!ps) {
        return 0;
    }
    if (ps->MinimumSupportDelay() > mTotalPursuitTime) {
        return 0;
    }

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetPursuitLevelAttrib();
    if (!pursuitLevelAttrib) {
        return 0;
    }

    mRoadBlockTimer = Sim::GetRandom()._SimRandom_FloatRange(4.0f) + 8.0f;
    int rv = 0;
    if (mNextRoadblockRequest) {
        rv = 4;
    }

    float probability;
    if (IsPerpInSight() == true) {
        probability = pursuitLevelAttrib->roadblockprobability();
    } else {
        float radius = pursuitLevelAttrib->SearchModeRoadblockRadius();
        float d = UMath::Distance(mLastKnownLocation, mTarget->GetPosition());
        probability = pursuitLevelAttrib->SearchModeRoadblockChance();
        probability = probability * (radius - d) / radius;
    }

    float simProb = Sim::GetRandom()._SimRandom_FloatRange(100.0f);
    if (simProb >= probability) {
        mNextRoadblockRequest = false;
    } else {
        mNextRoadblockRequest = true;
    }
    return rv;
}

void AIPursuit::AddRoadBlock(IRoadBlock *roadblock) {
    // 3237
    mRoadBlock = roadblock;
    Attach(roadblock);

    mNumRBCopsAdded = 0;

    if (mActiveFormation != FOLLOW) {
        if (!IsFinisherActive()) {
            EndCurrentFormation();
        }
    }
}

void AIPursuit::ClearGroundSupportRequest() {
    mGroundSupportRequest.Reset();
}

bool AIPursuit::SkidHitEnabled() const {
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();

    for (int i = 0; i < (int)ps->Num_AirSupportOptions(); i++) {
        const AirSupport &as = ps->AirSupportOptions(i);
        if (as.HeliStrategy == SKID_HIT) {
            return true;
        }
    }
    return false;
}

GroundSupportRequest *AIPursuit::RequestGroundSupport() {
    if (mIsPerpBusted || !mIsPerpInSight || mIsPursuitBailed) {
        return nullptr;
    }

    if (mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::NOT_ACTIVE) {
        return &mGroundSupportRequest;
    }

    if (mSupportCheckTimer < 0.0f) {
        mSupportCheckTimer = 10.0f;

        Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();
        if (ps->MinimumSupportDelay() > mTotalPursuitTime) {
            return nullptr;
        }

        int rand = Sim::GetRandom()._SimRandom_IntRange(100);

        if (!mSupportPriorityCheckDone) {
            for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); i++) {
                const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);
                if (leaderSupport.PriorityTime < mTotalPursuitTime) {
                    mSupportPriorityCheckDone = true;
                    if (leaderSupport.PriorityChance > static_cast<unsigned int>(rand)) {
                        mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                        mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
                        mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                        break;
                    }
                }
            }
        }

        if (mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::PENDING) {
            rand = Sim::GetRandom()._SimRandom_IntRange(100);

            if (!mRoadBlock) {
                for (int i = 0; i < static_cast<int>(ps->Num_HeavySupportOptions()); i++) {
                    const HeavySupport &heavySupport = ps->HeavySupportOptions(i);
                    rand -= heavySupport.Chance;
                    if (rand < 0) {
                        mGroundSupportRequest.mHeavySupport = &heavySupport;
                        mGroundSupportRequest.mSupportTimer = heavySupport.Duration;
                        mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                        mRoadBlockTimer = 15.0f;
                        break;
                    }
                }
            }

            if (rand >= 0) {
                for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); i++) {
                    const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);
                    rand -= leaderSupport.Chance;
                    if (rand < 0) {
                        mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                        mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
                        mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                        break;
                    }
                }
            }
        }

        if (mGroundSupportRequest.mLeaderSupport && mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::PENDING &&
            mCrossState != CROSS_AVAILABLE) {
            mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::NOT_ACTIVE;
            mGroundSupportRequest.mLeaderSupport = nullptr;
        }

        if (rand < 0) {
            return &mGroundSupportRequest;
        }
    }
    return nullptr;
}

bool AIPursuit::IsSupportVehicle(IVehicle *iv) {
    IPursuitAI *ipv;
    if (!iv->QueryInterface(&ipv)) {
        return false;
    }
    return ipv->GetSupportGoal() != UCrc32((const char *)nullptr);
}

bool AIPursuit::IsTarget(AITarget *aitarget) const {
    return mTarget->IsTarget(aitarget);
}

AITarget *AIPursuit::GetTarget() const {
    return mTarget;
}

bool AIPursuit::IsFinisherActive() const {
    return mBreakerTimer >= 0.0f;
}

float AIPursuit::TimeToFinisherAttempt() const {
    return mFormation->GetTimeToFinisher() - mInFormationTimer;
}

void AIPursuit::BailPursuit() {
    mIsPursuitBailed = true;

    ICopMgr::Get()->PursuitIsEvaded(this);
}

float AIPursuit::TimeUntilBusted() const {
    float rv;

    if (this->mBustedTimer > 0.03f) {
        rv = UMath::Min(1.0f, this->mBustedTimer * 0.2f);
        rv = (1.0f - this->mPursuitMeter) * rv + this->mPursuitMeter;
    } else if (this->mEvadeLevel >= 0.05f) {
        rv = -1.0f;
    } else {
        rv = this->mPursuitMeter;
    }

    return rv;
}

bool AIPursuit::IsAttemptingRoadBlock() const {
    return mRoadBlock != nullptr;
}

void AIPursuit::NotifyCopDamaged(IVehicle *ivehicle) {
    if (mAllowStatsToAccumulate) {
        mNumCopsDamaged++;

        if (mRoadBlock && mRoadBlock->IsComprisedOf(ivehicle->GetSimable()->GetOwnerHandle())) {
            mRoadBlock->IncNumCopsDamaged();
        }

        GManager::Get().IncValue("cops_damaged");
    }
}

void AIPursuit::OnDebugDraw() {}

Attrib::Gen::pursuitlevels *GetGlobalPursuitLevelAttrib() {
    Attrib::Gen::pursuitlevels *result = nullptr;

    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_PLAYERS).begin(); iter != IVehicle::GetList(VEHICLE_PLAYERS).end(); ++iter) {
        IVehicle *ivehicle = *iter;
        IPerpetrator *iperp;
        IVehicleAI *iai;
        if (ivehicle->QueryInterface(&iperp) && ivehicle->QueryInterface(&iai)) {
            bool pursuing = iai->GetPursuit() != nullptr;
            if (result && !pursuing) {
                continue;
            }
            result = iperp->GetPursuitLevelAttrib();
            if (pursuing) {
                return result;
            }
        }
    }

    {
        for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_RACERS).begin(); iter != IVehicle::GetList(VEHICLE_RACERS).end(); ++iter) {
            IVehicle *ivehicle = *iter;
            DriverClass dc = ivehicle->GetDriverClass();
            if (dc == DRIVER_HUMAN || dc == DRIVER_REMOTE) {
                continue;
            }
            IPerpetrator *iperp;
            IVehicleAI *iai;
            if (ivehicle->QueryInterface(&iperp) && ivehicle->QueryInterface(&iai)) {
                bool pursuing = iai->GetPursuit() != nullptr;
                if (result && !pursuing) {
                    continue;
                }
                result = iperp->GetPursuitLevelAttrib();
                if (pursuing) {
                    return result;
                }
            }
        }
    }

    return result;
}

bool IsValidPursuitCarName(const char *name) {
    Attrib::Gen::pursuitlevels *pursuitlevels = GetGlobalPursuitLevelAttrib();
    if (!pursuitlevels) {
        return false;
    }

    UCrc32 nameHash(name);
    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);
        if (nameHash == UCrc32(copcount.CopType.GetString())) {
            return true;
        }
    }

    return false;
}

const char *GetRandomValidCopCar() {
    Attrib::Gen::pursuitlevels *pl = GetGlobalPursuitLevelAttrib();
    if (!pl) {
        return nullptr;
    }

    const char *result = nullptr;

    int total = 0;

    for (unsigned int i = 0; i < pl->Num_cops(); i++) {
        const CopCountRecord &rec = pl->cops(i);
        UCrc32 copType(rec.CopType.GetString());

        if (heliHash1.GetValue() != copType.GetValue()) {

            total += rec.Count;
        }
    }

    int roll = Sim::GetRandom()._SimRandom_IntRange(total);

    for (unsigned int i = 0; i < pl->Num_cops(); i++) {
        const CopCountRecord &rec = pl->cops(i);
        UCrc32 copType(rec.CopType.GetString());

        if (heliHash1.GetValue() != copType.GetValue()) {

            roll -= rec.Count;
            if (roll < 0) {

                result = rec.CopType.GetString();
                break;
            }
        }
    }

    return result;
}

void AIPursuit::SpikesHit(IVehicleAI *ivai) {
    if (!ivai) {
        return;
    }
    if (mNumRBCopsAdded) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        IRoadBlock *rb = GetRoadBlock();
        if (!rb) {
            continue;
        }
        float distxz;
        IVehicle *minDistVehicle = nullptr;
        rb->GetMinDistanceToTarget(0.0f, distxz, &minDistVehicle);

        if (minDistVehicle) {
            if (rb->RemoveVehicle(minDistVehicle)) {
                AddVehicle(minDistVehicle);
                mNumRBCopsAdded++;
            }
        }
    }
}

void AIPursuit::EndPursuitEnteringSafehouse() {
    mPursuitStatus = PS_EVADED;
    mEvadeLevel = 1.0f;
    mEnterSafehouseOnDestruct = true;
}

void AIPursuit::UpdateJerk(float dt) {
    if (!mTarget->IsValid()) {
        return;
    }

    float t = dt * 0.1f;

    UMath::Lerp(mJerkLagPosition, mTarget->GetPosition(), t, mJerkLagPosition);

    float dist = UMath::Distance(mTarget->GetPosition(), mJerkLagPosition);
    mJerkLagDistance = UMath::Lerp(mJerkLagDistance, dist, t);

    float speed = mTarget->GetSpeed();
    mJerkLagSpeed = UMath::Lerp(mJerkLagSpeed, speed, t * 0.5f);

    float ratio = mJerkLagDistance > 0.01f ? mJerkLagSpeed * 10.0f / mJerkLagDistance : 0.0f;

    if (mIsAJerk) {
        if (ratio <= 1.75f) {
            mIsAJerk = false;
        }
    } else {
        if (ratio >= 3.0f) {
            mIsAJerk = true;
        }
    }
}
