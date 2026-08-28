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
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
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
    this->Reset();
}

PursuitFormation::~PursuitFormation() {
    this->Reset();
}

void PursuitFormation::Reset() {
    this->mTargetOffsets.clear();
}

void PursuitFormation::AddTargetOffset(const UMath::Vector3 &targetOffset, int minTargets, UCrc32 ipg, const UMath::Vector3 &inPositionOffset) {
    this->mTargetOffsets.push_back(TargetOffset(targetOffset, inPositionOffset, minTargets, ipg));
}

BoxInFormation::BoxInFormation(int copcount, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib != nullptr) {
        this->tightness = pursuitLevelAttrib->BoxinTightness();
        this->finishertime = pursuitLevelAttrib->BoxinDuration();
    } else {
        this->tightness = 0.5f;
        this->finishertime = 2.0f;
    }

    UMath::Vector3 pos;
    UMath::Vector3 fpos;
    float foff = 2.0f - (this->tightness * 5.0f);
    float fscale = 0.7f - (this->tightness * 0.5f);

    this->getPosition(0, 1.0f, pos);
    this->getPosition(3, fscale, fpos);
    fpos.z = foff;
    this->AddTargetOffset(pos, 1, UCrc32("AIGoalRam"), fpos);

    this->getPosition(1, 1.0f, pos);
    this->getPosition(1, fscale, fpos);
    this->AddTargetOffset(pos, 2, UCrc32("AIGoalRam"), fpos);

    this->getPosition(2, 1.0f, pos);
    this->getPosition(2, fscale, fpos);
    this->AddTargetOffset(pos, 2, UCrc32("AIGoalRam"), fpos);

    this->getPosition(3, 1.0f, pos);
    this->getPosition(3, fscale, fpos);
    this->AddTargetOffset(pos, 4, UCrc32("AIGoalRam"), fpos);

    this->SetMaxCops(4);
    this->SetMinFinisherCops(2);
    this->SetHasFinisher(true);
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
    float ftight = (this->tightness * 0.2f) + 0.2f;
    float scale = (finisher / this->GetTimeToFinisher() * ftight) + (1.0f - ftight);

    for (int i = 0; i < 4; i++) {
        UMath::Vector3 pos;
        this->getPosition(i, scale, pos);
        this->mTargetOffsets[i].mOffset = pos;
    }
}

RollingBlockFormation::RollingBlockFormation(int numCops, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr; // r29
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib != nullptr) {
        this->tightness = pursuitLevelAttrib->RollingBlockTightness();
        this->finishertime = pursuitLevelAttrib->RollingBlockDuration();
    } else {
        this->tightness = 0.5f;
        this->finishertime = 2.0f;
    }

    float fscale = 1.0f - (this->tightness * 0.8f);
    float foff = 2.0f - (this->tightness * 5.0f);
    static const int priority[5] = {1, 2, 2, 3, 3};

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        UMath::Vector3 fpos;

        this->getPosition(i, 1.0f, pos);
        this->getPosition(i, fscale, fpos);
        fpos.z = foff;
        this->AddTargetOffset(pos, priority[i], UCrc32("AIGoalRam"), fpos);
    }

    this->SetMaxCops(4);
    this->SetMinFinisherCops(2);
    this->SetHasFinisher(true);
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
    float ftight = this->tightness * 0.4f;
    float scale = (finisher / this->GetTimeToFinisher() * ftight) + (1.0f - ftight);

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        this->getPosition(i, scale, pos);
        this->mTargetOffsets[i].mOffset = pos;
    }
}

FollowFormation::FollowFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -13.0f);
    this->AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    this->AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    this->AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -17.0f);
    this->AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -17.0f);
    this->AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -17.0f);
    this->AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    this->SetMaxCops(6);
    this->SetHasFinisher(false);
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
    this->AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, 13.0f);
    this->AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    this->AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, 13.0f);
    this->AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    this->AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, 13.0f);
    this->AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    this->SetMaxCops(6);
    this->SetHasFinisher(false);
}

