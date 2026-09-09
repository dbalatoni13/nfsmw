#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
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
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPursuitLength.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

static const float kSecondsPerRepUpdate = 10.0f; // Decl: 83
static const float kSupportCheckTime = 10.0f;    // Decl: 84
float kBustedHUDTime = 3.0f;                     // Decl: 85

FormationType DebugCopFormation = PIT; // Decl: 87
bool CopFormationOverriden = false;    // Decl: 88
int giOverrideMaxCops;                 // Decl: 89

static const bool PrintPursuit = false; // Decl: 93

DECLARE_CONTAINER_TYPE(PursuitFormationTargetOffsetList);

// total size: 0x20
// Decl: 101
class PursuitFormation {
  public:
    // total size: 0x20
    // Decl: 106
    struct TargetOffset {
        TargetOffset(const UMath::Vector3 &targetOffset, const UMath::Vector3 &inPositionOffset, int minTargets, UCrc32 ipg)
            : mOffset(targetOffset),               //
              mInPositionOffset(inPositionOffset), //
              mMinTargets(minTargets),             //
              mInPositionGoal(ipg) {}

        ~TargetOffset() {}

        UMath::Vector3 mOffset;           // offset 0x0, size 0xC
        UMath::Vector3 mInPositionOffset; // offset 0xC, size 0xC
        int mMinTargets;                  // offset 0x18, size 0x4
        UCrc32 mInPositionGoal;           // offset 0x1C, size 0x4
    };

    // total size: 0x10
    // Decl: 119
    struct TargetOffsetList : public UTL::Std::vector<PursuitFormation::TargetOffset, _type_PursuitFormationTargetOffsetList> {
        TargetOffsetList() {}

        ~TargetOffsetList() {}

      private:
        USE_FASTALLOC(PursuitFormation::TargetOffsetList);
    };

    USE_FASTALLOC(PursuitFormation);

    PursuitFormation();

    virtual ~PursuitFormation();

    virtual void Update(float dT, IPursuit *pursuit) {}

    virtual float GetFinisherTolerance() {
        return 1.0f;
    }

    virtual float GetFinisherTime() {
        return 2.0f;
    }

    virtual float GetTimeToFinisher() {
        return 4.0f;
    }

    void Reset();

    void SetMaxCops(unsigned int m) {
        this->mMaxCops = m;
    }

    unsigned int GetMaxCops() {
        return this->mMaxCops;
    }

    void SetMinFinisherCops(unsigned int m) {
        this->mMinFinisherCops = m;
    }

    unsigned int GetMinFinisherCops() {
        return this->mMinFinisherCops;
    }

    void SetHasFinisher(bool f) {
        this->mHasFinisher = f;
    }

    bool GetHasFinisher() {
        return this->mHasFinisher;
    }

    void AddTargetOffset(const UMath::Vector3 &targetOffset, int minTargets, UCrc32 ipg, const UMath::Vector3 &inPositionOffset);

    const TargetOffsetList &GetTargetOffsets() {
        return this->mTargetOffsets;
    }

  protected:
    unsigned int mMaxCops;           // offset 0x0, size 0x4
    unsigned int mMinFinisherCops;   // offset 0x4, size 0x4
    bool mHasFinisher;               // offset 0x8, size 0x1
    TargetOffsetList mTargetOffsets; // offset 0xC, size 0x10
};

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

// total size: 0x28
// Decl: 202
class BoxInFormation : public PursuitFormation {
  public:
    BoxInFormation(int copcount, IPursuit *pursuit);

    // Overrides: PursuitFormation
    void Update(float dT, IPursuit *pursuit) override;

    // Overrides: PursuitFormation
    float GetFinisherTime() override {
        return this->finishertime;
    }

  private:
    void getPosition(int idx, float scale, UMath::Vector3 &pos);

    float tightness;    // offset 0x20, size 0x4
    float finishertime; // offset 0x24, size 0x4
};

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

void BoxInFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = (tightness * 0.2f) + 0.2f;
    float scale = finisher * ftight + (1.0f - ftight);

    for (int i = 0; i < 4; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

// total size: 0x28
// Decl: 294
class RollingBlockFormation : public PursuitFormation {
  public:
    RollingBlockFormation(int numCops, IPursuit *pursuit);

    // Overrides: PursuitFormation
    void Update(float dT, IPursuit *pursuit) override;

    // Overrides: PursuitFormation
    float GetFinisherTime() override {
        return this->finishertime;
    }

  private:
    void getPosition(int idx, float scale, UMath::Vector3 &pos);

    float tightness;    // offset 0x20, size 0x4
    float finishertime; // offset 0x24, size 0x4

    static const int num_positions = 5; // Decl: 308
};

RollingBlockFormation::RollingBlockFormation(int numCops, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
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

void RollingBlockFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt() / GetTimeToFinisher();
    float ftight = tightness * 0.4f;
    float scale = finisher * ftight + (1.0f - ftight);

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

// total size: 0x20
// Decl: 381
class FollowFormation : public PursuitFormation {
  public:
    FollowFormation(int copcount);
};

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
// Decl: 420
class StaggerFollowFormation : public PursuitFormation {
  public:
    StaggerFollowFormation(int copcount);
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

// total size: 0x20
// Decl: 460
class PitFormation : public PursuitFormation {
  public:
    PitFormation(int copcount);

    // Overrides: PursuitFormation
    float GetTimeToFinisher() override {
        return 1.2f;
    }

    // Overrides: PursuitFormation
    float GetFinisherTolerance() override {
        return 0.5f;
    }
};

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

// total size: 0x20
// Decl: 496
class HerdFormation : public PursuitFormation {
  public:
    HerdFormation(int copcount);

    // Overrides: PursuitFormation
    void Update(float dT, struct IPursuit *pursuit) override;
};

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

void HerdFormation::Update(float dT, IPursuit *pursuit) {
    AITarget *target = pursuit->GetTarget();
    if (target == nullptr) {
        return;
    }

    IVehicleAI *vehicleai;
    if (!target->QueryInterface(&vehicleai)) {
        return;
    }

    WRoadNav *roadnav = vehicleai->GetDriveToNav();
    if (roadnav == nullptr) {
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

    if (profile == nullptr || profile->fNumZones == 0) {
        return;
    }

    UMath::Vector3 segmentForwardVector;
    roadNetwork.GetSegmentForwardVector(queryNav.GetSegmentInd(), segmentForwardVector);

    bool inverted = UMath::Dot(segmentForwardVector, targetforward) < 0.0f;

    int rightmostlaneindex = profile->GetNumLanes(true, inverted);
    float rightmostlaneoffset = 0.0f;

    for (int i = 0; i < rightmostlaneindex; i++) {
        int lanenumber = profile->GetLaneNumber(i, inverted);

        if (profile->GetLaneType(lanenumber, false) == WRoadNav::kLaneTraffic) {
            rightmostlaneoffset = UMath::Max(rightmostlaneoffset, profile->GetLaneOffset(i, inverted));
        }
    }

    float crowddistance = UMath::Max(1.0f, UMath::Min(3.0f, roadoffset - rightmostlaneoffset + 2.0f));

    for (TargetOffsetList::iterator i = this->mTargetOffsets.begin(); i != this->mTargetOffsets.end(); ++i) {
        i->mOffset.x = -crowddistance;
    }
}

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
      mTarget(nullptr),                  //
      mFormation(nullptr),               //
      mRoadBlock(nullptr),               //
      mTimeSinceSetupSpeech(0),          //
      mBustedTimer(0.0f),                //
      mBustedIncrement(0.0f),            //
      mBustedHUDTime(0.0f),              //
      mIsPerpBusted(false),              //
      mIsPursuitBailed(false),           //
      mMostRecentCopDestroyedType(),     //
      mEvadeLevel(0.0f),                 //
      mCoolDownTimeRemaining(0.0f),      //
      mCoolDownTimeRequired(60.0f),      //
      mPercentOfContingentEngaged(0.0f), //
      mNumCopsFullyEngaged(0),           //
      mPursuitMeter(0.0f),               //
      mIsPerpInSight(true),              //
      mHiddenZoneTime(0.0f),             //
      mTimeSinceAnyCopSawPerp(-5.0f),    //
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
      mPursuitStatus(PS_INITIAL_CHASE),  //
      mBackupCountdownTimer(0.0f),       //
      mEnterSafehouseOnDestruct(false) {
    this->MakeDebugable(DBG_AI);

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
    this->mBreakerTimer = -1.0f;
    this->mTotalPursuitTime = 0.0f;
    this->mCollapseActive = false;
    this->mFormationAttemptCount = 0;
    this->mLastKnownLocation = UMath::Vector3Make(0.0f, 0.0f, 0.0f);

    this->mCopContingent.reserve(5);
    this->mAllowStatsToAccumulate = false;

    if (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::IsFinalEpicPursuit()) {
            this->mMaximumHeat = 6.0f;
            this->mBaseHeat = 6.0f;
        } else {
            this->mBaseHeat = GRaceStatus::Get().GetBinBaseHeat();
            this->mMaximumHeat = GRaceStatus::Get().GetBinMaxHeat();
        }
    } else if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_QuickRace && !GRaceStatus::IsChallengeRace()) {
        this->mBaseHeat = 1.0f;
        this->mMaximumHeat = 5.0f;
    } else {
        this->mBaseHeat = GRaceStatus::Get().GetBinBaseHeat();
        this->mMaximumHeat = GRaceStatus::Get().GetBinMaxHeat();
        this->mHeatScale = GRaceStatus::Get().GetBinHeatScale();

        bool useWorldHeatForRace = false;
        if (GRaceStatus::Exists()) {
            GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
            if (raceParms != nullptr) {
                useWorldHeatForRace = raceParms->GetUseWorldHeatInRace();
            }

            if (useWorldHeatForRace) {
                this->mBaseHeat = raceParms->GetForceHeatLevel();
                this->mMaximumHeat = raceParms->GetMaxRaceHeatLevel();
                this->mHeatScale = 1.0f;
            }
        }

        if (!useWorldHeatForRace) {
            GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
            if (raceParms != nullptr && raceParms->GetMaxHeatLevel() < this->mMaximumHeat) {
                this->mMaximumHeat = raceParms->GetMaxHeatLevel();
            }
        }
    }

    this->mCurrentPursuitLevel = 0;
    this->mActiveFormation = STAGGER_FOLLOW;
    this->mActiveFormationTime = 0.0f;
    this->InitFormation(0);

    this->mSpawnCopTimer = 0.0f;
    this->mSpawnHeliTimer = 10.0f;
    this->mDoTestForHeliSearch = false;
    this->mForceHeliSpawnNext = false;
    this->mCopDestroyedBonusTimer = 0.0f;
    this->mCopDestroyedBonusMultiplier = 1;
    this->mMostRecentCopDestroyedRepPoints = 0;
    this->mMostRecentCopDestroyedType = nullptr;
    this->mCoolDownMeterDisplayed = false;
    this->mPursuitMeterModeTimer = 0.0f;

    this->mSupportCheckTimer = kSupportCheckTime;
    this->mSupportPriorityCheckDone = false;
    this->mGroundSupportRequest.Reset();

    this->mJerkLagPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    this->mJerkLagDistance = 1000.0f;
    this->mJerkLagSpeed = 0.0f;
    this->mIsAJerk = false;
    this->mNumRBCopsAdded = 0;
    this->mMinDistanceToTarget = 100000.0f;
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

int numberIPV_misses = 0; // Decl: 942

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

// UNSOLVED
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

void AIPursuit::EvenOutOffsets(Vector3List &copRelativePositions, FormationTargetList &formationOffsets) {
    typedef UTL::Std::vector<const PursuitFormation::TargetOffset *, _type_AIPursuitEvenOutOffsetsSourceOffsets> SourceVector;

    const PursuitFormation::TargetOffsetList &offsetList = this->mFormation->GetTargetOffsets();

    SourceVector source_offsets;
    source_offsets.reserve(offsetList.size());

    for (PursuitFormation::TargetOffsetList::const_iterator i = offsetList.begin(); i != offsetList.end(); ++i) {
        source_offsets.push_back(&*i);
    }

    while (copRelativePositions.size() > formationOffsets.size() && formationOffsets.size() < this->mFormation->GetMaxCops()) {
        int bestPriority = 0;
        float bestDistance = 0.0f;
        SourceVector::iterator bestOffset = source_offsets.end();

        for (SourceVector::iterator i = source_offsets.begin(); i != source_offsets.end(); ++i) {
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

        if (bestOffset == source_offsets.end()) {
            break;
        }

        formationOffsets.push_back(FormationTarget((*bestOffset)->mOffset, (*bestOffset)->mInPositionOffset, (*bestOffset)->mInPositionGoal));
        *bestOffset = nullptr;
    }
}

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

            float distance = UMath::Distancexz(copPosition, offsetPosition);
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

// total size: 0xC
// Decl: 1663
struct CopAndAngle {
    CopAndAngle(IPursuitAI *c, float a, float d)
        : cop(c),   //
          angle(a), //
          distance(d) {}

    IPursuitAI *cop; // offset 0x0, size 0x4
    float angle;     // offset 0x4, size 0x4
    float distance;  // offset 0x8, size 0x4
};

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

static const float kCollapseSpeedThreshhold = 15.0f;    // size: 0x4, Decl: 1645
static const float kFormationCandidateDistance = 60.0f; // size: 0x4, Decl: 1646

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

bool AIPursuit::SetupCollapse(const Pursuers &cops, int max_inner, float inner_radius, float outer_radius) {
    typedef UTL::Std::vector<CopAndAngle, _type_AIPursuitSetupCollapseCopAngles> CopAngleVector;

    inner_radius = bMax(3.0f, inner_radius);
    outer_radius = bMax(inner_radius + 1.0f, outer_radius);

    CopAngleVector copangles;
    copangles.reserve(cops.size());

    AITarget *target = this->GetTarget();

    UMath::Vector3 front;
    if (target->GetSpeed() < KPH2MPS(5.0f)) {
        target->GetForwardVector(front);
    } else {
        front = target->GetLinearVelocity();
    }
    UMath::Normalize(front);

    UMath::Vector3 side;
    side = UMath::Vector3Make(front.z, 0.0f, -front.x);
    UMath::Normalize(side);

    UMath::Vector3 pos = target->GetPosition();

    UCrc32 fleegoal("AIGoalFleePursuit");

    Pursuers::const_iterator pursuitIter;
    for (pursuitIter = cops.begin(); pursuitIter != cops.end(); ++pursuitIter) {
        IVehicleAI *iai;
        IPursuitAI *ipv = *pursuitIter;
        if (!ipv->QueryInterface(&iai)) {
            continue;
        }
        UMath::Vector3 off;
        if (UMath::Distance(iai->GetVehicle()->GetPosition(), this->mTarget->GetPosition()) > kFormationCandidateDistance) {
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
    for (int i = 0; i < num; i++) {
        float a = UMath::Abs(copangles[i].angle);
        if (a < smallestAngle) {
            smallestAngle = a;
            frontmostCop = i;
        }
    }

    float step = 6.283185f / num;
    for (int i = 0; i < num; i++) {
        float angle = i * step;
        float c = UMath::Cosr(angle);
        float s = UMath::Sinr(angle);

        int index = (i + frontmostCop) % num;
        copangles[index].cop->SetInPositionOffset(UMath::Vector3Make(s * radius, 0.0f, c * radius));
        copangles[index].cop->SetInPositionGoal(kPullOverGoal);
        copangles[index].cop->DoInPositionGoal();
    }
}

void AIPursuit::UpdateFormation(float dT) {
    if (!this->mTarget->IsValid()) {
        return;
    }

    IVehicleAI *targetvehicleai;
    if (!this->mTarget->QueryInterface(&targetvehicleai)) {
        return;
    }

    IRigidBody *itargetRB;
    if (!this->mTarget->QueryInterface(&itargetRB)) {
        return;
    }

    this->mFormation->Update(dT, this);

    Pursuers assignCopList;
    Vector3List copRelativePositions;
    assignCopList.reserve(this->mIVehicleList.size());
    copRelativePositions.reserve(this->mIVehicleList.size());

    float formationCandidateLimit = kFormationCandidateDistance;
    formationCandidateLimit += UMath::Distance(this->mTarget->GetPosition(), targetvehicleai->GetCurrentRoad()->GetPosition());

    IVehicle::List::const_iterator vehicleIter;
    for (vehicleIter = this->mIVehicleList.begin(); vehicleIter != this->mIVehicleList.end(); ++vehicleIter) {
        IVehicle *ivehicle = *vehicleIter;
        IPursuitAI *ipv;
        if (!ivehicle->QueryInterface(&ipv)) {
            continue;
        }

        if (this->IsSupportVehicle(ivehicle)) {
            continue;
        }

        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;

        UMath::Vector3 targetRelativePos = ivehicle->GetSimable()->GetPosition();
        UMath::Sub(targetRelativePos, this->mTarget->GetPosition(), targetRelativePos);

        ipv->SetInFormation(false);

        if (bIsChopper) {
            if (!this->mIsPerpBusted && !this->mIsPursuitBailed) {
                this->AssignChopperGoal(ipv);
            }
        } else {
            if (ivehicle->GetAIVehiclePtr()->GetDrivableToTargetPos()) {
                if (UMath::Length(targetRelativePos) <= formationCandidateLimit) {
                    itargetRB->ConvertWorldToLocal(targetRelativePos, false);
                    assignCopList.push_back(ipv);
                    copRelativePositions.push_back(targetRelativePos);
                }
            }
        }
    }

    FormationTargetList formationOffsets;
    formationOffsets.reserve(this->mFormation->GetTargetOffsets().size());

    this->EvenOutOffsets(copRelativePositions, formationOffsets);

    if (copRelativePositions.size() != 0 && formationOffsets.size() != 0) {
        this->AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, true);
    }

    int i = 0;
    int countInFormation = 0;
    unsigned int countInPosition = 0;

    this->UpdateOutOfFormationOffsets();

    float grossDistanceToTarget = 0.0f;

    Pursuers::const_iterator pursuitIter;
    for (pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter, ++i) {
        IPursuitAI *ipv = *pursuitIter;

        if (ipv->GetInFormation()) {
            ++countInFormation;

            UMath::Vector3 copOffset = ipv->GetPursuitOffset();
            UMath::Vector3 copRelativePosition = copRelativePositions[i];

            float distanceToTarget = UMath::Distancexz(copOffset, copRelativePosition);

            if (distanceToTarget < 4.0f) {
                grossDistanceToTarget += distanceToTarget;
                ++countInPosition;
                ipv->SetInPosition(true);
                continue;
            }
        }
        ipv->SetInPosition(false);
    }

    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;

    if (this->mTarget->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    float collapsespeed = KPH2MPS(pursuitLevelAttrib->CollapseSpeed());

    if (this->mIsAJerk) {
        collapsespeed = KPH2MPS(125.0f);
    }

    if (this->mBreakerTimer >= 0.0f && this->mBreakerTimer < this->mFormation->GetFinisherTime() && !this->mIsPerpBusted && !this->mIsPursuitBailed) {
        this->mBreakerTimer += dT;
    } else {
        if (pursuitLevelAttrib != nullptr && this->mTarget->GetSpeed() < collapsespeed && countInFormation > 0 && !this->mIsPerpBusted &&
            this->mIsPerpInSight && !this->mIsPursuitBailed) {
            this->mCollapseActive =
                this->SetupCollapse(assignCopList, pursuitLevelAttrib->MaxCopsCollapsing(),
                                    static_cast<float>(pursuitLevelAttrib->CollapseInnerRadius()), pursuitLevelAttrib->CollapseOuterRadius());

            if (this->mCollapseActive && this->mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE &&
                this->mGroundSupportRequest.mHeavySupport != nullptr) {
                for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
                    IVehicle *ivehicle = *iter;

                    if (this->IsSupportVehicle(ivehicle)) {
                        IPursuitAI *ipursuitai;
                        if (ivehicle->QueryInterface(&ipursuitai)) {
                            ipursuitai->StartFlee();
                        }
                    }
                }

                this->mGroundSupportRequest.Reset();
            }
        } else if (!this->mIsPerpBusted && (this->mBreakerTimer >= 0.0f || this->mCollapseActive)) {
            this->mCollapseActive = false;
            this->mInFormationTimer = 0.0f;
            this->mBreakerTimer = -1.0f;

            Pursuers::const_iterator pursuitIter;
            for (pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter) {
                IPursuitAI *ipv = *pursuitIter;

                if (ipv != nullptr) {
                    ipv->SetInPositionGoal(UCrc32::kNull);
                    ipv->StartPursuit(this->mTarget, nullptr);
                }
            }

            for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
                IVehicle *ivehicle = *iter;
                IPursuitAI *ipv;

                if (ivehicle->QueryInterface(&ipv)) {
                    UCrc32 goal = ivehicle->GetAIVehiclePtr()->GetGoalName();
                    UCrc32 inpositiongoal = ipv->GetInPositionGoal();

                    if (goal == inpositiongoal || goal == kPullOverGoal) {
                        ipv->SetInPositionGoal(UCrc32::kNull);
                        ipv->StartPursuit(this->mTarget, nullptr);
                    }
                }
            }
        } else if (this->mFormation->GetHasFinisher() && countInPosition != 0 && !this->mCollapseActive && !this->mIsPerpBusted &&
                   !this->mIsPursuitBailed) {
            float averageDistanceToTarget = grossDistanceToTarget / static_cast<float>(countInPosition);

            const float disttolerance = this->mFormation->GetFinisherTolerance();

            float formationrate = disttolerance * 4.0f;

            this->mInFormationTimer += bClamp(((2 * formationrate) - averageDistanceToTarget) / formationrate, -1.0f, 1.0f) * dT;

            if (this->mInFormationTimer < 0.0f) {
                this->mInFormationTimer = 0.0f;
            }

            if (this->mInFormationTimer >= this->mFormation->GetTimeToFinisher()) {
                if (countInPosition >= this->mFormation->GetMinFinisherCops()) {
                    this->mBreakerTimer = 0.0f;

                    for (Pursuers::const_iterator pursuitIter = assignCopList.begin(); pursuitIter != assignCopList.end(); ++pursuitIter) {
                        IPursuitAI *ipv = *pursuitIter;

                        if (ipv != nullptr && ipv->GetInFormation() && ipv->GetInPositionGoal() != UCrc32::kNull) {
                            ipv->DoInPositionGoal();
                        }
                    }
                } else {
                    this->mInFormationTimer = this->mFormation->GetTimeToFinisher() - 0.01f;
                }
            }
        } else {
            this->mInFormationTimer = 0.0f;
        }
    }
}

void AIPursuit::UpdateOutOfFormationOffsets() {
    IRigidBody *itargetRB;
    this->mTarget->QueryInterface(&itargetRB);

    Pursuers assignCopList;
    Vector3List copRelativePositions;
    assignCopList.reserve(this->mIVehicleList.size());
    copRelativePositions.reserve(this->mIVehicleList.size());

    IVehicle *const *vehicleIter = this->mIVehicleList.begin();
    for (; vehicleIter != this->mIVehicleList.end(); ++vehicleIter) {
        IPursuitAI *ipv;
        IVehicle *ivehicle = *vehicleIter;
        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;

        if (bIsChopper || !ivehicle->QueryInterface(&ipv) || ipv->GetInFormation() || this->IsSupportVehicle(ivehicle)) {
            continue;
        }

        UMath::Vector3 targetRelativePos = ivehicle->GetSimable()->GetPosition();
        UMath::Sub(targetRelativePos, this->mTarget->GetPosition(), targetRelativePos);

        if (itargetRB != nullptr) {
            itargetRB->ConvertWorldToLocal(targetRelativePos, false);
        }

        ipv->SetInPosition(false);
        assignCopList.push_back(ipv);
        copRelativePositions.push_back(targetRelativePos);
    }

    if (assignCopList.size() == 0) {
        return;
    }

    int i = 0;
    FormationTargetList formationOffsets;
    formationOffsets.reserve(assignCopList.size());

    Pursuers::const_iterator pursuitIter = assignCopList.begin();
    for (; pursuitIter != assignCopList.end(); ++pursuitIter, ++i) {
        IPursuitAI *ipv = *pursuitIter;
        int r = i / 6;
        int s = 1 - (i % 2) * 2;
        int c = (i / 2 + 1) % 3 - 1;
        float horizontal_offset = static_cast<float>(c) * 3.5f;
        float vertical_offset = static_cast<float>(s) * (static_cast<float>(r) * 5.0f + 25.0f);
        UMath::Vector3 offset = UMath::Vector3Make(horizontal_offset, 0.0f, vertical_offset);

        formationOffsets.push_back(FormationTarget(offset, UMath::Vector3Make(0.0f, 0.0f, 0.0f), UCrc32::kNull));
    }

    this->AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets, false);
}

bool AIPursuit::IsPlayerPursuit() const {
    return this->GetTarget() != nullptr && this->GetTarget()->GetSimable() != nullptr && this->GetTarget()->GetSimable()->GetPlayer() != nullptr;
}

bool AIPursuit::ContingentHasActiveCops() const {
    for (ContingentVector::const_iterator j = this->mCopContingent.begin(); j != this->mCopContingent.end(); ++j) {
        if (j->mCount != 0) {
            return true;
        }
    }
    return false;
}

static const float kBustedTimeout = 5.0f;      // Decl: 2154
static const float kBustedCopDistance = 15.0f; // Decl: 2155
static const float kRoadBlockLOS = 60.0f;      // Decl: 2156

bool ForcePursuitNeverEnd = false; // Decl: 2161

int ForceRoadBlock = 0;           // Decl: 2163
int ForceRoadBlockSpikes = 0;     // Decl: 2164
bool ForceClearRoadblock = false; // Decl: 2165
bool ForcePursuitBail = false;    // Decl: 2166
float kTimePerPerpHeatBump = 120.0f;

bool AIPursuit::OnTask(HSIMTASK htask, float dT) {
    if (htask == this->mBustedTimerTask) {
        this->mBustedTimer += this->mBustedIncrement;
        if (this->mBustedTimer < 0.0f) {
            this->mBustedTimer = 0.0f;
        }
        return true;
    }

    if (htask != this->mSimulateTask || !this->mTarget->IsValid()) {
        return true;
    }

    this->mAllowStatsToAccumulate = !GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing ||
                                    (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace());

    this->UpdateJerk(dT);
    this->UpdateFormation(dT);

    float pursuitTimeBeforeUpdate = this->mTotalPursuitTime;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    bool is_player_perp = this->IsPlayerPursuit();

    IPerpetrator *iperp;
    if (this->mTarget->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();

        if (this->mNumCopsRequiredToEvade == 0) {
            this->LockInPursuitAttribs();
        }

        if (this->GetPursuitStatus() != PS_COOL_DOWN) {
            if (this->mAllowStatsToAccumulate) {
                this->mTotalPursuitTime += dT;
                GManager::Get().TrackValue("pursuit_length", this->mTotalPursuitTime);
            }

            if (static_cast<int>(pursuitTimeBeforeUpdate) != static_cast<int>(this->mTotalPursuitTime)) {
                MNotifyPursuitLength(this->mTarget->GetSimable()->GetOwnerHandle(), this->mTotalPursuitTime).Post(UCrc32(UCRC32_Gameplay));
            }

            float heat = iperp->GetHeat();

            if (pursuitLevelAttrib != nullptr) {
                float tphl = pursuitLevelAttrib->TimePerHeatLevel();
                this->mCoolDownTimeRequired = pursuitLevelAttrib->evadetimeout();

                if (FEDatabase->GetCareerSettings() != nullptr) {
                    int bin;
                    float heatIncModifier;

                    if (GRaceStatus::IsChallengeRace()) {
                        bin = 14;
                    } else {
                        bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
                    }

                    if (bin > 14) {
                        bin = 14;
                    }

                    heatIncModifier = pursuitLevelAttrib->ScaleEscalationPerBucket(bin);
                    tphl *= heatIncModifier;
                }

                heat += dT / tphl;
            }

            heat = bClamp(heat, this->mBaseHeat, this->mMaximumHeat);
            iperp->SetHeat(heat);

            heat = iperp->GetHeat();
            if (static_cast<int>(heat) != this->mCurrentPursuitLevel) {
                this->mCurrentPursuitLevel = static_cast<int>(heat);
                this->mActiveFormationTime = 0.0f;
                this->mSupportPriorityCheckDone = false;
                pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
                this->mRepPointsPerMinute = pursuitLevelAttrib->RepPointsPerMinute();
            }
        }

        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }

    this->mRoadBlockTimer -= dT;
    this->mActiveFormationTime -= dT;
    this->mSpawnCopTimer -= dT;
    this->mSpawnHeliTimer -= dT;
    this->mSupportCheckTimer -= dT;
    this->mCopDestroyedBonusTimer -= dT;
    this->mPursuitMeterModeTimer += dT;

    this->mGroundSupportRequest.Update(dT);

    SoundAI *copspeech = SoundAI::Get();
    if (copspeech != nullptr && copspeech->GetFocus() == 1) {
        this->mTimeSinceSetupSpeech = WorldTimer;
    }

    float t_speech_finished = (WorldTimer - this->mTimeSinceSetupSpeech).GetSeconds();

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    bool speech_finished;
    if (!pursuitRace && IsSpeechEnabled != 0) {
        speech_finished = 15.0f < t_speech_finished;
    } else {
        speech_finished = true;
    }

    if (this->mActiveFormationTime <= 0.0f && pursuitLevelAttrib != nullptr && !this->IsFinisherActive() && !this->mIsPerpBusted &&
        !this->mIsPursuitBailed && speech_finished) {
        int numFormations = pursuitLevelAttrib->Num_CopFormations();

        if (numFormations > 0) {
            FormationType newFormation = STAGGER_FOLLOW;
            float newFormationTime = pursuitLevelAttrib->StaggerFormationTime();

            if (this->mRoadBlock != nullptr && !this->mRoadBlock->GetDodged() && !this->mRoadBlock->GetNumCopsDamaged() &&
                !this->mRoadBlock->GetNumCopsDestroyed()) {
                newFormation = FOLLOW;
            } else if ((this->mFormationAttemptCount & 1) != 0) {
                float sumWeight = 0.0f;
                int i;

                for (i = 0; i < numFormations; ++i) {
                    sumWeight += pursuitLevelAttrib->CopFormations(i).Frequency;
                }

                float randomWeight = Sim::GetRandom().SimRandom_FloatRange(sumWeight);

                for (i = 0; i < numFormations; ++i) {
                    const CopFormationRecord &formationRec = pursuitLevelAttrib->CopFormations(i);
                    randomWeight -= formationRec.Frequency;

                    if (randomWeight <= 0.0f) {
                        newFormation = formationRec.Formation;
                        newFormationTime = formationRec.Duration;
                        break;
                    }
                }
            }

            this->mFormationAttemptCount++;

            if (newFormation != this->mActiveFormation) {
                this->mActiveFormation = newFormation;
                this->InitFormation(this->GetNumCops());
            }

            this->mActiveFormationTime = Sim::GetRandom().SimRandom_FloatRange(3.0f) + newFormationTime;
        }
    }

    this->RemoveUnwantedVehicles();

    float MinDistanceToTarget3 = FLT_MAX;
    this->mTimeSinceAnyCopSawPerp += dT;

    float MinDistanceToTargetxz = MinDistanceToTarget3;
    float engageRadius;
    if (pursuitLevelAttrib != nullptr) {
        engageRadius = pursuitLevelAttrib->FullEngagementRadius();
    } else {
        engageRadius = 150.0f;
    }

    int numVehiclesInRadius = 0;
    int numVehiclesActivelyInPursuit;

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;

        if (ivehicle->IsActive() && !ivehicle->IsDestroyed()) {
            IPursuitAI *ipursuitai;
            if (ivehicle->QueryInterface(&ipursuitai)) {
                float time = ipursuitai->GetTimeSinceTargetSeen();
                if (time < this->mTimeSinceAnyCopSawPerp) {
                    this->mTimeSinceAnyCopSawPerp = time;
                }
            }

            float distancey = bAbs(ivehicle->GetPosition().y - this->mTarget->GetPosition().y);
            float distance3 = UMath::Distance(ivehicle->GetPosition(), this->mTarget->GetPosition());
            float distancexz = UMath::Distancexz(ivehicle->GetPosition(), this->mTarget->GetPosition());

            if (distancey < 1.5f && distancexz < MinDistanceToTargetxz) {
                MinDistanceToTargetxz = distancexz;
            }

            if (distance3 < MinDistanceToTarget3) {
                MinDistanceToTarget3 = distance3;
            }

            if (distance3 < engageRadius) {
                if (!this->IsSupportVehicle(ivehicle)) {
                    numVehiclesInRadius++;
                }

                if (ipursuitai != nullptr && this->mPursuitStatus != PS_COOL_DOWN) {
                    ipursuitai->SetWithinEngagementRadius();
                }
            }
        }
    }

    this->mNumCopsFullyEngaged = numVehiclesInRadius;

    int remainingCopsToEvade = this->mNumCopsRequiredToEvade - this->mNumFullyEngagedCopsEvaded;

    if (remainingCopsToEvade < 1) {
        remainingCopsToEvade++;
        this->mNumCopsRequiredToEvade++;
    }

    int dif = this->mNumCopsFullyEngaged - remainingCopsToEvade;
    float bustedSpeedLimit;
    float sumTimeElapsed;
    if (dif > 0) {
        this->mNumCopsRequiredToEvade += dif;
    }

    if (this->mNumCopsRequiredToEvade != 0 && this->mPursuitStatus == PS_INITIAL_CHASE && remainingCopsToEvade <= this->mNumCopsToTriggerBackupTime) {
        this->mPursuitStatus = PS_BACKUP_REQUESTED;
        this->mBackupCountdownTimer = pursuitLevelAttrib->BackupCallTimer();
    }

    if (GRaceStatus::IsFinalEpicPursuit()) {
        this->mTimeSinceAnyCopSawPerp = 0.0f;
    }

    if (this->mRoadBlock != nullptr) {
        if (!iperp->IsHiddenFromCars()) {
            float RBMinDistancexz;
            float RBMinDistance3 = this->mRoadBlock->GetMinDistanceToTarget(dT, RBMinDistancexz, &this->mNearestCopInRoadblock);

            if (RBMinDistance3 < MinDistanceToTarget3) {
                MinDistanceToTarget3 = RBMinDistance3;

                if (RBMinDistance3 < kRoadBlockLOS) {
                    this->mIsPerpInSight = true;
                    this->mTimeSinceAnyCopSawPerp = 0.0f;
                }
            }

            if (RBMinDistancexz < MinDistanceToTargetxz) {
                MinDistanceToTargetxz = RBMinDistancexz;
            }

            this->mDistanceToNearestCopInRoadblock = RBMinDistance3;
        }

        if (this->mRoadBlock->IsPerpCheating() && this->mNumRBCopsAdded == 0 && this->mNearestCopInRoadblock != nullptr &&
            this->mRoadBlock->RemoveVehicle(this->mNearestCopInRoadblock)) {
            this->AddVehicle(this->mNearestCopInRoadblock);
            this->mNumCopsRequiredToEvade++;
            this->mNumRBCopsAdded++;
        }
    } else {
        this->mNearestCopInRoadblock = nullptr;
        this->mDistanceToNearestCopInRoadblock = 0.0f;
    }

    this->mMinDistanceToTarget = MinDistanceToTarget3;

    bustedSpeedLimit = KPH2MPS(pursuitLevelAttrib->BustSpeed());

    if (!this->mIsPerpBusted) {
        if (this->mIsPerpInSight && !this->mIsPursuitBailed) {
            bool isflashing = false;
            IRBVehicle *ivb;

            if (this->mTarget->QueryInterface(&ivb) && ivb->GetInvulnerability() == INVULNERABLE_FROM_MANUAL_RESET) {
                isflashing = true;
            }

            float busteddistance = kBustedCopDistance;

            if (isflashing) {
                busteddistance *= 6.0f;
            }

            if ((isflashing || this->mTarget->GetSpeed() < bustedSpeedLimit) && MinDistanceToTargetxz < busteddistance) {
                this->mBustedIncrement = dT * 0.25f;

                if (isflashing) {
                    this->mBustedIncrement *= 4.0f;
                }
            } else {
                this->mBustedIncrement = dT * -0.5f;
            }

            if (INIS::Exists() && INIS::Get()->IsWorldMomement()) {
                this->mBustedIncrement = 0.0f;
            }

            int minBefore = static_cast<int>(pursuitTimeBeforeUpdate * 0.1f);
            int minNow = static_cast<int>(this->mTotalPursuitTime * 0.1f);

            if (minBefore != minNow) {
                iperp->AddToPendingRepPointsNormal(this->mRepPointsPerMinute);
            }

            if (this->mPursuitStatus == PS_BACKUP_REQUESTED) {
                this->mBackupCountdownTimer -= dT;

                if (this->mBackupCountdownTimer < 0.0f) {
                    this->mPursuitStatus = PS_INITIAL_CHASE;
                    this->LockInPursuitAttribs();
                }
            }
        } else {
            this->mBustedIncrement = dT * -0.5f;
        }
    } else {
        this->mBustedIncrement = dT * 0.25f;
    }

    if (!TheOnlineManager.IsOnlineRace() && !this->mIsPerpBusted && !this->mIsPursuitBailed && this->mBustedTimer > kBustedTimeout) {
        this->mIsPerpBusted = true;
        this->mPursuitStatus = PS_BUSTED;

        if (is_player_perp) {
            for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
                IPursuitAI *ipursuitai;
                IVehicle *ivehicle = *iter;

                if (!ivehicle->IsActive() || ivehicle->IsDestroyed()) {
                    continue;
                }

                if (ivehicle->QueryInterface(&ipursuitai)) {
                    ivehicle->GlareOff(VehicleFX::LIGHT_COPS);
                    ipursuitai->SetInPositionGoal(UCrc32("AIGoalStopShort"));
                    ipursuitai->DoInPositionGoal();
                }
            }

            MPerpBusted(this->mTarget->GetSimable()->GetOwnerHandle()).Send(UCrc32(0x20d60dbf));
        } else {
            this->BailPursuit();

            if (GRaceStatus::Exists()) {
                GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(this->mTarget->GetSimable());

                if (racerInfo != nullptr) {
                    racerInfo->Busted();
                    racerInfo->ForceStop();

                    MPerpBusted(this->mTarget->GetSimable()->GetOwnerHandle()).Send(UCrc32("AIRacerBusted"));
                }
            }
        }
    } else if (this->mIsPerpBusted && is_player_perp) {
        bool was_over = this->mBustedHUDTime > kBustedHUDTime;

        this->mBustedHUDTime += dT;

        if (!was_over && this->mBustedHUDTime > kBustedHUDTime) {
            MPerpBusted(this->mTarget->GetSimable()->GetOwnerHandle()).Send(UCrc32(UCRC32_NIS));
        }
    }

    if (this->mIsPursuitBailed) {
        for (IVehicle *const *iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
            IPursuitAI *ipursuitai;
            IVehicle *ivehicle = *iter;

            if (!ivehicle->IsActive() || ivehicle->IsDestroyed()) {
                continue;
            }

            if (ivehicle->QueryInterface(&ipursuitai)) {
                ipursuitai->StartFlee();
            }
        }
    }

    this->mIsPerpInSight = this->mTimeSinceAnyCopSawPerp < 7.0f;

    if (iperp != nullptr) {
        bool perpHiding = false;

        if (iperp->IsHiddenFromCars() || iperp->IsHiddenFromHelicopters()) {
            perpHiding = true;
        }

        if (perpHiding && !this->mIsPerpInSight) {
            float hiddenZoneMultiplier;

            if (pursuitLevelAttrib != nullptr) {
                hiddenZoneMultiplier = pursuitLevelAttrib->HiddenZoneTimeMultiplier();
            } else {
                hiddenZoneMultiplier = 3.0f;
            }

            this->mHiddenZoneTime += dT * hiddenZoneMultiplier;
        }

        if (this->mIsPerpInSight) {
            this->mHiddenZoneTime = 0.0f;
            this->mLastKnownLocation = this->mTarget->GetPosition();
        }
    }

    sumTimeElapsed = this->mTimeSinceAnyCopSawPerp + this->mHiddenZoneTime;

    if (this->mTimeSinceAnyCopSawPerp > 7.0f) {
        bool is_evaded = false;

        this->mPursuitMeter = -1.0f;
        this->mEvadeLevel = sumTimeElapsed / this->mCoolDownTimeRequired;

        if (!this->mCoolDownMeterDisplayed) {
            this->mEvadeLevel = 0.0f;

            if (this->mPursuitMeterModeTimer > 2.5f) {
                this->mPursuitMeterModeTimer = 0.0f;
                this->mPursuitStatus = PS_COOL_DOWN;
                this->mCoolDownMeterDisplayed = true;

                this->mSpawnCopTimer = bMin(this->mSpawnCopTimer, pursuitLevelAttrib->TimeBetweenCopSpawn());
                this->mBackupCountdownTimer = 0.0f;
                this->mDoTestForHeliSearch = true;

                if (this->IsPlayerPursuit()) {
                    GInfractionManager::Get().ReportResistingArrest();
                }
            }
        } else {
            if (this->mEvadeLevel < 0.05f) {
                this->mEvadeLevel = 0.05f;
            } else if (this->mEvadeLevel >= 1.0f) {
                is_evaded = true;
            }
        }

        if (is_evaded) {
            this->mPursuitStatus = PS_EVADED;
            this->mEvadeLevel = 1.0f;

            if (this->IsPlayerPursuit() && ICopMgr::Exists()) {
                ICopMgr::Get()->LockoutCops(true);
            }
        }
    } else {
        if (this->mTimeSinceAnyCopSawPerp > 0.29f) {
            this->mPursuitMeter = bClamp(-0.5f - this->mTimeSinceAnyCopSawPerp / 14.0f, -1.0f, -0.5f);
        } else if (pursuitLevelAttrib != nullptr) {
            float deadBustedDist = pursuitLevelAttrib->MeterDeadZoneBustedDistance();
            float deadEvadeDist = pursuitLevelAttrib->MeterDeadZoneEvadeDist();
            float LOSDist = pursuitLevelAttrib->frontLOSdistance();

            this->mPursuitMeter = 0.0f;

            if (MinDistanceToTarget3 > deadEvadeDist) {
                float ratio = (MinDistanceToTarget3 - deadEvadeDist) / (LOSDist - deadEvadeDist);

                this->mPursuitMeter = bClamp(-0.1f - ratio * 0.4f, -0.5f, -0.1f);
            } else if (MinDistanceToTarget3 < deadBustedDist) {
                if (this->mTarget->GetSpeed() > MPH2MPS(70.0f)) {
                    this->mPursuitMeter = 0.0f;
                } else {
                    float D_ratio = bClamp((deadBustedDist - MinDistanceToTarget3) / (deadBustedDist - kBustedCopDistance), 0.0f, 1.0f);

                    float D = D_ratio;

                    float S = bClamp((KPH2MPS(100.0f) - this->mTarget->GetSpeed()) / (KPH2MPS(100.0f) - bustedSpeedLimit), 0.0f, 1.0f);

                    this->mPursuitMeter = (D * 0.3f + S * 0.7f) * 0.4f + 0.1f;
                }
            }
        }

        if (this->mCoolDownMeterDisplayed) {
            this->mEvadeLevel *= 0.93f;

            if (this->mEvadeLevel < 0.05f) {
                this->mEvadeLevel = 0.05f;
            }

            if (this->mPursuitMeterModeTimer > 2.5f) {
                this->mCoolDownMeterDisplayed = false;
                this->mPursuitStatus = PS_INITIAL_CHASE;
                this->mPursuitMeterModeTimer = 0.0f;
                this->mDoTestForHeliSearch = false;
            }
        } else {
            this->mEvadeLevel = 0.0f;
        }
    }

    this->mCoolDownTimeRemaining = UMath::Max(0.0f, this->mCoolDownTimeRequired - sumTimeElapsed);

    if (this->mCoolDownTimeRemaining > this->GetCoolDownTimeRequired()) {
        this->mCoolDownTimeRemaining = this->GetCoolDownTimeRequired();
    }

    if (this->mIsPerpBusted || this->mIsPursuitBailed) {
        this->mEvadeLevel = 0.0f;
    }

    return true;
}

bool AIPursuit::IsHeliInPursuit() const {
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            return true;
        }
    }
    return false;
}

