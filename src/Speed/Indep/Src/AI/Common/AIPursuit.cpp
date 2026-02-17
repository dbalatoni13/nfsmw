#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

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
    float finisher = pursuit->TimeToFinisherAttempt();
    float ftight = (tightness * 0.2f) + 0.2f;
    float scale = (finisher / GetTimeToFinisher() * ftight) + (1.0f - ftight);

    for (int i = 0; i < 4; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

RollingBlockFormation::RollingBlockFormation(int numCops, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr; // r29
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
    float finisher = pursuit->TimeToFinisherAttempt();
    float ftight = tightness * 0.4f;
    float scale = (finisher / GetTimeToFinisher() * ftight) + (1.0f - ftight);

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
    inline ~StaggerFollowFormation() override {}
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

// void HerdFormation::Update(float dT, IPursuit *pursuit) {}

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
      mTimeSinceSetupSpeech(0.0f),       //
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

    // TODO later after we have more Gameplay stuff decomped
    // flipped in ghidra
    if (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::IsFinalEpicPursuit()) {
            mBaseHeat = 6.0f;
            mMaximumHeat = 6.0f;
        } else {
        }

    } else {
    }

    mCurrentPursuitLevel = 0;
    mActiveFormationTime = 0.0f;
    mActiveFormation = STAGGER_FOLLOW;
    InitFormation(0);
    mSpawnCopTimer = 0.0f;
    mDoTestForHeliSearch = false;
    mSpawnHeliTimer = 10.0f;
    mForceHeliSpawnNext = false;
    mCopDestroyedBonusTimer = 0.0f;
    mMostRecentCopDestroyedRepPoints = 0;
    mCopDestroyedBonusMultiplier = 1;
    mSupportCheckTimer = 10.0f;
    mMostRecentCopDestroyedType = (const char *)nullptr;
    mCoolDownMeterDisplayed = false;
    mPursuitMeterModeTimer = 0.0f;
    mSupportPriorityCheckDone = false;
    mGroundSupportRequest.Reset();

    mJerkLagPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    mJerkLagDistance = 1000.0f;

    mNumRBCopsAdded = 0;
    mMinDistanceToTarget = 100000.0f;
    mIsAJerk = false;
}

AIPursuit::~AIPursuit() {
    DetachAll();
    RemoveTask(mSimulateTask);
    RemoveTask(mBustedTimerTask);

    delete mFormation;
    delete mTarget;

    // TODO is this in the destructor of GroundSupportRequest?
    mGroundSupportRequest.Reset();
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
                int perpHeat = iperp->GetHeat();
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
