#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
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
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
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

    if (via->IsCurrentGoal("AIGoalHeliExit") == false) {
        pursuitChopper->SetInPositionGoal("AIGoalHeliPursuit");
        pursuitChopper->SetInFormation(true);
        if (!via->IsCurrentGoal(pursuitChopper->GetInPositionGoal())) {
            pursuitChopper->DoInPositionGoal();
        }
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
    if ((mIsPerpBusted || mIsPursuitBailed) && GetNumCops() == 0) {
        return true;
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
                mRoadBlock->IncNumCopsDestroyed();
            }
        }
        GManager::Get().IncValue("cops_damaged");
    }
}

void AIPursuit::OnDebugDraw() {
}

void AIPursuit::EndPursuitEnteringSafehouse() {
    mPursuitStatus = PS_EVADED;
    mEnterSafehouseOnDestruct = true;
    mEvadeLevel = 1.0f;
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
    if (iv->QueryInterface(&ipv)) {
        return ipv->GetSupportGoal() != (const char *)nullptr;
    }
    return false;
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
                if (iroadblock->GetNumCops()) {
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
        if (!mIsAJerk) {
            if (jerkfactor >= 3.0f) {
                mIsAJerk = true;
            }
        } else {
            if (jerkfactor <= 1.75f) {
                mIsAJerk = false;
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

static const UCrc32 heliHash1("YOURHELICOPTERVEHICLETYPE");

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