bool ForcePursuitEnd = false; // Decl: 3174

bool AIPursuit::ShouldEnd() const {
    if (!this->mTarget->IsValid()) {
        return true;
    }

    if (this->mEvadeLevel >= 1.0f) {
        return true;
    }

    if (this->mPursuitStatus == PS_EVADED) {
        return true;
    }

    if (this->mIsPerpBusted || this->mIsPursuitBailed) {
        if (this->GetNumCops() == 0) {
            return true;
        }
    } else {
        return false;
    }

    return false;
}

static const UCrc32 heliHash1("copheli");

void AIPursuit::GetAdjustedCopCounts(CopCountRecord *counts, int &numcounts) {
    numcounts = 0;

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetGlobalPursuitLevelAttrib();
    if (pursuitLevelAttrib == nullptr) {
        return;
    }

    int max_cops = INT_MAX - 2;
    bool is_player_pursuit = this->IsPlayerPursuit();
    if (!is_player_pursuit) {
        max_cops = 3;
        if (ICopMgr::Get()->IsPlayerPursuitActive()) {
            max_cops = 2;
        }
    }

    int min_cops = 0;

    if (this->mPursuitStatus == PS_COOL_DOWN) {
        Attrib::Gen::pursuitlevels *myLevelAttrib = this->GetPursuitLevelAttrib();
        min_cops = myLevelAttrib->NumPatrolCars();
        max_cops = min_cops = bMin(max_cops, min_cops);
    } else {
        max_cops = bClamp(this->mNumCopsRequiredToEvade - this->mNumFullyEngagedCopsEvaded, 0, max_cops);
    }

    int nominal_cops = 0;
    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);

        if (copcount.CopType.GetHash32() != heliHash1.GetValue()) {
            nominal_cops += copcount.Count;
        }
    }

    int want_cops = bClamp(nominal_cops, min_cops, max_cops);

    for (unsigned int i = 0; i < pursuitLevelAttrib->Num_cops(); i++) {
        const CopCountRecord &copcount = pursuitLevelAttrib->cops(i);

        if (copcount.CopType.GetHash32() == heliHash1.GetValue()) {
            counts[numcounts] = copcount;
            numcounts++;
            continue;
        }
        int count = copcount.Count;
        int adjustedcount = static_cast<int>(static_cast<float>(count * want_cops) / static_cast<float>(nominal_cops) + 0.5f);

        if (adjustedcount) {
            counts[numcounts] = copcount;
            counts[numcounts].Count = adjustedcount;
            want_cops -= adjustedcount;
            nominal_cops -= count;
            numcounts++;
        }
    }
}

