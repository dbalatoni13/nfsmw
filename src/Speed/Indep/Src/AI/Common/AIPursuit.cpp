#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPursuitLength.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

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
    float scale = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = (tightness * 0.2f) + 0.2f;
    scale = scale * ftight + (1.0f - ftight);

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
    float scale = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = tightness * 0.4f;
    scale = scale * ftight + (1.0f - ftight);

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
#ifndef EA_PLATFORM_XENON
void AIPursuit::EvenOutOffsets(Vector3List &copRelativePositions, FormationTargetList &formationOffsets) {
    typedef UTL::Std::vector<PursuitFormation::TargetOffsetList::const_iterator, _type_AIPursuitEvenOutOffsetsSourceOffsets> SourceVector;

    const PursuitFormation::TargetOffsetList &offsetList = mFormation->GetTargetOffsets();

    SourceVector source_offsets;
    source_offsets.reserve(offsetList.size());

    for (PursuitFormation::TargetOffsetList::const_iterator i = offsetList.begin(); i != offsetList.end(); ++i) {
        source_offsets.push_back(i);
    }

    while (copRelativePositions.size() > formationOffsets.size() && formationOffsets.size() < mFormation->GetMaxCops()) {
        int bestPriority = 0;
        float bestDistance = 0.0f;
        PursuitFormation::TargetOffsetList::const_iterator *bestOffset = source_offsets.end();

        for (PursuitFormation::TargetOffsetList::const_iterator *i = source_offsets.begin(); i != source_offsets.end(); ++i) {
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
        // TODO how on xenon? it's const..
        *bestOffset = 0;
    }
}
#endif

DECLARE_CONTAINER_TYPE(AIPursuitAssignClosestOffsetsDistances);
DECLARE_CONTAINER_TYPE(AIPursuitAssignClosestOffsetsMaximums);

// Functionally matching ig? dwarf is matching, some issues with a loop
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
        if (currentCop < 0) {
            continue;
        }

        currentOffset = -1;
        // TODO issue with this loop
        for (int j = 0; j < numCols; ++j) {
            float distance = copOffsetDistance[currentCop * numCols + j];
            if (distance != INDEX_ASSIGNED && distance == furthestDistance) {
                currentOffset = j;
                break;
            }
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
            if (f > s) {
                return s / f;
            } else {
                return 2.0f - f / s;
            }
        } else {
            // TODO
            if (f > -s) {
                return -2.0f - f / s;
            } else {
                return s / f;
            }
        }
    } else {
        if (s > 0.0f) {
            if (-f > s) {
                return s / f + 4.0f;
            } else {
                return 2.0f - f / s;
            }
        } else {
            if (-f > -s) {
                return s / f + -4.0f;
            } else {
                return -2.0f - f / s;
            }
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

    AITarget *target = GetTarget(); // r30

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

    Pursuers::const_iterator pursuitIter; // r28
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

    int inneroffset = 0;             // r31
    int numinner = copangles.size(); // r29
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

    // TODO weird
    for (int i = 0; i < num; i++) {
        float angle = i * (6.283185f / num); // TODO different M_TWOPI constant...
        float c = UMath::Cosr(angle);
        float s = UMath::Sinr(angle);
        unsigned int index = (i + frontmostCop) % num;

        copangles[index].cop->SetInPositionOffset(UMath::Vector3Make(s * radius, 0.0f, c * radius));
        copangles[index].cop->SetInPositionGoal(kPullOverGoal);
        copangles[index].cop->DoInPositionGoal();
    }
}

void AIPursuit::UpdateFormation(float dT) {
    if (!mTarget->IsValid()) {
        return;
    }

    IVehicleAI *targetvehicleai;
    if (!mTarget->QueryInterface(&targetvehicleai)) {
        return;
    }

    IRigidBody *itargetRB;
    if (!mTarget->QueryInterface(&itargetRB)) {
        return;
    }

    mFormation->Update(dT, this);

    Pursuers assignCopList;
    Vector3List copRelativePositions;
    assignCopList.reserve(mIVehicleList.size());
    copRelativePositions.reserve(mIVehicleList.size());

    float grossDistanceToTarget = UMath::Distance(mTarget->GetPosition(), targetvehicleai->GetCurrentRoad()->GetPosition());
    float formationCandidateLimit = 60.0f;

    for (IVehicle *const *vehicleIter = mIVehicleList.begin(); vehicleIter != mIVehicleList.end(); vehicleIter++) {
        IVehicle *ivehicle = *vehicleIter;
        IPursuitAI *ipv;
        if (!ivehicle->QueryInterface(&ipv)) {
            continue;
        }
        if (IsSupportVehicle(ivehicle)) {
            continue;
        }

        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;
        ISimable *simable = ivehicle->GetSimable();
        UMath::Vector3 targetRelativePos = simable->GetPosition();
        UMath::Sub(targetRelativePos, mTarget->GetPosition(), targetRelativePos);

        ipv->SetInFormation(false);

        if (bIsChopper) {
            if (!mIsPerpBusted && !mIsPursuitBailed) {
                AssignChopperGoal(ipv);
            }
        } else {
            IVehicleAI *ivai = ivehicle->GetAIVehiclePtr();
            if (ivai->GetDrivableToTargetPos()) {
                float dist = UMath::Length(targetRelativePos);
                if (dist <= grossDistanceToTarget + formationCandidateLimit) {
                    itargetRB->ConvertWorldToLocal(targetRelativePos, false);
                    assignCopList.push_back(ipv);
                    copRelativePositions.push_back(targetRelativePos);
                }
            }
        }
    }

    FormationTargetList formationOffsets;
    formationOffsets.reserve(mFormation->GetTargetOffsets().size());

    EvenOutOffsets(copRelativePositions, formationOffsets);

    if (copRelativePositions.size() != 0 && formationOffsets.size() != 0) {
        AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, true);
    }

    int i = 0;
    int countInFormation = 0;
    unsigned int countInPosition = 0;
    UpdateOutOfFormationOffsets();

    float averageDistanceToTarget = 0.0f;
    for (IPursuitAI *const *pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); pursuitIter++, i++) {
        IPursuitAI *ipv = *pursuitIter;
        if (!ipv->GetInFormation()) {
            ipv->SetInPosition(false);
        } else {
            countInFormation++;
            UMath::Vector3 copRelativePosition = copRelativePositions[i];
            UMath::Vector3 copOffset = ipv->GetPursuitOffset();
            float distanceToTarget = UMath::Distancexz(copOffset, copRelativePosition);
            if (distanceToTarget < 4.0f) {
                averageDistanceToTarget += distanceToTarget;
                countInPosition++;
                ipv->SetInPosition(true);
            } else {
                ipv->SetInPosition(false);
            }
        }
    }

    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    if (mTarget->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    float collapsespeed = KPH2MPS(pursuitLevelAttrib->CollapseSpeed());
    if (mIsAJerk) {
        collapsespeed = KPH2MPS(125.0f);
    }

    if (mBreakerTimer >= 0.0f) {
        float finisherTime = mFormation->GetFinisherTime();
        if (mBreakerTimer < finisherTime && !mIsPerpBusted && !mIsPursuitBailed) {
            mBreakerTimer += dT;
            goto finisherCheck;
        }
    }

    // collapseCheck
    if (pursuitLevelAttrib == nullptr || mTarget->GetSpeed() >= collapsespeed || countInFormation < 1) {
        goto noCollapse;
    }

    if (mIsPerpBusted) {
        mInFormationTimer = 0.0f;
    } else if (mIsPerpInSight && !mIsPursuitBailed) {
        int maxCopsCollapsing = pursuitLevelAttrib->MaxCopsCollapsing(0);
        float innerRadius = static_cast<float>(pursuitLevelAttrib->CollapseInnerRadius(0));
        float outerRadius = pursuitLevelAttrib->CollapseOuterRadius();
        mCollapseActive = SetupCollapse(assignCopList, maxCopsCollapsing, innerRadius, outerRadius);
        if (mCollapseActive && mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE && mGroundSupportRequest.mHeavySupport != nullptr) {
            for (IVehicle *const *iter = mIVehicleList.begin(); iter != mIVehicleList.end(); iter++) {
                IVehicle *ivehicle = *iter;
                if (IsSupportVehicle(ivehicle)) {
                    IPursuitAI *ipursuitai;
                    if (ivehicle->QueryInterface(&ipursuitai)) {
                        ipursuitai->StartFlee();
                    }
                }
            }
            mGroundSupportRequest.Reset();
        }
    } else {
        goto noCollapse;
    }
    goto cleanup;

noCollapse: {
    if (!mIsPerpBusted && (mBreakerTimer >= 0.0f || mCollapseActive)) {
        mCollapseActive = false;
        float zero = 0.0f;
        mInFormationTimer = zero;
        mBreakerTimer = -1.0f;
        for (IPursuitAI *const *pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); pursuitIter++) {
            IPursuitAI *ipv = *pursuitIter;
            if (ipv != nullptr) {
                ipv->SetInPositionGoal(UCrc32::kNull);
                ipv->StartPursuit(mTarget, nullptr);
            }
        }
        for (IVehicle *const *iter = mIVehicleList.begin(); iter != mIVehicleList.end(); iter++) {
            IVehicle *ivehicle = *iter;
            IPursuitAI *ipv;
            if (!ivehicle->QueryInterface(&ipv)) {
                continue;
            }
            IVehicleAI *ivai = ivehicle->GetAIVehiclePtr();
            UCrc32 goal = ivai->GetGoalName();
            UCrc32 inpositiongoal = ipv->GetInPositionGoal();
            if (goal == inpositiongoal || goal == kPullOverGoal) {
                ipv->SetInPositionGoal(UCrc32::kNull);
                ipv->StartPursuit(mTarget, nullptr);
            }
        }
    }
    goto cleanup;
}