PitFormation::PitFormation(int copcount) {
    UMath::Vector3 stupid_hack;
    UMath::Vector3 stupid_hack1;

    stupid_hack = UMath::Vector3Make(4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(-10.0f, 0.0f, -2.7f);
    this->AddTargetOffset(stupid_hack, 1, UCrc32("AIGoalPit"), stupid_hack1);

    stupid_hack = UMath::Vector3Make(-4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(10.0f, 0.0f, -2.7f);
    this->AddTargetOffset(stupid_hack, 1, UCrc32("AIGoalPit"), stupid_hack1);

    this->SetMaxCops(1);
    this->SetHasFinisher(true);
}

HerdFormation::HerdFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 0.0f);
    this->AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 5.0f);
    this->AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, -5.0f);
    this->AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    this->SetMaxCops(3);
    this->SetHasFinisher(false);
}

// void HerdFormation::Update(float dT, IPursuit *pursuit) {}

void GroundSupportRequest::Reset() {
    bool bAddToContingent = true;
    if (mSupportRequestStatus == ACTIVE && mHeavySupport != nullptr && mHeavySupport->HeavyStrategy == HEAVY_ROADBLOCK) {
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
                ipv->SetSupportGoal(UCrc32(static_cast<const char *>(nullptr)));
                if (iv->IsActive()) {
                    IVehicleAI *ivai;
                    // unchecked
                    ipv->QueryInterface(&ivai);
                    IPursuit *ip = ivai->GetPursuit();
                    if (ip != nullptr) {
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
    this->mSimulateTask = this->AddTask(UCrc32("AIPursuit"), 0.25f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    this->mBustedTimerTask = this->AddTask(UCrc32("AIPursuit"), 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(this->mSimulateTask, "AIPursuit");

    this->mIVehicleList.clear();
    this->mIVehicleList.reserve(10);

    this->mNearestCopInRoadblock = nullptr;
    this->mRoadBlockTimer = 0.0f;
    this->mDistanceToNearestCopInRoadblock = 0.0f;
    this->mTarget = new AITarget(nullptr);
    this->mTarget->Clear();

    this->mInFormationTimer = 0.0f;
    this->mTotalPursuitTime = 0.0f;
    this->mBreakerTimer = -1.0f;
    this->mCollapseActive = false;
    this->mFormationAttemptCount = 0;

    this->mLastKnownLocation = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    this->mCopContingent.reserve(5);

    // TODO later after we have more Gameplay stuff decomped
    // flipped in ghidra
    if (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::IsFinalEpicPursuit()) {
            this->mBaseHeat = 6.0f;
            this->mMaximumHeat = 6.0f;
        } else {
        }

    } else {
    }

    this->mCurrentPursuitLevel = 0;
    this->mActiveFormationTime = 0.0f;
    this->mActiveFormation = STAGGER_FOLLOW;
    this->InitFormation(0);
    this->mSpawnCopTimer = 0.0f;
    this->mDoTestForHeliSearch = false;
    this->mSpawnHeliTimer = 10.0f;
    this->mForceHeliSpawnNext = false;
    this->mCopDestroyedBonusTimer = 0.0f;
    this->mMostRecentCopDestroyedRepPoints = 0;
    this->mCopDestroyedBonusMultiplier = 1;
    this->mSupportCheckTimer = 10.0f;
    this->mMostRecentCopDestroyedType = (const char *)nullptr;
    this->mCoolDownMeterDisplayed = false;
    this->mPursuitMeterModeTimer = 0.0f;
    this->mSupportPriorityCheckDone = false;
    this->mGroundSupportRequest.Reset();

    this->mJerkLagPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    this->mJerkLagDistance = 1000.0f;

    this->mNumRBCopsAdded = 0;
    this->mMinDistanceToTarget = 100000.0f;
    this->mIsAJerk = false;
}

AIPursuit::~AIPursuit() {
    this->DetachAll();
    this->RemoveTask(this->mSimulateTask);
    this->RemoveTask(this->mBustedTimerTask);

    delete this->mFormation;
    delete this->mTarget;

    // TODO is this in the destructor of GroundSupportRequest?
    this->mGroundSupportRequest.Reset();
}

Sim::IActivity *AIPursuit::Construct(Sim::Param params) {
    return new AIPursuit(params);
}

Attrib::Gen::pursuitlevels *AIPursuit::GetPursuitLevelAttrib() const {
    Attrib::Gen::pursuitlevels *plevels = nullptr;
    IPerpetrator *perp;
    if (this->GetTarget() != nullptr) {
        if (this->GetTarget()->QueryInterface(&perp)) {
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
    if (this->GetTarget() != nullptr) {
        if (this->GetTarget()->QueryInterface(&perp)) {
            ps = perp->GetPursuitSupportAttrib();
        }
    } else {
        ps = nullptr;
    }
    return ps;
}

void AIPursuit::LockInPursuitAttribs() {
    Attrib::Gen::pursuitlevels *ps = this->GetPursuitLevelAttrib();
    if (ps != nullptr) {
        this->mNumCopsRequiredToEvade = ps->FullEngagementCopCount();
        this->mNumCopsToTriggerBackupTime = ps->NumCopsToTriggerBackup();
        this->mCoolDownTimeRequired = ps->evadetimeout();
        this->mNumFullyEngagedCopsEvaded = 0;
    }
}

uint32 AIPursuit::CalcTotalCostToState() const {
    uint32 total = this->mCopsDestroyed * 5000;
    total += this->mNumHeliSpawns * 2000;
    total += this->mNumRoadblocksDeployed * 500;
    total += this->mNumCopsDamaged * 250;
    total += this->mNumTrafficCarsHit * 500;
    total += this->mNumSpikeStripsDeployed * 250;
    total += this->mNumHeliSpikeStripsDeployed * 225;
    total += this->mNumCopCarsDeployed * 250;
    total += this->mNumSupportVehiclesDeployed * 450;
    total += this->mPropertyDamageValue;

    return total;
}

void AIPursuit::AddVehicleToContingent(IVehicle *ivehicle) {
    UCrc32 hash = UCrc32(ivehicle->GetVehicleName());
    for (ContingentVector::iterator i = this->mCopContingent.begin();; ++i) {
        if (i == this->mCopContingent.end()) {
            this->mCopContingent.push_back(CopContingent(hash));
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
            this->mTarget->Aquire(ivehicle->GetSimable());
            this->mJerkLagPosition = this->mTarget->GetPosition();

            if (this->IsPlayerPursuit()) {
                CameraAI::MaybeDoPursuitCam(ivehicle);
                PostRacePursuitScreen::GetPursuitData().ClearData();
                GInfractionManager::Get().PursuitStarted();
                GManager::Get().NotifyPursuitStarted();
            }

            float heat = iperp->GetHeat();
            if (heat < this->mBaseHeat) {
                heat = this->mBaseHeat;
            }
            iperp->SetHeat(heat);
            iperp->ClearPendingRepPoints();
        } else if (ivehicle->QueryInterface(&ipv)) {
            this->mIVehicleList.push_back(ivehicle);

            Attrib::Gen::pursuitlevels *plevels = this->GetPursuitLevelAttrib();
            if (plevels != nullptr) {
                if (this->mTotalCopsInvolved < 3 && this->mPursuitStatus != PS_COOL_DOWN) {
                    this->mSpawnCopTimer = plevels->TimeBetweenFirstFourSpawn();
                } else {
                    this->mSpawnCopTimer = plevels->TimeBetweenCopSpawn();
                    if (this->mNumCopsNeeded > 2) {
                        if (this->mFastSpawnNext) {
                            this->mFastSpawnNext = false;
                            this->mSpawnCopTimer = 0.2f;
                        } else {
                            this->mFastSpawnNext = true;
                        }
                    }
                }
            } else {
                this->mSpawnCopTimer = 0.0f;
            }
            this->mTotalCopsInvolved++;

            const UCrc32 crossName = UCrc32("copcross");
            const UCrc32 suv = UCrc32("copsuv");
            const UCrc32 suvl = UCrc32("copsuvl");
            const UCrc32 hench = UCrc32("copsporthench");
            const UCrc32 vname = UCrc32(ivehicle->GetVehicleName());

            if (vname == suv || vname == suvl || vname == crossName || vname == hench) {
                this->mNumSupportVehiclesDeployed++;
                if (vname == crossName) {
                    this->mCrossState = CROSS_SPAWNED;
                }
            } else {
                if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                    this->mForceHeliSpawnNext = false;
                    this->mNumHeliSpawns++;
                } else {
                    this->mNumCopCarsDeployed++;
                }
            }
            GManager::Get().TrackValue("total_cops_in_pursuit", this->mTotalCopsInvolved);

            IPerpetrator *iperp;
            if (this->mTarget->QueryInterface(&iperp) && this->mRepPointsPerMinute == 0) {
                int perpHeat = static_cast<int>(iperp->GetHeat());
                if (plevels != nullptr) {
                    this->mRepPointsPerMinute = plevels->RepPointsPerMinute();
                }
            }

            ipv->StartPursuit(this->mTarget, nullptr);
            if (this->IsSupportVehicle(ivehicle)) {
                ipv->StartSupportGoal();
                this->mNumSupportVehiclesActive++;
            }
            this->AddVehicleToContingent(ivehicle);
        }
    }
    this->TrackVehicleCounts();
    this->Activity::OnAttached(pOther);
}

void AIPursuit::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;

    if (UTL::COM::ComparePtr(pOther, this->mRoadBlock)) {
        this->mRoadBlock = nullptr;
    } else {
        if (this->GetTarget()->IsValid() && UTL::COM::ComparePtr(this->GetTarget()->GetSimable(), pOther)) {
            ISimable *defaultsimable = IPlayer::First(PLAYER_LOCAL)->GetSimable();

            for (IVehicle::List::iterator i = this->mIVehicleList.begin(); i != this->mIVehicleList.end(); ++i) {
                IVehicle *ivehicle = *i;
                ivehicle->GetAIVehiclePtr()->GetTarget()->Aquire(defaultsimable);
            }
            this->mTarget->Clear();
        } else if (pOther->QueryInterface(&ivehicle)) {
            const UCrc32 crossName = UCrc32("copcross");
            bool isCross = ivehicle->GetVehicleName() == crossName;

            if (ivehicle->IsDestroyed()) {
                this->IncNumCopsDestroyed(ivehicle);
                if (isCross) {
                    this->mCrossState = CROSS_DISABLED;
                }
            } else if (isCross) {
                this->mCrossState = CROSS_AVAILABLE;
            }

            IAIHelicopter *aih;
            if (ivehicle->QueryInterface(&aih)) {
                Attrib::Gen::pursuitlevels *plevels = this->GetPursuitLevelAttrib();
                if (plevels != nullptr) {
                    this->mSpawnHeliTimer = plevels->TimeBetweenHeliActive();
                }
            }

            IVehicle::List::iterator iter = std::find(this->mIVehicleList.begin(), this->mIVehicleList.end(), ivehicle);
            if (iter != this->mIVehicleList.end()) {
                bool bIsSupport = this->IsSupportVehicle(ivehicle);
                if (bIsSupport) {
                    this->mNumSupportVehiclesActive--;
                    if (this->mNumSupportVehiclesActive == 0) {
                        this->mGroundSupportRequest.Reset();
                    }
                }
                this->mIVehicleList.erase(iter);

                IPursuitAI *ipv;
                if (ivehicle->QueryInterface(&ipv)) {
                    if (ipv->WasWithinEngagementRadius() && !bIsSupport && this->mAllowStatsToAccumulate) {
                        this->mNumFullyEngagedCopsEvaded++;
                    }
                    ipv->EndPursuit();
                }

                UCrc32 hash = UCrc32(ivehicle->GetVehicleName());
                for (ContingentVector::iterator i = this->mCopContingent.begin();; i++) {
                    if (i->mType == hash) {
                        i->mCount--;
                        break;
                    }
                }
            }
        }
    }

    this->TrackVehicleCounts();
}

void AIPursuit::IncNumCopsDestroyed(IVehicle *ivehicle) {
    if (!this->mAllowStatsToAccumulate) {
        return;
    }
    IVehicleAI *ivai = ivehicle->GetAIVehiclePtr();
    if (ivai != nullptr) {
        this->mMostRecentCopDestroyedRepPoints = ivai->GetAttributes().RepPointsForDestroying(this->mCurrentPursuitLevel);
        this->mMostRecentCopDestroyedType = ivehicle->GetVehicleName();

        int multiplier = 1;
        if (this->mCopDestroyedBonusTimer > 0.0f) {
            if (this->mCopDestroyedBonusMultiplier < 3) {
                this->mCopDestroyedBonusMultiplier++;
            }
            multiplier = this->mCopDestroyedBonusMultiplier;
        } else {
            this->mCopDestroyedBonusTimer = 0.0f;
            this->mCopDestroyedBonusMultiplier = 1;
        }
        int repForDestruction = this->mMostRecentCopDestroyedRepPoints * multiplier;

        Attrib::Gen::pursuitlevels *plevel = this->GetPursuitLevelAttrib();
        if (plevel != nullptr) {
            this->mCopDestroyedBonusTimer = plevel->DestroyCopBonusTime();
        }
        IPerpetrator *iperp;
        if (this->mTarget->QueryInterface(&iperp)) {
            iperp->AddToPendingRepPointsFromCopDestruction(repForDestruction);
        }
    }

    if (this->mRoadBlock != nullptr) {
        if (this->mRoadBlock->IsComprisedOf(ivehicle->GetSimable()->GetOwnerHandle()) != nullptr) {
            this->mRoadBlock->IncNumCopsDestroyed();
        }
    }
    this->mCopsDestroyed++;
    GManager::Get().TrackValue("cops_destroyed_in_pursuit", this->mCopsDestroyed);
}

void AIPursuit::TrackVehicleCounts() {
    int copCarCount = 0;
    int chopperCount = 0;

    for (IVehicle::List::const_iterator vehicleIter = this->mIVehicleList.begin(); vehicleIter != this->mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;
        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;
        if (bIsChopper) {
            chopperCount++;
        } else {
            copCarCount++;
        }
    }
    if (GManager::Exists() && this->mAllowStatsToAccumulate) {
        GManager::Get().TrackValue("cops_in_pursuit", copCarCount);
        GManager::Get().TrackValue("helis_in_pursuit", chopperCount);
    }
}

FormationType AIPursuit::GetFormationType() const {
    return this->mActiveFormation;
}

void AIPursuit::InitFormation(int numCops) {
    delete this->mFormation;

    switch (this->mActiveFormation) {
        case PIT:
            this->mFormation = new PitFormation(numCops);
            break;
        case BOX_IN:
            this->mFormation = new BoxInFormation(numCops, this);
            break;
        case ROLLING_BLOCK:
            this->mFormation = new RollingBlockFormation(numCops, this);
            break;
        case FOLLOW:
            this->mFormation = new FollowFormation(numCops);
            break;
        case HERD:
            this->mFormation = new HerdFormation(numCops);
            break;
        case HELI_PURSUIT:
            this->mFormation = new FollowFormation(numCops);
            break;
        case STAGGER_FOLLOW:
            this->mFormation = new StaggerFollowFormation(numCops);
            break;
        default:
            break;
    }
}

void AIPursuit::EndCurrentFormation() {
    this->mActiveFormationTime = 0.0f;
    this->mBreakerTimer = -1.0f;
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
    if (this->IsAttemptingRoadBlock())
        return;

    IVehicleAI *via;
    pursuitChopper->QueryInterface(&via);

    if (via->IsCurrentGoal(UCrc32("AIGoalHeliExit")) == false) {
        pursuitChopper->SetInPositionGoal(UCrc32("AIGoalHeliPursuit"));
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

    const PursuitFormation::TargetOffsetList &offsetList = this->mFormation->GetTargetOffsets();

    SourceVector source_offsets;
    source_offsets.reserve(offsetList.size());

    for (PursuitFormation::TargetOffsetList::const_iterator i = offsetList.begin(); i != offsetList.end(); ++i) {
        source_offsets.push_back(i);
    }

    while (copRelativePositions.size() > formationOffsets.size() && formationOffsets.size() < this->mFormation->GetMaxCops()) {
        int bestPriority = 0;
        float bestDistance = 0.0f;
        PursuitFormation::TargetOffsetList::const_iterator *bestOffset = source_offsets.end();

        for (PursuitFormation::TargetOffsetList::const_iterator *i = source_offsets.begin(); i != source_offsets.end(); ++i) {
            // TODO does this .end belong here?
            if ((*i != nullptr) && (bestOffset == source_offsets.end() || (*i)->mMinTargets <= bestPriority)) {
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
        *bestOffset = nullptr;
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

        this->AssignCopOffset(currentCop, assignCopList, formationOffsets[currentOffset].Offset, formationOffsets[currentOffset].InPositionOffset,
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

    AITarget *target = this->GetTarget(); // r30

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
        if (UMath::Distance(iai->GetVehicle()->GetPosition(), this->mTarget->GetPosition()) > 60.0f) {
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
        this->AssignCopsInCircle(&copangles[0], inneroffset, outer_radius, front, side);
    }
    this->AssignCopsInCircle(&copangles[inneroffset], numinner, inner_radius, front, side);

    return true;
}

static const UCrc32 kPullOverGoal = UCrc32("AIGoalPullOver");

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
    return this->GetTarget() != nullptr && this->GetTarget()->GetSimable() != nullptr && this->GetTarget()->GetSimable()->GetPlayer() != nullptr;
}