void AIPursuit::RemoveUnwantedVehicles() {
    int numAdjustedCounts;
    CopCountRecord adjustedCounts[8];
    this->GetAdjustedCopCounts(adjustedCounts, numAdjustedCounts);

    int typecount = 0;
    int fleecount = 0;
    UCrc32 fleetype = UCrc32::kNull;

    for (ContingentVector::const_iterator j = this->mCopContingent.begin(); j != this->mCopContingent.end(); ++j) {
        int num_this_type_to_flee = j->mCount;

        for (int i = 0; i < numAdjustedCounts; ++i) {
            const CopCountRecord &copcount = adjustedCounts[i];
            if (UCrc32(copcount.CopType) == j->mType) {
                num_this_type_to_flee = j->mCount - copcount.Count;
                break;
            }
        }

        if (num_this_type_to_flee < 1) {
            continue;
        }

        if (Sim::GetRandom().SimRandom_Float() >= static_cast<float>(typecount) / static_cast<float>(typecount + num_this_type_to_flee)) {
            fleetype = j->mType;
            fleecount = num_this_type_to_flee;
            break;
        }

        typecount += num_this_type_to_flee;
    }

    if (fleetype != UCrc32::kNull) {
        this->FleeCopOfType(fleetype, bMin(2, fleecount));
    }

    if (!this->mIsPerpInSight && this->mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::ACTIVE &&
        this->mGroundSupportRequest.mHeavySupport != nullptr) {
        for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
            IVehicle *ivehicle = *iter;
            if (this->IsSupportVehicle(ivehicle)) {
                IPursuitAI *ipursuitai;
                if (ivehicle->QueryInterface(&ipursuitai)) {
                    ipursuitai->StartFlee();
                }
            }
        }

        this->mGroundSupportRequest.Reset();
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

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;

        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            continue;
        }

        IPursuitAI *ipv;
        if (ivehicle->QueryInterface(&ipv) && ipv->GetSupportGoal().GetValue() != 0) {
            continue;
        }

        if (ivehicle->IsDestroyed()) {
            continue;
        }

        IVehicleAI *iai;
        if (ivehicle->QueryInterface(&iai) && iai->GetGoalName() == fleegoal) {
            ++already_fleeing;
            continue;
        }

        bool can_see_you = ipv->GetTimeSinceTargetSeen() <= 0.0f;
        if (can_see_you) {
            ++num_can_see_you;
        }

        if (UCrc32(ivehicle->GetVehicleName()) != type) {
            continue;
        }

        float dist = UMath::Distance(ivehicle->GetPosition(), this->mTarget->GetPosition());

        if (!can_see_you) {
            dist += 40.0f;
        }

        if (dist > distance || furthest == nullptr) {
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
    if (furthest != nullptr && furthest->QueryInterface(&ipursuitai) && fleecount > already_fleeing) {
        bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;

        if (!can_see_you || num_can_see_you > 2) {
            if (can_see_you) {
                --num_can_see_you;
            }

            ipursuitai->StartFlee();
        }
    }

    if (secondfurthest != nullptr && secondfurthest->QueryInterface(&ipursuitai) && fleecount > already_fleeing + 1) {
        bool can_see_you = ipursuitai->GetTimeSinceTargetSeen() <= 0.0f;

        if (!can_see_you || num_can_see_you > 2) {
            ipursuitai->StartFlee();
        }
    }
}