finisherCheck: {
    PursuitFormation *pf = mFormation;
    if (!pf->GetHasFinisher() || countInPosition == 0 || mCollapseActive || countInFormation != 0 || mIsPursuitBailed) {
        mInFormationTimer = 0.0f;
    } else {
        float disttolerance = mFormation->GetFinisherTolerance();
        float averageDist = static_cast<float>(countInPosition);
        float formationrate = disttolerance * 4.0f;
        formationrate = ((formationrate + formationrate) - averageDistanceToTarget / averageDist) / formationrate;
        formationrate = bClamp(formationrate, -1.0f, 1.0f);
        float newTimer = mInFormationTimer + formationrate * dT;
        mInFormationTimer = newTimer;
        if (newTimer < 0.0f) {
            mInFormationTimer = 0.0f;
        }
        float timeToFinisher = mFormation->GetTimeToFinisher();
        if (mInFormationTimer < timeToFinisher) {
            goto cleanup;
        }
        if (countInPosition < mFormation->GetMinFinisherCops()) {
            mInFormationTimer = mFormation->GetTimeToFinisher() - 0.01f;
        } else {
            mBreakerTimer = 0.0f;
            for (IPursuitAI *const *pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); pursuitIter++) {
                IPursuitAI *ipv = *pursuitIter;
                if (ipv != nullptr && ipv->GetInFormation() && ipv->GetInPositionGoal() != UCrc32::kNull) {
                    ipv->DoInPositionGoal();
                }
            }
        }
    }
}

cleanup:
    ;
}

extern float kBustedHUDTime;