const char *AIPursuit::CopRequest() {
    if (this->mIsPerpBusted || this->mSpawnCopTimer >= 0.0f || this->mIsPursuitBailed) {
        return nullptr;
    }

    Attrib::Gen::pursuitlevels *plevels = this->GetPursuitLevelAttrib();
    Attrib::Gen::pursuitsupport *ps = this->GetPursuitSupportAttrib();

    bool allowHeli = false;
    if (this->mSpawnHeliTimer < 0.0f && ps != nullptr && ps->MinimumSupportDelay() < this->mTotalPursuitTime) {
        allowHeli = true;
    }

    if (allowHeli && this->mDoTestForHeliSearch) {
        this->mDoTestForHeliSearch = false;

        if (!this->mIsPerpInSight && !HeliVehicleActive()) {
            float heliSearchChance = plevels->SearchModeHeliSpawnChance();
            float rand = Sim::GetRandom().SimRandom_FloatRange(100.0f);

            if (rand <= heliSearchChance) {
                this->mForceHeliSpawnNext = true;

                SoundAI *copspeech = SoundAI::Get();
                if (copspeech != nullptr && copspeech->GetHeli() != nullptr) {
                    copspeech->GetHeli()->Quadrant();
                }
            }
        }
    }

    if (this->mForceHeliSpawnNext) {
        return "copheli";
    }

    int numCopTypesToChooseFrom;
    CopCountRecord adjustedCounts[8];
    this->GetAdjustedCopCounts(adjustedCounts, numCopTypesToChooseFrom);

    const char *request = nullptr;

    struct {
        uint32_t typeHash;
        int countNeeded;
        int Chance;
    } currentlyActive[10];

    int totalNeeded = 0;

    for (int i = 0; i < numCopTypesToChooseFrom; ++i) {
        const CopCountRecord &copcount = adjustedCounts[i];

        currentlyActive[i].typeHash = copcount.CopType.GetHash32();
        currentlyActive[i].countNeeded = copcount.Count;

        for (ContingentVector::const_iterator j = this->mCopContingent.begin(); j != this->mCopContingent.end(); ++j) {
            if (currentlyActive[i].typeHash == j->mType.GetValue()) {
                currentlyActive[i].countNeeded = bMax(0, currentlyActive[i].countNeeded - j->mCount);
                break;
            }
        }

        totalNeeded += currentlyActive[i].countNeeded;
    }

    this->mNumCopsNeeded = totalNeeded;

    if (totalNeeded == 0) {
        return nullptr;
    }

    int totalWeight = 0;

    for (int i = 0; i < numCopTypesToChooseFrom; ++i) {
        const CopCountRecord &copcount = adjustedCounts[i];

        currentlyActive[i].Chance = copcount.Chance != 0 ? copcount.Chance : 100;

        if (currentlyActive[i].typeHash == heliHash1.GetValue() && !allowHeli) {
            currentlyActive[i].Chance = 0;
        }

        if (currentlyActive[i].countNeeded == 0) {
            currentlyActive[i].Chance = 0;
        }

        totalWeight += currentlyActive[i].Chance;
    }

    int rand = Sim::GetRandom().SimRandom_IntRange(totalWeight);

    for (int i = 0; i < numCopTypesToChooseFrom; ++i) {
        rand -= currentlyActive[i].Chance;

        if (rand < 0) {
            request = adjustedCounts[i].CopType.GetString();
            break;
        }
    }

    return request;
}

int AIPursuit::RequestRoadBlock() {
    if (this->mIsPerpBusted || this->mIsPursuitBailed || this->mRoadBlock != nullptr) {
        return 0;
    }

    if (this->mRoadBlockTimer >= 0.0f) {
        return 0;
    }

    Attrib::Gen::pursuitsupport *ps = this->GetPursuitSupportAttrib();
    if (ps == nullptr) {
        return 0;
    }

    if (ps->MinimumSupportDelay() > this->mTotalPursuitTime) {
        return 0;
    }

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = this->GetPursuitLevelAttrib();
    if (pursuitLevelAttrib == nullptr) {
        return 0;
    }

    this->mRoadBlockTimer = Sim::GetRandom().SimRandom_FloatRange(4.0f) + 8.0f;

    int rv = this->mNextRoadblockRequest ? 4 : 0;

    float probability;
    if (this->IsPerpInSight() == true) {
        probability = pursuitLevelAttrib->roadblockprobability();
    } else {
        float radius = pursuitLevelAttrib->SearchModeRoadblockRadius();
        float d = UMath::Distance(this->mLastKnownLocation, this->mTarget->GetPosition());

        probability = pursuitLevelAttrib->SearchModeRoadblockChance();
        probability = (probability * (radius - d)) / radius;
    }

    float simProb = Sim::GetRandom().SimRandom_FloatRange(100.0f);

    if (simProb >= probability) {
        this->mNextRoadblockRequest = false;
    } else {
        this->mNextRoadblockRequest = true;
    }

    return rv;
}