bool AIPursuit::OnTask(HSIMTASK htask, float dT) {
    // Busted timer task handler
    if (htask == mBustedTimerTask) {
        float timer = mBustedTimer + mBustedIncrement;
        mBustedTimer = timer;
        if (timer < 0.0f) {
            mBustedTimer = 0.0f;
        }
        return true;
    }

    if (htask != mSimulateTask || !mTarget->IsValid()) {
        return true;
    }

    // mAllowStatsToAccumulate
    bool allowStats = false;
    if (!GRaceStatus::Exists() ||
        GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing ||
        (GRaceStatus::Get().GetRaceParameters() != nullptr &&
         GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
        allowStats = true;
    }
    mAllowStatsToAccumulate = allowStats;

    UpdateJerk(dT);
    UpdateFormation(dT);

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    float pursuitTimeBeforeUpdate = mTotalPursuitTime;
    bool is_player_perp = IsPlayerPursuit();

    IPerpetrator *iperp = nullptr;
    bool hasPerp = mTarget->QueryInterface(&iperp);

    if (iperp) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();

        if (mNumCopsRequiredToEvade == 0) {
            LockInPursuitAttribs();
        }

        ePursuitStatus status = GetPursuitStatus();

        if (status != PS_COOL_DOWN) {
            if (mAllowStatsToAccumulate) {
                mTotalPursuitTime += dT;
                GManager::Get().TrackValue("total_pursuit_time", mTotalPursuitTime);
            }

            if (static_cast<int>(pursuitTimeBeforeUpdate) != static_cast<int>(mTotalPursuitTime)) {
                HSIMABLE perpHandle = mTarget->GetSimable()->GetOwnerHandle();
                MNotifyPursuitLength msg(perpHandle, mTotalPursuitTime);
                UCrc32 channel(0x20d60dbf);
                msg.Post(channel);
            }

            float heat = iperp->GetHeat();

            if (pursuitLevelAttrib) {
                float timePerHeatLevel = pursuitLevelAttrib->TimePerHeatLevel();
                mCoolDownTimeRequired = pursuitLevelAttrib->evadetimeout();

                if (FEDatabase->CurrentUserProfiles[0] != nullptr) {
                    bool isChallenge = false;
                    if (GRaceStatus::Exists()) {
                        GRace::Type raceType;
                        if (GRaceStatus::Get().GetRaceParameters() == nullptr) {
                            raceType = static_cast<GRace::Type>(-1);
                        } else {
                            raceType = GRaceStatus::Get().GetRaceParameters()->GetRaceType();
                        }
                        if (raceType == GRace::kRaceType_Challenge) {
                            isChallenge = true;
                        }
                    }

                    unsigned char bucket;
                    if (isChallenge) {
                        bucket = 14;
                    } else {
                        bucket = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    }
                    if (bucket > 14) {
                        bucket = 14;
                    }
                    timePerHeatLevel = timePerHeatLevel * pursuitLevelAttrib->ScaleEscalationPerBucket(bucket);
                }

                heat = heat + dT / timePerHeatLevel;
            }

            heat = bClamp(heat, mBaseHeat, mMaximumHeat);

            iperp->SetHeat(heat);

            heat = iperp->GetHeat();
            int newLevel = static_cast<int>(heat);
            if (newLevel != mCurrentPursuitLevel) {
                mCurrentPursuitLevel = newLevel;
                mActiveFormationTime = 0.0f;
                mSupportPriorityCheckDone = false;
                pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
                mRepPointsPerMinute = pursuitLevelAttrib->RepPointsPerMinute();
            }
        }

        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    // Timer decrements
    mRoadBlockTimer -= dT;
    mActiveFormationTime -= dT;
    mSpawnCopTimer -= dT;
    mSpawnHeliTimer -= dT;
    mSupportCheckTimer -= dT;
    mCopDestroyedBonusTimer -= dT;
    mPursuitMeterModeTimer += dT;

    mGroundSupportRequest.Update(dT);

    // SoundAI speech check
    if (SoundAI::Exists() && SoundAI::Get()->GetFocus() == 1) {
        mTimeSinceSetupSpeech = WorldTimer;
    }

    // Speech timer
    bool pursuitRace = false;
    float t_speech_finished = (WorldTimer - mTimeSinceSetupSpeech).GetSeconds();
    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    bool speech_finished;
    if (!pursuitRace && IsSpeechEnabled) {
        speech_finished = 5.0f < t_speech_finished;
    } else {
        speech_finished = true;
    }

    bool hasPursuitLevelAttrib = pursuitLevelAttrib != nullptr;

    // Formation selection
    if (mActiveFormationTime <= 0.0f && hasPursuitLevelAttrib &&
        !IsFinisherActive() &&
        !mIsPerpBusted && !mIsPursuitBailed && speech_finished) {

        int numFormations = pursuitLevelAttrib->Num_CopFormations();

        if (numFormations > 0) {
            float staggerDuration = pursuitLevelAttrib->StaggerFormationTime(0);
            FormationType formationType = STAGGER_FOLLOW;

            bool doRandom;
            if (mRoadBlock == nullptr ||
                mRoadBlock->GetDodged() ||
                mRoadBlock->GetNumCopsDamaged() ||
                mRoadBlock->GetNumCopsDestroyed()) {
                if ((mFormationAttemptCount & 1) != 0) {
                    // Accumulate total frequency
                    float totalFreq = 0.0f;
                    for (int i = 0; i < numFormations; i++) {
                        const CopFormationRecord &rec = pursuitLevelAttrib->CopFormations(i);
                        totalFreq += rec.Frequency;
                    }

                    // Weighted random selection
                    float randVal = Sim::GetRandom()._SimRandom_FloatRange(totalFreq);

                    for (int i = 0; i < numFormations; i++) {
                        const CopFormationRecord &rec = pursuitLevelAttrib->CopFormations(i);
                        randVal -= rec.Frequency;
                        if (randVal <= 0.0f) {
                            staggerDuration = rec.Duration;
                            formationType = rec.Formation;
                            break;
                        }
                    }
                }
                doRandom = true;
            } else {
                formationType = FOLLOW;
                doRandom = false;
            }

            mFormationAttemptCount++;

            if (formationType != mActiveFormation) {
                mActiveFormation = formationType;
                int numCops = GetNumCops();
                InitFormation(numCops);
            }

            float staggerRandom = Sim::GetRandom()._SimRandom_FloatRange(2.0f);
            mActiveFormationTime = staggerRandom + staggerDuration;
        }
    }

    RemoveUnwantedVehicles();

    // Vehicle iteration
    float MinDistanceToTarget3 = 999999.0f;
    mTimeSinceAnyCopSawPerp += dT;
    float heightThreshold = 50.0f;
    float engageRadius = 100.0f;
    if (hasPursuitLevelAttrib) {
        engageRadius = pursuitLevelAttrib->FullEngagementRadius();
    }

    int numVehiclesInRadius = 0;
    IVehicle **vehicles = mIVehicleList.begin();
    float MinDistanceToTargetxz = MinDistanceToTarget3;

    for (IVehicle **it = vehicles; it != vehicles + mIVehicleList.size(); it++) {
        IVehicle *vehicle = *it;

        if (!vehicle->IsActive()) {
            vehicles = mIVehicleList.begin();
            continue;
        }
        if (vehicle->IsDestroyed()) {
            vehicles = mIVehicleList.begin();
            continue;
        }

        IPursuitAI *pursuitAI = nullptr;
        vehicle->QueryInterface(&pursuitAI);

        if (pursuitAI) {
            float timeSinceTargetSeen = pursuitAI->GetTimeSinceTargetSeen();
            if (timeSinceTargetSeen < mTimeSinceAnyCopSawPerp) {
                mTimeSinceAnyCopSawPerp = timeSinceTargetSeen;
            }
        }

        const UMath::Vector3 &vehiclePos = vehicle->GetPosition();
        float vehicleY = vehiclePos.y;
        float targetY = mTarget->GetPosition().y;

        const UMath::Vector3 &vehiclePos2 = vehicle->GetPosition();

        UMath::Vector3 diff;
        UMath::Sub(vehiclePos2, mTarget->GetPosition(), diff);
        float dist3d = VU0_sqrt(UMath::LengthSquare(diff));

        const UMath::Vector3 *vehiclePos3 = &vehicle->GetPosition();
        float dxz_x = vehiclePos3->x - mTarget->GetPosition().x;
        float dxz_z = vehiclePos3->z - mTarget->GetPosition().z;
        float distXZ = VU0_sqrt(dxz_x * dxz_x + dxz_z * dxz_z);

        if (bAbs(vehicleY - targetY) < heightThreshold && distXZ < MinDistanceToTargetxz) {
            MinDistanceToTargetxz = distXZ;
        }

        if (dist3d < MinDistanceToTarget3) {
            MinDistanceToTarget3 = dist3d;
        }

        if (dist3d < engageRadius) {
            if (!IsSupportVehicle(vehicle)) {
                numVehiclesInRadius++;
            }
            if (pursuitAI && mPursuitStatus != PS_COOL_DOWN) {
                pursuitAI->SetWithinEngagementRadius();
            }
        }

        vehicles = mIVehicleList.begin();
    }

    mNumCopsFullyEngaged = numVehiclesInRadius;

    // Cop count / evade logic
    int remainingCopsToEvade = mNumCopsRequiredToEvade - mNumFullyEngagedCopsEvaded;
    if (remainingCopsToEvade < 1) {
        remainingCopsToEvade++;
        mNumCopsRequiredToEvade++;
    }

    int dif = mNumCopsFullyEngaged - remainingCopsToEvade;
    if (dif > 0) {
        mNumCopsRequiredToEvade += dif;
    }

    if (mNumCopsRequiredToEvade != 0 && mPursuitStatus == PS_INITIAL_CHASE &&
        remainingCopsToEvade <= mNumCopsToTriggerBackupTime) {
        mPursuitStatus = PS_BACKUP_REQUESTED;
        mBackupCountdownTimer = pursuitLevelAttrib->BackupCallTimer();
    }

    // Epic pursuit race check
    if (GRaceStatus::IsFinalEpicPursuit()) {
        mTimeSinceAnyCopSawPerp = 0.0f;
    }

    // Roadblock handling
    if (mRoadBlock == nullptr) {
        mNearestCopInRoadblock = nullptr;
        mDistanceToNearestCopInRoadblock = 0.0f;
    } else {
        if (!iperp->IsHiddenFromCars()) {
            float distxz;
            float rbDist = mRoadBlock->GetMinDistanceToTarget(dT, distxz, &mNearestCopInRoadblock);
            if (rbDist < MinDistanceToTarget3 && rbDist < 100.0f) {
                mIsPerpInSight = true;
                mTimeSinceAnyCopSawPerp = 0.0f;
            }
            if (distxz < MinDistanceToTargetxz) {
                MinDistanceToTargetxz = distxz;
            }
            mDistanceToNearestCopInRoadblock = rbDist;
        }

        if (mRoadBlock->IsPerpCheating() && mNumRBCopsAdded == 0 &&
            mNearestCopInRoadblock != nullptr && mRoadBlock->GetDodged()) {
            AddVehicle(mNearestCopInRoadblock);
            mNumCopsRequiredToEvade++;
            mNumRBCopsAdded++;
        }
    }

    mMinDistanceToTarget = MinDistanceToTarget3;

    // Busted speed limit
    float bustedSpeedLimit = KPH2MPS(pursuitLevelAttrib->BustSpeed(0));

    float bustedIncrement;
    if (!mIsPerpBusted && mIsPerpInSight && !mIsPursuitBailed) {
        // Check if target is in IRBVehicle with manual reset invulnerability
        bool isInvulnerable = false;
        IRBVehicle *irbVehicle = nullptr;
        mTarget->QueryInterface(&irbVehicle);

        if (irbVehicle && irbVehicle->GetInvulnerability() == INVULNERABLE_FROM_MANUAL_RESET) {
            isInvulnerable = true;
        }

        float closeDistance = KPH2MPS(5.0f);
        if (!isInvulnerable) {
            float targetSpeed = mTarget->GetSpeed();
            if (targetSpeed < bustedSpeedLimit) {
                goto speed_below_limit;
            }
            bustedIncrement = dT * 1.0f;
        } else {
            closeDistance = closeDistance * KPH2MPS(1.0f);
speed_below_limit:
            if (closeDistance <= MinDistanceToTargetxz) {
                bustedIncrement = dT * 1.0f;
            } else {
                float increment = dT * 0.0f;
                mBustedIncrement = increment;
                if (isInvulnerable) {
                    bustedIncrement = increment * 2.0f;
                    goto set_busted_increment;
                }
                goto after_busted_increment;
            }
        }
set_busted_increment:
        mBustedIncrement = bustedIncrement;
after_busted_increment:

        // INIS check - zero busted increment during world movement
        if (INIS::Exists() && INIS::Get()->IsWorldMomement()) {
            mBustedIncrement = 0.0f;
        }

        // Rep points per minute - check if minute boundary crossed
        int currentMinute = static_cast<int>(mTotalPursuitTime / 60.0f);
        if (static_cast<int>(pursuitTimeBeforeUpdate / 60.0f) != currentMinute) {
            iperp->AddToPendingRepPointsNormal(mRepPointsPerMinute);
        }

        // Backup countdown timer
        if (mPursuitStatus == PS_BACKUP_REQUESTED) {
            float newTimer = mBackupCountdownTimer - dT;
            mBackupCountdownTimer = newTimer;
            if (newTimer < 0.0f) {
                mPursuitStatus = PS_INITIAL_CHASE;
                EndPursuitEnteringSafehouse();
            }
        }
    } else {
        mBustedIncrement = dT * bustedIncrement;
    }

    // Busted check
    if (!mIsPerpBusted) {
        if (!mIsPursuitBailed && 1.0f < mBustedTimer) {
            mIsPerpBusted = true;
            mPursuitStatus = PS_BUSTED;

            if (!is_player_perp) {
                BailPursuit();

                GRacerInfo *racerInfo = nullptr;
                if (GRaceStatus::Exists()) {
                    racerInfo = GRaceStatus::Get().GetRacerInfo(mTarget->GetSimable());
                }
                if (racerInfo) {
                    racerInfo->Busted();
                    racerInfo->ForceStop();
                }

                HSIMABLE perpHandle = mTarget->GetSimable()->GetOwnerHandle();
                MPerpBusted msg(perpHandle);
                UCrc32 channel("MPerpBusted");
                msg.Post(channel);
            } else {
                // Player pursuit busted - iterate all vehicles
                IVehicle **vehBegin = mIVehicleList.begin();
                for (IVehicle **vit = vehBegin; vit != vehBegin + mIVehicleList.size(); vit++) {
                    IVehicle *veh = *vit;

                    if (!veh->IsActive()) {
                        vehBegin = mIVehicleList.begin();
                        continue;
                    }
                    if (veh->IsDestroyed()) {
                        vehBegin = mIVehicleList.begin();
                        continue;
                    }

                    IPursuitAI *copAI = UTL::COM::QueryInterface<IPursuitAI>(veh);
                    if (copAI) {
                        veh->GlareOff(LIGHT_COPS);
                        UCrc32 bustedGoal("busted");
                        copAI->SetInPositionGoal(bustedGoal);
                        copAI->DoInPositionGoal();
                    }

                    vehBegin = mIVehicleList.begin();
                }

                HSIMABLE perpHandle = mTarget->GetSimable()->GetOwnerHandle();
                MPerpBusted msg(perpHandle);
                UCrc32 channel(0x20d60dbf);
                msg.Post(channel);
            }
        }
    } else if (is_player_perp) {
        // Already busted - HUD timer
        float prevHUDTime = mBustedHUDTime;
        float newHUDTime = prevHUDTime + dT;
        mBustedHUDTime = newHUDTime;
        if (prevHUDTime <= kBustedHUDTime && kBustedHUDTime < newHUDTime) {
            HSIMABLE perpHandle = mTarget->GetSimable()->GetOwnerHandle();
            MPerpBusted msg(perpHandle);
            UCrc32 channel(0xfea34c0a);
            msg.Post(channel);
        }
    }

    // Bailed pursuit - all cops flee
    if (mIsPursuitBailed) {
        IVehicle **vehBegin = mIVehicleList.begin();
        for (IVehicle **vit = vehBegin; vit != vehBegin + mIVehicleList.size(); vit++) {
            IVehicle *veh = *vit;

            if (!veh->IsActive()) {
                vehBegin = mIVehicleList.begin();
                continue;
            }
            if (veh->IsDestroyed()) {
                vehBegin = mIVehicleList.begin();
                continue;
            }

            IPursuitAI *copAI = UTL::COM::QueryInterface<IPursuitAI>(veh);
            if (copAI) {
                copAI->StartFlee();
            }

            vehBegin = mIVehicleList.begin();
        }
    }

    // Update mIsPerpInSight
    mIsPerpInSight = mTimeSinceAnyCopSawPerp < 3.0f;

    // Hidden zone / cooldown handling
    if (iperp) {
        bool perpIsHidden = false;
        if (iperp->IsHiddenFromCars() || iperp->IsHiddenFromHelicopters()) {
            perpIsHidden = true;
        }

        if (perpIsHidden) {
            if (!mIsPerpInSight) {
                float multiplier = 2.0f;
                if (hasPursuitLevelAttrib) {
                    multiplier = pursuitLevelAttrib->HiddenZoneTimeMultiplier(0);
                }
                mHiddenZoneTime += dT * multiplier;
                goto cooldown_logic;
            }
        } else {
            if (!mIsPerpInSight) {
                goto cooldown_logic;
            }
        }

        // In sight - reset hidden zone and update last known location
        mHiddenZoneTime = 0.0f;
        mLastKnownLocation.x = mTarget->GetPosition().x;
        mLastKnownLocation.z = mTarget->GetPosition().z;
        mLastKnownLocation.y = mTarget->GetPosition().y;
    }

cooldown_logic:
    float sumTimeElapsed = mTimeSinceAnyCopSawPerp + mHiddenZoneTime;

    if (mTimeSinceAnyCopSawPerp > 3.0f) {
        // Evade mode
        bool fullyEvaded = false;
        float ratio = sumTimeElapsed / mCoolDownTimeRequired;
        mPursuitMeter = -1.0f;
        mEvadeLevel = ratio;
        float meterThreshold = 0.5f;

        if (!mCoolDownMeterDisplayed) {
            mEvadeLevel = 0.0f;
            if (meterThreshold < mPursuitMeterModeTimer) {
                mPursuitStatus = PS_COOL_DOWN;
                mPursuitMeterModeTimer = 0.0f;
                mCoolDownMeterDisplayed = true;

                float timeBetweenCopSpawn = pursuitLevelAttrib->TimeBetweenCopSpawn();
                mBackupCountdownTimer = 0.0f;
                mDoTestForHeliSearch = true;

                if (mSpawnCopTimer - timeBetweenCopSpawn < 0.0f) {
                    timeBetweenCopSpawn = mSpawnCopTimer;
                }
                mSpawnCopTimer = timeBetweenCopSpawn;

                if (IsPlayerPursuit()) {
                    GInfractionManager::Get().ReportInfraction(GInfractionManager::kInfraction_Resist);
                }
            }
        } else {
            if (ratio < 0.25f) {
                mEvadeLevel = 0.25f;
            }
            if (1.0f <= ratio) {
                fullyEvaded = true;
            }
        }

        if (fullyEvaded) {
            mPursuitStatus = PS_EVADED;
            mEvadeLevel = 1.0f;

            if (IsPlayerPursuit() && ICopMgr::Exists()) {
                ICopMgr::Get()->LockoutCops(true);
            }
        }
    } else {
        if (mTimeSinceAnyCopSawPerp > 1.5f) {
            // Transition zone
            float scale = mTimeSinceAnyCopSawPerp * 0.5f;
            float meterVal = 1.0f - scale;
            float clampMin = -1.0f;
            if (meterVal - clampMin < 0.0f) {
                meterVal = clampMin;
            }
            if (1.0f - meterVal < 0.0f) {
                meterVal = 1.0f;
            }
            mPursuitMeter = meterVal;
        } else if (hasPursuitLevelAttrib) {
            // Active pursuit meter calculation
            float deadZoneBustedDist = pursuitLevelAttrib->MeterDeadZoneBustedDistance(0);
            float deadZoneEvadeDist = pursuitLevelAttrib->MeterDeadZoneEvadeDist();

            mPursuitMeter = 0.0f;

            if (deadZoneEvadeDist < MinDistanceToTarget3) {
                float frontLOSDist = pursuitLevelAttrib->frontLOSdistance();
                float meterDist = (MinDistanceToTarget3 - deadZoneEvadeDist) / (frontLOSDist - deadZoneEvadeDist) * -2.0f;
                float speedScale = 1.0f;
                float clampMin = -1.0f;

                if (1.0f - meterDist < 0.0f) {
                    meterDist = 1.0f;
                }

                mPursuitMeter = meterDist;
            } else if (MinDistanceToTarget3 < deadZoneBustedDist) {
                float targetSpeed = mTarget->GetSpeed();

                if (targetSpeed <= KPH2MPS(200.0f)) {
                    float distRatio = (deadZoneBustedDist - MinDistanceToTarget3) / (deadZoneBustedDist - 5.0f);
                    if (distRatio < 0.0f) {
                        distRatio = 0.0f;
                    }
                    float maxDist = 1.0f;
                    if (maxDist - distRatio < 0.0f) {
                        distRatio = maxDist;
                    }

                    float speedLimit = KPH2MPS(80.0f);
                    float speedTargetSpeed = mTarget->GetSpeed();
                    float speedRatio = (speedLimit - speedTargetSpeed) / (speedLimit - bustedSpeedLimit);
                    if (speedRatio < 0.0f) {
                        speedRatio = 0.0f;
                    }
                    if (maxDist - speedRatio < 0.0f) {
                        speedRatio = maxDist;
                    }

                    mPursuitMeter = (distRatio * 0.3f + speedRatio * 0.7f) * -2.0f + 1.0f / 60.0f;
                } else {
                    mPursuitMeter = 0.0f;
                }
            }
        }

        // Cooldown meter decay
        if (!mCoolDownMeterDisplayed) {
            mEvadeLevel = 0.0f;
        } else {
            float newLevel = mEvadeLevel * 0.95f;
            mEvadeLevel = newLevel;
            if (newLevel < 0.25f) {
                mEvadeLevel = 0.25f;
            }

            if (0.5f < mPursuitMeterModeTimer) {
                mDoTestForHeliSearch = false;
                mPursuitMeterModeTimer = 0.0f;
                mCoolDownMeterDisplayed = false;
                mPursuitStatus = PS_INITIAL_CHASE;
            }
        }
    }

    // Final cooldown time
    float cooldownRemaining = mCoolDownTimeRequired - sumTimeElapsed;
    if (cooldownRemaining < 0.0f) {
        cooldownRemaining = 0.0f;
    }
    mCoolDownTimeRemaining = cooldownRemaining;

    float virtCooldown = GetCoolDownTimeRemaining();
    if (virtCooldown < mCoolDownTimeRemaining) {
        mCoolDownTimeRemaining = GetCoolDownTimeRemaining();
    }

    if (mIsPerpBusted || mIsPursuitBailed) {
        mEvadeLevel = 0.0f;
    }

    return true;
}

bool AIPursuit::IsPlayerPursuit() const {
    return GetTarget() && GetTarget()->GetSimable() && GetTarget()->GetSimable()->GetPlayer();
}

AITarget *AIPursuit::GetTarget() const {
    return mTarget;
}

bool AIPursuit::IsTarget(AITarget *aitarget) const {
    return mTarget->IsTarget(aitarget);
}

bool AIPursuit::ContingentHasActiveCops() const {
    for (ContingentVector::const_iterator j = mCopContingent.begin(); j != mCopContingent.end(); ++j) {
        if (j->mCount != 0) {
            return true;
        }
    }
    return false;
}

bool AIPursuit::IsHeliInPursuit() const {
    for (IVehicle::List::const_iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            return true;
        }
    }
    return false;
}

bool AIPursuit::ShouldEnd() const {
    if (!mTarget->IsValid()) {
        return true;
    }
    if (mEvadeLevel >= 1.0f) {
        return true;
    }
    if (mPursuitStatus == PS_EVADED) {
        return true;
    }
    if (mIsPerpBusted || mIsPursuitBailed) {
        if (GetNumCops() == 0) {
            return true;
        }
    }
    return false;
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
    if (mBustedTimer > 0.03f) {
        float rv = UMath::Min(mBustedTimer * 0.2f, 1.0f);
        return (1.0f - mPursuitMeter) * rv + mPursuitMeter;
    }
    if (mEvadeLevel >= 0.05f) {
        return -1.0f;
    }
    return mPursuitMeter;
}

bool AIPursuit::IsAttemptingRoadBlock() const {
    return mRoadBlock != nullptr;
}

void AIPursuit::NotifyCopDamaged(IVehicle *ivehicle) {
    if (mAllowStatsToAccumulate) {
        mNumCopsDamaged++;
        if (mRoadBlock) {
            if (mRoadBlock->IsComprisedOf(ivehicle->GetSimable()->GetOwnerHandle())) {
                mRoadBlock->IncNumCopsDamaged();
            }
        }
        GManager::Get().IncValue("cops_damaged");
    }
}