void AIPursuit::AddRoadBlock(IRoadBlock *roadblock) {
    this->mRoadBlock = roadblock;
    this->Attach(roadblock);
    this->mNumRBCopsAdded = 0;
    if (this->mActiveFormation != FOLLOW) {
        if (!this->IsFinisherActive()) {
            this->EndCurrentFormation();
        }
    }
}

void AIPursuit::ClearGroundSupportRequest() {
    this->mGroundSupportRequest.Reset();
}

bool AIPursuit::SkidHitEnabled() const {
    Attrib::Gen::pursuitsupport *ps = this->GetPursuitSupportAttrib();
    for (int i = 0; i < static_cast<int>(ps->Num_AirSupportOptions()); i++) {
        const AirSupport &airSupport = ps->AirSupportOptions(i);
        if (airSupport.HeliStrategy == SKID_HIT) {
            return true;
        }
    }
    return false;
}

GroundSupportRequest *AIPursuit::RequestGroundSupport() {
    if (this->mIsPerpBusted || !this->mIsPerpInSight || this->mIsPursuitBailed) {
        return nullptr;
    }

    if (this->mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::NOT_ACTIVE) {
        return &this->mGroundSupportRequest;
    }

    if (this->mSupportCheckTimer >= 0.0f) {
        return nullptr;
    }

    this->mSupportCheckTimer = 10.0f;

    Attrib::Gen::pursuitsupport *ps = this->GetPursuitSupportAttrib();
    if (ps->MinimumSupportDelay() > this->mTotalPursuitTime) {
        return nullptr;
    }

    int rand = Sim::GetRandom().SimRandom_IntRange(100);

    if (!this->mSupportPriorityCheckDone) {
        for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); ++i) {
            const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);

            if (leaderSupport.PriorityTime < this->mTotalPursuitTime) {
                this->mSupportPriorityCheckDone = true;

                if (leaderSupport.PriorityChance > rand) {
                    this->mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                    this->mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
                    this->mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    break;
                }
            }
        }
    }

    if (this->mGroundSupportRequest.mSupportRequestStatus != GroundSupportRequest::PENDING) {
        rand = Sim::GetRandom().SimRandom_IntRange(100);

        if (this->mRoadBlock == nullptr) {
            for (int i = 0; i < static_cast<int>(ps->Num_HeavySupportOptions()); ++i) {
                const HeavySupport &heavySupport = ps->HeavySupportOptions(i);

                rand -= heavySupport.Chance;
                if (rand < 0) {
                    this->mGroundSupportRequest.mHeavySupport = &heavySupport;
                    this->mGroundSupportRequest.mSupportTimer = heavySupport.Duration;
                    this->mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    this->mRoadBlockTimer = 15.0f;
                    break;
                }
            }
        }

        if (rand >= 0) {
            for (int i = 0; i < static_cast<int>(ps->Num_LeaderSupportOptions()); ++i) {
                const LeaderSupport &leaderSupport = ps->LeaderSupportOptions(i);

                rand -= leaderSupport.Chance;
                if (rand < 0) {
                    this->mGroundSupportRequest.mLeaderSupport = &leaderSupport;
                    this->mGroundSupportRequest.mSupportTimer = leaderSupport.Duration;
                    this->mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::PENDING;
                    break;
                }
            }
        }
    }

    if (this->mGroundSupportRequest.mLeaderSupport != nullptr && this->mGroundSupportRequest.mSupportRequestStatus == GroundSupportRequest::PENDING &&
        this->mCrossState != CROSS_AVAILABLE) {
        this->mGroundSupportRequest.mSupportRequestStatus = GroundSupportRequest::NOT_ACTIVE;
        this->mGroundSupportRequest.mLeaderSupport = nullptr;
    }

    if (rand >= 0) {
        return nullptr;
    }

    return &this->mGroundSupportRequest;
}