void AIPursuit::OnDebugDraw() {
}

void AIPursuit::EndPursuitEnteringSafehouse() {
    mEvadeLevel = 1.0f;
    mPursuitStatus = PS_EVADED;
    mEnterSafehouseOnDestruct = true;
}

void AIPursuit::AddRoadBlock(IRoadBlock *roadblock) {
    mRoadBlock = roadblock;
    Attach(roadblock);
    mNumRBCopsAdded = 0;
    if (mActiveFormation != FOLLOW) {
        if (!IsHeliInPursuit()) {
            EndCurrentFormation();
        }
    }
}

bool AIPursuit::IsSupportVehicle(IVehicle *iv) {
    IPursuitAI *ipv;
    if (!iv->QueryInterface(&ipv)) {
        return false;
    }
    return ipv->GetSupportGoal() != static_cast< const char * >(nullptr);
}

void AIPursuit::ClearGroundSupportRequest() {
    mGroundSupportRequest.Reset();
}

bool AIPursuit::SkidHitEnabled() const {
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();
    for (int i = 0; i < static_cast<int>(ps->Num_AirSupportOptions()); i++) {
        const AirSupport &airSupport = ps->AirSupportOptions(i);
        if (airSupport.HeliStrategy == SKID_HIT) {
            return true;
        }
    }
    return false;
}

void AIPursuit::SpikesHit(IVehicleAI *ivai) {
    if (!ivai) {
        return;
    }
    if (mNumRBCopsAdded != 0) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        IRoadBlock *iroadblock = GetRoadBlock();
        if (iroadblock) {
            IVehicle *ivehicleNear = nullptr;
            float dummy;
            iroadblock->GetMinDistanceToTarget(0.0f, dummy, &ivehicleNear);
            if (ivehicleNear) {
                if (iroadblock->RemoveVehicle(ivehicleNear)) {
                    AddVehicle(ivehicleNear);
                    mNumRBCopsAdded++;
                }
            }
        }
    }
}

void AIPursuit::UpdateJerk(float dt) {
    if (mTarget->IsValid()) {
        float jerklerp = dt * 0.1f;
        const UMath::Vector3 &pos = mTarget->GetPosition();
        UMath::Lerp(mJerkLagPosition, pos, jerklerp, mJerkLagPosition);
        float distance = UMath::Distance(pos, mJerkLagPosition);
        mJerkLagDistance = UMath::Lerp(mJerkLagDistance, distance, jerklerp);
        float speed = mTarget->GetSpeed();
        mJerkLagSpeed = UMath::Lerp(mJerkLagSpeed, speed, jerklerp * 0.5f);
        float jerkfactor;
        if (mJerkLagDistance > 0.01f) {
            jerkfactor = (mJerkLagSpeed * 10.0f) / mJerkLagDistance;
        } else {
            jerkfactor = 0.0f;
        }
        if (mIsAJerk) {
            if (jerkfactor <= 1.75f) {
                mIsAJerk = false;
            }
        } else {
            if (jerkfactor >= 3.0f) {
                mIsAJerk = true;
            }
        }
    }
}

// TODO: RemoveUnwantedVehicles - complex function using GetAdjustedCopCounts, SimRandom, etc.
// TODO: FleeCopOfType - complex function iterating IVehicle list, tracking furthest cops

int AIPursuit::RequestRoadBlock() {
    if (mIsPerpBusted || mIsPursuitBailed || mRoadBlock) {
        return 0;
    }
    if (mRoadBlockTimer >= 30.0f) {
        return 0;
    }
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();
    if (!ps) {
        return 0;
    }
    if (ps->MinimumSupportDelay(0) > mTotalPursuitTime) {
        return 0;
    }
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetPursuitLevelAttrib();
    if (!pursuitLevelAttrib) {
        return 0;
    }
    float randVal = Sim::GetRandom()._SimRandom_FloatRange(1.0f);
    mRoadBlockTimer = randVal * 30.0f + 10.0f;
    int rv = 0;
    if (mNextRoadblockRequest) {
        rv = 4;
    }
    float probability;
    if (mPursuitStatus == PS_BACKUP_REQUESTED) {
        probability = pursuitLevelAttrib->roadblockprobability();
    } else {
        float radius = pursuitLevelAttrib->SearchModeRoadblockRadius();
        float d = UMath::Distance(mLastKnownLocation, mTarget->GetPosition());
        probability = (pursuitLevelAttrib->SearchModeRoadblockChance() * (radius - d)) / radius;
    }
    float simProb = Sim::GetRandom()._SimRandom_FloatRange(1.0f);
    mNextRoadblockRequest = simProb < probability;
    return rv;
}

GroundSupportRequest *AIPursuit::RequestGroundSupport() {
    if (mIsPerpBusted || !mIsPerpInSight || mIsPursuitBailed) {
        return nullptr;
    }
    if (mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::NOT_ACTIVE) {
        return &mGroundSupportRequest;
    }
    if (mSupportCheckTimer >= 5.0f) {
        return nullptr;
    }
    mSupportCheckTimer = 15.0f;
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();
    if (ps->MinimumSupportDelay(0) > mTotalPursuitTime) {
        return nullptr;
    }
    unsigned int rand = Sim::GetRandom()._SimRandom_IntRange(100);
    if (!mSupportPriorityCheckDone) {
        for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); i++) {
            const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);
            if (leaderSupport.PriorityTime < mTotalPursuitTime) {
                mSupportPriorityCheckDone = true;
                if (rand < leaderSupport.PriorityChance) {
                    mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                    mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
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
                if (static_cast<int>(rand) < 0) {
                    mGroundSupportRequest.mHeavySupport = &heavySupport;
                    mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    mGroundSupportRequest.mSupportTimer = heavySupport.Duration;
                    mRoadBlockTimer = 60.0f;
                    break;
                }
            }
        }
        if (static_cast<int>(rand) >= 0) {
            for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); i++) {
                const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);
                rand -= leaderSupport.Chance;
                if (static_cast<int>(rand) < 0) {
                    mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                    mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
                    break;
                }
            }
        }
    }
    if (mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::NOT_ACTIVE) {
        return &mGroundSupportRequest;
    }
    return nullptr;
}

static const UCrc32 heliHash1("copheli");

Attrib::Gen::pursuitlevels *GetGlobalPursuitLevelAttrib() {
    Attrib::Gen::pursuitlevels *pl = nullptr;
    for (IVehicle *const *iter = IVehicle::GetList(VEHICLE_PLAYERS).begin();
         iter != IVehicle::GetList(VEHICLE_PLAYERS).end(); ++iter) {
        IVehicle *itargetVehicle = *iter;
        IPerpetrator *iperp;
        IVehicleAI *ivehicleai;
        if (itargetVehicle->QueryInterface(&iperp) && itargetVehicle->QueryInterface(&ivehicleai)) {
            bool ispursued = iperp->IsBeingPursued();
            if (!pl || ispursued) {
                pl = iperp->GetPursuitLevelAttrib();
                if (ispursued) {
                    return pl;
                }
            }
        }
    }
    for (IVehicle *const *iter = IVehicle::GetList(VEHICLE_RACERS).begin();
         iter != IVehicle::GetList(VEHICLE_RACERS).end(); ++iter) {
        IVehicle *itargetVehicle = *iter;
        DriverClass driverclass = itargetVehicle->GetDriverClass();
        if (driverclass != DRIVER_HUMAN && driverclass != DRIVER_REMOTE) {
            IPerpetrator *iperp;
            IVehicleAI *ivehicleai;
            if (itargetVehicle->QueryInterface(&iperp) && itargetVehicle->QueryInterface(&ivehicleai)) {
                bool ispursued = iperp->IsBeingPursued();
                if (!pl || ispursued) {
                    pl = iperp->GetPursuitLevelAttrib();
                    if (ispursued) {
                        return pl;
                    }
                }
            }
        }
    }
    return pl;
}