bool AIPursuit::IsSupportVehicle(IVehicle *iv) {
    IPursuitAI *ipv;
    if (!iv->QueryInterface(&ipv)) {
        return false;
    }
    return ipv->GetSupportGoal() != static_cast<const char *>(nullptr);
}

bool AIPursuit::IsTarget(AITarget *aitarget) const {
    return this->mTarget->IsTarget(aitarget);
}

AITarget *AIPursuit::GetTarget() const {
    return this->mTarget;
}

bool AIPursuit::IsFinisherActive() const {
    return this->mBreakerTimer >= 0.0f;
}

float AIPursuit::TimeToFinisherAttempt() const {
    return this->mFormation->GetTimeToFinisher() - this->mInFormationTimer;
}

void AIPursuit::BailPursuit() {
    this->mIsPursuitBailed = true;
    ICopMgr *icopmanager = ICopMgr::Get();
    icopmanager->PursuitIsEvaded(this);
}

float testPursuitBar = 0.0f; // Decl: 3918

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
    return this->mRoadBlock != nullptr;
}

void AIPursuit::NotifyCopDamaged(IVehicle *ivehicle) {
    if (this->mAllowStatsToAccumulate) {
        this->mNumCopsDamaged++;
        if (this->mRoadBlock != nullptr) {
            if (this->mRoadBlock->IsComprisedOf(ivehicle->GetSimable()->GetOwnerHandle()) != nullptr) {
                this->mRoadBlock->IncNumCopsDamaged();
            }
        }
        GManager::Get().IncValue("cops_damaged");
    }
}

void AIPursuit::OnDebugDraw() {}

Attrib::Gen::pursuitlevels *GetGlobalPursuitLevelAttrib() {
    Attrib::Gen::pursuitlevels *pl = nullptr;

    IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_PLAYERS).begin();
    for (; iter != IVehicle::GetList(VEHICLE_PLAYERS).end(); ++iter) {
        IPerpetrator *iperp;
        IVehicleAI *ivehicleai;
        IVehicle *itargetVehicle = *iter;

        if (!itargetVehicle->QueryInterface(&iperp) || !itargetVehicle->QueryInterface(&ivehicleai)) {
            continue;
        }

        bool ispursued = ivehicleai->GetPursuit() != nullptr;

        if (pl == nullptr || ispursued) {
            pl = iperp->GetPursuitLevelAttrib();

            if (ispursued) {
                return pl;
            }
        }
    }

    iter = IVehicle::GetList(VEHICLE_RACERS).begin();
    for (; iter != IVehicle::GetList(VEHICLE_RACERS).end(); ++iter) {
        IPerpetrator *iperp;
        IVehicleAI *ivehicleai;
        IVehicle *itargetVehicle = *iter;
        DriverClass driverclass = itargetVehicle->GetDriverClass();

        if (driverclass == DRIVER_HUMAN || driverclass == DRIVER_REMOTE) {
            continue;
        }
        if (!itargetVehicle->QueryInterface(&iperp) || !itargetVehicle->QueryInterface(&ivehicleai)) {
            continue;
        }
        bool ispursued = ivehicleai->GetPursuit() != nullptr;

        if (pl == nullptr || ispursued) {
            pl = iperp->GetPursuitLevelAttrib();

            if (ispursued) {
                return pl;
            }
        }
    }

    return pl;
}

bool IsValidPursuitCarName(const char *name) {
    Attrib::Gen::pursuitlevels *pursuitlevels = GetGlobalPursuitLevelAttrib();
    if (pursuitlevels == nullptr) {
        return false;
    }

    UCrc32 nameHash(name);

    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); ++i) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);

        if (nameHash == UCrc32(copcount.CopType.GetString())) {
            return true;
        }
    }

    return false;
}

const char *GetRandomValidCopCar() {
    Attrib::Gen::pursuitlevels *pursuitlevels = GetGlobalPursuitLevelAttrib();
    if (pursuitlevels == nullptr) {
        return nullptr;
    }

    const char *vehicleName = nullptr;
    int totalRequested = 0;

    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); ++i) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);

        if (heliHash1 == UCrc32(copcount.CopType.GetString())) {
            continue;
        }
        totalRequested += copcount.Count;
    }

    int rand = Sim::GetRandom().SimRandom_IntRange(totalRequested);

    for (unsigned int i = 0; i < pursuitlevels->Num_cops(); ++i) {
        const CopCountRecord &copcount = pursuitlevels->cops(i);

        if (heliHash1 == UCrc32(copcount.CopType.GetString())) {
            continue;
        }
        rand -= copcount.Count;

        if (rand < 0) {
            vehicleName = copcount.CopType.GetString();
            break;
        }
    }

    return vehicleName;
}

void AIPursuit::SpikesHit(IVehicleAI *ivai) {
    if (ivai == nullptr) {
        return;
    }
    if (this->mNumRBCopsAdded != 0) {
        return;
    }
    bool did_it;
    for (int i = 0; i < 3; i++) {
        IRoadBlock *iroadblock = this->GetRoadBlock();
        if (iroadblock != nullptr) {
            IVehicle *ivehicleNear = nullptr;
            float dummy;
            float dist = iroadblock->GetMinDistanceToTarget(0.0f, dummy, &ivehicleNear);
            if (ivehicleNear != nullptr && iroadblock->RemoveVehicle(ivehicleNear)) {
                this->AddVehicle(ivehicleNear);
                this->mNumRBCopsAdded++;
                did_it = true;
            }
        }
    }
}

void AIPursuit::EndPursuitEnteringSafehouse() {
    this->mEvadeLevel = 1.0f;
    this->mPursuitStatus = PS_EVADED;
    this->mEnterSafehouseOnDestruct = true;
}

static const float kJerkDecayTime = 10.0f; // Decl: 4139
static const float kJerkOnFactor = 3.0f;   // Decl: 4140
static const float kJerkOffFactor = 1.75f; // Decl: 4141

void AIPursuit::UpdateJerk(float dt) {
    if (!this->mTarget->IsValid()) {
        return;
    }
    const UMath::Vector3 &pos = this->mTarget->GetPosition();
    float jerklerp = dt * 0.1f;
    UMath::Lerp(this->mJerkLagPosition, pos, jerklerp, this->mJerkLagPosition);

    float distance = UMath::Distance(pos, this->mJerkLagPosition);
    this->mJerkLagDistance = UMath::Lerp(this->mJerkLagDistance, distance, jerklerp);

    float speed = this->mTarget->GetSpeed();
    this->mJerkLagSpeed = UMath::Lerp(this->mJerkLagSpeed, speed, jerklerp * 0.5f);

    float jerkfactor;
    if (this->mJerkLagDistance > 0.01f) {
        jerkfactor = (this->mJerkLagSpeed * 10.0f) / this->mJerkLagDistance;
    } else {
        jerkfactor = 0.0f;
    }
    if (this->mIsAJerk) {
        if (jerkfactor <= 1.75f) {
            this->mIsAJerk = false;
        }
    } else {
        if (jerkfactor >= 3.0f) {
            this->mIsAJerk = true;
        }
    }
}