bool IsValidPursuitCarName(const char *name) {
    Attrib::Gen::pursuitlevels *pursuitlevels = GetGlobalPursuitLevelAttrib();
    if (!pursuitlevels) {
        return false;
    }
    UCrc32 nameHash(name);
    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);
        if (UCrc32(copcount.CopType) == nameHash) {
            return true;
        }
    }
    return false;
}

const char *GetRandomValidCopCar() {
    Attrib::Gen::pursuitlevels *pursuitlevels = GetGlobalPursuitLevelAttrib();
    if (!pursuitlevels) {
        return nullptr;
    }
    int totalRequested = 0;
    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);
        if (heliHash1 != UCrc32(copcount.CopType)) {
            totalRequested += copcount.Count;
        }
    }
    int rand = Sim::GetRandom()._SimRandom_IntRange(totalRequested);
    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);
        if (heliHash1 != UCrc32(copcount.CopType)) {
            rand -= copcount.Count;
            if (rand < 0) {
                return copcount.CopType.GetString();
            }
        }
    }
    return nullptr;
}

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

    int nominal_cops;
    int min_cops;
    if (mPursuitStatus == PS_COOL_DOWN) {
        Attrib::Gen::pursuitlevels *myLevelAttrib = GetPursuitLevelAttrib();
        nominal_cops = bMin(myLevelAttrib->NumPatrolCars(), max_cops);
    } else {
        nominal_cops = mNumCopsRequiredToEvade - mNumFullyEngagedCopsEvaded;
        if (nominal_cops < 0) {
            nominal_cops = 0;
        }
        if (max_cops < nominal_cops) {
            nominal_cops = max_cops;
        }
    }

    min_cops = 0;
    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);
        if (copcount.CopType.GetHash32() != heliHash1.GetValue()) {
            min_cops += copcount.Count;
        }
    }

    if (min_cops > nominal_cops) {
        nominal_cops = min_cops;
    }
    if (nominal_cops > max_cops) {
        nominal_cops = max_cops;
    }

    int want_cops = nominal_cops;
    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);
        if (copcount.CopType.GetHash32() != heliHash1.GetValue()) {
            int count = copcount.Count;
            int adjustedcount = count;
            if (!is_player_pursuit && count > 0) {
                adjustedcount = bMax(1, count / 2);
            }
            adjustedcount = bMin(adjustedcount, want_cops);
            if (adjustedcount > 0) {
                want_cops -= adjustedcount;
                counts[numcounts] = copcount;
                counts[numcounts].Count = adjustedcount;
                numcounts++;
            }
        }
    }
}

void AIPursuit::FleeCopOfType(UCrc32 type, int fleecount) {
    UCrc32 fleegoal("AIGoalFlee");
    IVehicle *furthest = nullptr;
    IVehicle *secondfurthest = nullptr;
    int num_can_see_you = 0;
    int already_fleeing = 0;
    float d2 = 0.0f;
    float distance = 0.0f;

    IVehicle *const *iter = mIVehicleList.begin();
    for (; iter != mIVehicleList.end(); iter++) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            continue;
        }
        IPursuitAI *ipursuitai;
        if (ivehicle->QueryInterface(&ipursuitai) && ipursuitai->GetInPursuit()) {
            continue;
        }
        IVehicleAI *iai;
        if (ivehicle->QueryInterface(&iai) && iai->GetGoalName() == fleegoal) {
            already_fleeing++;
            continue;
        }
        float dist = ipursuitai->GetTimeSinceTargetSeen();
        bool can_see_you = dist <= 0.0f;
        if (can_see_you) {
            num_can_see_you++;
        }
        UCrc32 vehicleTypeName(ivehicle->GetVehicleName());
        if (vehicleTypeName == type) {
            float distToTarget = UMath::Distance(ivehicle->GetPosition(), mTarget->GetPosition());
            if (!can_see_you) {
                distToTarget += 40.0f;
            }
            if (distToTarget > distance) {
                secondfurthest = furthest;
                d2 = distance;
                furthest = ivehicle;
                distance = distToTarget;
            } else if (furthest && distToTarget > d2) {
                secondfurthest = ivehicle;
                d2 = distToTarget;
            }
        }
    }

    if (furthest) {
        IPursuitAI *ipursuitai;
        if (furthest->QueryInterface(&ipursuitai) && already_fleeing < fleecount) {
            bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;
            if (!can_see_you || num_can_see_you > 2) {
                if (can_see_you) {
                    num_can_see_you--;
                }
                ipursuitai->EndPursuit();
            }
        }
    }

    if (secondfurthest) {
        IPursuitAI *ipursuitai;
        if (secondfurthest->QueryInterface(&ipursuitai) && already_fleeing + 1 < fleecount) {
            bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;
            if (!can_see_you || num_can_see_you > 2) {
                ipursuitai->EndPursuit();
            }
        }
    }
}

void AIPursuit::RemoveUnwantedVehicles() {
    CopCountRecord adjustedCounts[8];
    int numAdjustedCounts;
    GetAdjustedCopCounts(adjustedCounts, numAdjustedCounts);

    int typecount = 0;
    int fleecount = 0;
    UCrc32 fleetype = UCrc32::kNull;

    const CopContingent *j = mCopContingent.begin();
    for (; j != mCopContingent.end(); j++) {
        int num_this_type_to_flee = j->mCount;
        for (int i = 0; i < numAdjustedCounts; i++) {
            const CopCountRecord &copcount = adjustedCounts[i];
            if (UCrc32(copcount.CopType) == j->mType) {
                num_this_type_to_flee = j->mCount - copcount.Count;
                break;
            }
        }
        if (num_this_type_to_flee < 1) {
            continue;
        }
        typecount += num_this_type_to_flee;
        float prob = static_cast<float>(fleecount) / static_cast<float>(typecount);
        if (prob <= Sim::GetRandom()._SimRandom_Float()) {
            fleetype = j->mType;
            fleecount = num_this_type_to_flee;
        }
    }

    if (fleetype != UCrc32::kNull) {
        fleecount = bMin(fleecount, 2);
        FleeCopOfType(fleetype, fleecount);
    }

    if (!mIsPerpInSight && mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE &&
        mGroundSupportRequest.mHeavySupport != nullptr) {
        IVehicle *const *iter = mIVehicleList.begin();
        for (; iter != mIVehicleList.end(); iter++) {
            IVehicle *ivehicle = *iter;
            if (IsSupportVehicle(ivehicle)) {
                IPursuitAI *ipursuitai;
                if (ivehicle->QueryInterface(&ipursuitai)) {
                    ipursuitai->EndPursuit();
                }
            }
        }
        mGroundSupportRequest.Reset();
    }
}

const char *AIPursuit::CopRequest() {
    if (mIsPerpBusted || mSpawnCopTimer >= 0.0f || mIsPursuitBailed) {
        return nullptr;
    }

    Attrib::Gen::pursuitlevels *plevels = GetPursuitLevelAttrib();
    Attrib::Gen::pursuitsupport *ps = GetPursuitSupportAttrib();

    bool allowHeli = false;
    if (mSpawnHeliTimer < 0.0f && ps != nullptr && mTotalPursuitTime >= ps->MinimumSupportDelay(0)) {
        allowHeli = true;
    }

    if (allowHeli && mDoTestForHeliSearch) {
        mDoTestForHeliSearch = false;
        if (!mIsPerpInSight && !HeliVehicleActive()) {
            float heliSearchChance = plevels->SearchModeHeliSpawnChance(0);
            float rand = Sim::GetRandom()._SimRandom_FloatRange(100.0f);
            if (rand < heliSearchChance) {
                mForceHeliSpawnNext = true;
                SoundAI *copspeech = SoundAI::Get();
                if (copspeech != nullptr && copspeech->GetHeli() != nullptr) {
                    copspeech->GetHeli()->Quadrant();
                }
            }
        }
    }

    if (mForceHeliSpawnNext) {
        mForceHeliSpawnNext = false;
        return "copheli";
    }

    int numCopTypesToChooseFrom = 0;
    CopCountRecord adjustedCounts[8];
    GetAdjustedCopCounts(adjustedCounts, numCopTypesToChooseFrom);

    struct {
        unsigned int typeHash;
        int countNeeded;
        int Chance;
    } currentlyActive[10];

    int totalNeeded = 0;
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        const CopCountRecord &copcount = adjustedCounts[i];
        unsigned int hash = copcount.CopType.GetHash32();
        int countNeeded = static_cast< int >(copcount.Count);
        for (const CopContingent *j = mCopContingent.begin(); j != mCopContingent.end(); ++j) {
            if (j->mType.GetValue() == hash) {
                countNeeded = bMax(0, countNeeded - static_cast< int >(j->mCount));
            }
        }
        currentlyActive[i].typeHash = hash;
        currentlyActive[i].countNeeded = countNeeded;
        currentlyActive[i].Chance = copcount.Chance;
        totalNeeded += countNeeded;
    }

    mNumCopsNeeded = totalNeeded;
    if (totalNeeded <= 0) {
        return nullptr;
    }

    int totalWeight = 0;
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        const CopCountRecord &copcount = adjustedCounts[i];
        if (copcount.CopType.GetHash32() != heliHash1.GetValue() || allowHeli) {
            if (currentlyActive[i].countNeeded > 0) {
                totalWeight += currentlyActive[i].Chance;
            }
        }
    }

    int rand = Sim::GetRandom()._SimRandom_IntRange(totalWeight);
    const char *request = nullptr;
    for (int i = 0; i < numCopTypesToChooseFrom; i++) {
        if (currentlyActive[i].countNeeded > 0) {
            if (currentlyActive[i].typeHash != heliHash1.GetValue() || allowHeli) {
                rand -= currentlyActive[i].Chance;
                if (rand < 0) {
                    request = adjustedCounts[i].CopType.GetString();
                    break;
                }
            }
        }
    }
    return request;
}

void HerdFormation::Update(float dT, IPursuit *pursuit) {
    AITarget *target = pursuit->GetTarget();
    if (target == nullptr) {
        return;
    }

    ISimable *simable = target->GetSimable();
    IVehicleAI *ivehicleAI;
    if (simable == nullptr || !simable->QueryInterface(&ivehicleAI)) {
        return;
    }
    WRoadNav *currentRoad = ivehicleAI->GetCurrentRoad();
    if (currentRoad == nullptr) {
        return;
    }

    WRoadNav queryNav;
    UMath::Vector3 targetDir = target->GetDirection();
    queryNav.InitAtPoint(target->GetPosition(), targetDir, true, 0.0f);

    UMath::Vector3 roadPos;
    roadPos = queryNav.GetPosition();
    UMath::Vector3 roadDir;
    roadDir = queryNav.GetForwardVector();

    float sqLen = VU0_v3lengthsquare(roadDir);
    float len = VU0_sqrt(sqLen);
    if (len != 0.0f) {
        VU0_v3scale(roadDir, 1.0f / len, roadDir);
    }

    UMath::Vector3 perpDir;
    perpDir.x = roadDir.z;
    perpDir.y = roadDir.y;
    perpDir.z = -roadDir.x;

    UMath::Vector3 delta;
    UMath::Sub(target->GetPosition(), roadPos, delta);
    float dotProduct = UMath::Dot(delta, perpDir);

    const WRoadSegment *segment = queryNav.GetSegment();
    const WRoadProfile *profile = WRoadNetwork::Get().GetSegmentProfile(*segment, queryNav.GetNodeInd());
    if (profile == nullptr || profile->fNumZones == 0) {
        return;
    }

    UMath::Vector3 segForward;
    WRoadNetwork::Get().GetSegmentForwardVector(queryNav.GetSegmentInd(), segForward);
    float segDot = UMath::Dot(segForward, targetDir);

    int numLanes;
    if (segDot < 0.0f) {
        numLanes = profile->fMiddleZone;
    } else {
        numLanes = profile->fNumZones - profile->fMiddleZone;
    }

    float maxOffset = 0.0f;
    for (int i = 0; i < numLanes; i++) {
        int laneIdx = i;
        if (segDot < 0.0f) {
            laneIdx = profile->fNumZones - i - 1;
        }
        int laneType = profile->mLanes[laneIdx].GetType();
        if (laneType == 1) {
            int idx = i;
            if (segDot < 0.0f) {
                idx = profile->fNumZones - i - 1;
            }
            float offset = profile->mLanes[idx].GetOffset();
            if (offset > maxOffset) {
                maxOffset = offset;
            }
        }
    }

    float crowdDist = static_cast< float >(dotProduct - maxOffset) + 3.0f;
    crowdDist = UMath::Min(crowdDist, 8.0f);
    crowdDist = UMath::Max(crowdDist, 1.0f);

    for (TargetOffsetList::iterator it = mTargetOffsets.begin(); it != mTargetOffsets.end(); ++it) {
        it->mOffset.x = -crowdDist;
    }
}

void AIPursuit::UpdateOutOfFormationOffsets() {
    IRigidBody *targetRB;
    mTarget->QueryInterface(&targetRB);

    Pursuers assignCopList;
    Vector3List copRelativePositions;
    assignCopList.reserve(mIVehicleList.size());
    copRelativePositions.reserve(mIVehicleList.size());

    for (IVehicle::List::iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() != VehicleClass::CHOPPER) {
            IPursuitAI *ipv;
            if (ivehicle->GetSimable()->QueryInterface(&ipv) && !ipv->GetInFormation() && !IsSupportVehicle(ivehicle)) {
                ISimable *simable = ivehicle->GetSimable();
                UMath::Vector3 relPos = simable->GetPosition();
                UMath::Sub(relPos, mTarget->GetPosition(), relPos);
                if (targetRB) {
                    targetRB->ConvertWorldToLocal(relPos, false);
                }
                ipv->SetInPosition(false);
                assignCopList.push_back(ipv);
                copRelativePositions.push_back(relPos);
            }
        }
    }

    if (assignCopList.size() == 0) {
        return;
    }

    int i = 0;
    FormationTargetList formationOffsets;
    formationOffsets.reserve(assignCopList.size());

    const float spacing = 6.0f;
    const float baseDistance = 12.0f;
    const float distScale = 6.0f;
    const float zero = 0.0f;

    for (Pursuers::iterator it = assignCopList.begin(); it != assignCopList.end(); ++it) {
        float xOffset = static_cast< float >((i / 2 + 1) % 3 - 1) * spacing;
        float yOffset = zero;
        float zOffset = static_cast< float >((i % 2) * -2 + 1) * (static_cast< float >(i / 6) * distScale + baseDistance);

        UMath::Vector3 offset;
        offset.x = xOffset;
        offset.y = yOffset;
        offset.z = zOffset;
        UMath::Vector3 inPosOffset;
        inPosOffset.x = zero;
        inPosOffset.y = zero;
        inPosOffset.z = zero;

        FormationTarget target(offset, inPosOffset, UCrc32::kNull);
        formationOffsets.push_back(target);

        ++i;
    }

    EvenOutOffsets(copRelativePositions, formationOffsets);
    if (copRelativePositions.size() != 0 && formationOffsets.size() != 0) {
        AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, false);
    }
}
