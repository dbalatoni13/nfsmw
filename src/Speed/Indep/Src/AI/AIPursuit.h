#ifndef AI_AIPURSUIT_H
#define AI_AIPURSUIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Sim/SimActivity.h"

DECLARE_CONTAINER_TYPE(PursuitFormationTargetOffsetList);

// total size: 0x20
class PursuitFormation {
  public:
    // total size: 0x20
    struct TargetOffset {
        TargetOffset(const UMath::Vector3 &targetOffset, const UMath::Vector3 &inPositionOffset, int minTargets, UCrc32 ipg)
            : mOffset(targetOffset),               //
              mInPositionOffset(inPositionOffset), //
              mMinTargets(minTargets),             //
              mInPositionGoal(ipg) {}

        ~TargetOffset() {}

        TargetOffset &operator=(const TargetOffset &_ctor_arg) {
            mOffset = _ctor_arg.mOffset;
            mInPositionOffset = _ctor_arg.mInPositionOffset;
            mMinTargets = _ctor_arg.mMinTargets;
            mInPositionGoal = _ctor_arg.mInPositionGoal;
        }

        TargetOffset(const TargetOffset &_ctor_arg)
            : mOffset(_ctor_arg.mOffset),                     //
              mInPositionOffset(_ctor_arg.mInPositionOffset), //
              mMinTargets(_ctor_arg.mMinTargets),             //
              mInPositionGoal(_ctor_arg.mInPositionGoal) {}

        UMath::Vector3 mOffset;           // offset 0x0, size 0xC
        UMath::Vector3 mInPositionOffset; // offset 0xC, size 0xC
        int mMinTargets;                  // offset 0x18, size 0x4
        UCrc32 mInPositionGoal;           // offset 0x1C, size 0x4
    };

    // total size: 0x10
    struct TargetOffsetList : public UTL::Std::vector<PursuitFormation::TargetOffset, _type_PursuitFormationTargetOffsetList> {
        // void *operator new(size_t size, void *ptr) {}

        // void operator delete(void *mem, void *ptr) {}

        void *operator new(size_t size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, size_t size) {
            if (mem) {
                return gFastMem.Free(mem, size, nullptr);
            }
        }

        void *operator new(size_t size, const char *name) {
            return gFastMem.Alloc(size, name);
        }

        // void operator delete(void *mem, const char *name) {}

        void operator delete(void *mem, size_t size, const char *name) {
            if (mem) {
                return gFastMem.Free(mem, size, name);
            }
        }

        TargetOffsetList() {}

        ~TargetOffsetList() {}
    };

    // void *operator new(size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        if (mem) {
            return gFastMem.Free(mem, size, nullptr);
        }
    }

    void *operator new(size_t size, const char *name) {
        return gFastMem.Alloc(size, name);
    }

    // void operator delete(void *mem, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {
        if (mem) {
            return gFastMem.Free(mem, size, name);
        }
    }

    PursuitFormation();

    virtual ~PursuitFormation();

    void Reset();
    void AddTargetOffset(const UMath::Vector3 &targetOffset, int minTargets, UCrc32 ipg, const UMath::Vector3 &inPositionOffset);

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

    void SetMaxCops(unsigned int m) {
        mMaxCops = m;
    }

    unsigned int GetMaxCops() {
        return mMaxCops;
    }

    void SetMinFinisherCops(unsigned int m) {
        mMinFinisherCops = m;
    }

    unsigned int GetMinFinisherCops() {
        return mMinFinisherCops;
    }

    void SetHasFinisher(bool f) {
        mHasFinisher = f;
    }

    bool GetHasFinisher() {
        return mHasFinisher;
    }

    const TargetOffsetList &GetTargetOffsets() {
        return mTargetOffsets;
    }

  protected:
    unsigned int mMaxCops;           // offset 0x0, size 0x4
    unsigned int mMinFinisherCops;   // offset 0x4, size 0x4
    bool mHasFinisher;               // offset 0x8, size 0x1
    TargetOffsetList mTargetOffsets; // offset 0xC, size 0x10
};

// total size: 0x28
class BoxInFormation : public PursuitFormation {
  public:
    BoxInFormation(int copcount, IPursuit *pursuit);

    // Overrides: PursuitFormation
    void Update(float dT, IPursuit *pursuit) override;

    // Overrides: PursuitFormation
    ~BoxInFormation() override {}

    // Overrides: PursuitFormation
    float GetFinisherTime() override {
        return finishertime;
    }

  private:
    void getPosition(int idx, float scale, UMath::Vector3 &pos);

    float tightness;    // offset 0x20, size 0x4
    float finishertime; // offset 0x24, size 0x4
};

// total size: 0x28
class RollingBlockFormation : public PursuitFormation {
  public:
    RollingBlockFormation(int numCops, IPursuit *pursuit);

    // Overrides: PursuitFormation
    void Update(float dT, IPursuit *pursuit) override;

    // Overrides: PursuitFormation
    ~RollingBlockFormation() override {}

    // Overrides: PursuitFormation
    float GetFinisherTime() override {
        return finishertime;
    }

  private:
    void getPosition(int idx, float scale, UMath::Vector3 &pos);

    static const int num_positions; // size: 0x4, address: 0xFFFFFFFF

    float tightness;    // offset 0x20, size 0x4
    float finishertime; // offset 0x24, size 0x4
};

// total size: 0x20
class FollowFormation : public PursuitFormation {
  public:
    FollowFormation(int copcount);

    // Overrides: PursuitFormation
    ~FollowFormation() override {}
};

// total size: 0x20
class PitFormation : public PursuitFormation {
  public:
    PitFormation(int copcount);

    // Overrides: PursuitFormation
    ~PitFormation() override {}

    // Overrides: PursuitFormation
    float GetTimeToFinisher() override {
        return 1.2f;
    }

    // Overrides: PursuitFormation
    float GetFinisherTolerance() override {
        return 0.5f;
    }
};

// total size: 0x20
class HerdFormation : public PursuitFormation {
  public:
    HerdFormation(int copcount);

    // Overrides: PursuitFormation
    void Update(float dT, struct IPursuit *pursuit) override;

    // Overrides: PursuitFormation
    ~HerdFormation() override {}
};

// total size: 0xC
struct CopAndAngle {
    CopAndAngle(IPursuitAI *c, float a, float d)
        : cop(c),   //
          angle(a), //
          distance(d) {}

    IPursuitAI *cop; // offset 0x0, size 0x4
    float angle;     // offset 0x4, size 0x4
    float distance;  // offset 0x8, size 0x4
};

DECLARE_CONTAINER_TYPE(AIPursuers);
DECLARE_CONTAINER_TYPE(AIVector3List);
DECLARE_CONTAINER_TYPE(AIFormationTargetList);
DECLARE_CONTAINER_TYPE(AICopContingent);

// total size: 0x248
class AIPursuit : public Sim::Activity, public IPursuit, public Debugable {
  public:
    enum eCrossState {
        CROSS_AVAILABLE = 0,
        CROSS_SPAWNED = 1,
        CROSS_DISABLED = 2,
    };
    // total size: 0x1C
    struct FormationTarget {
        FormationTarget(const UMath::Vector3 &o, const UMath::Vector3 &ipo, const UCrc32 &g)
            : Offset(o),             //
              InPositionOffset(ipo), //
              Goal(g) {}

        FormationTarget(const FormationTarget &_ctor_arg)
            : Offset(_ctor_arg.Offset),                     //
              InPositionOffset(_ctor_arg.InPositionOffset), //
              Goal(_ctor_arg.Goal) {}

        UMath::Vector3 Offset;           // offset 0x0, size 0xC
        UMath::Vector3 InPositionOffset; // offset 0xC, size 0xC
        UCrc32 Goal;                     // offset 0x18, size 0x4
    };
    // total size: 0x8
    struct CopContingent {
        CopContingent(UCrc32 t)
            : mType(t), //
              mCount(1) {}

        CopContingent(const CopContingent &_ctor_arg)
            : mType(_ctor_arg.mType), //
              mCount(_ctor_arg.mCount) {}

        UCrc32 mType;        // offset 0x0, size 0x4
        unsigned int mCount; // offset 0x4, size 0x4
    };

    typedef UTL::Std::vector<IPursuitAI *, _type_AIPursuers> Pursuers;
    typedef UTL::Std::vector<UMath::Vector3, _type_AIVector3List> Vector3List;
    typedef UTL::Std::vector<AIPursuit::FormationTarget, _type_AIFormationTargetList> FormationTargetList;
    typedef UTL::Std::vector<AIPursuit::CopContingent, _type_AICopContingent> ContingentVector;

    // void *operator new(size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        if (mem) {
            return gFastMem.Free(mem, size, nullptr);
        }
    }

    void *operator new(size_t size, const char *name) {
        return gFastMem.Alloc(size, name);
    }

    // void operator delete(void *mem, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {
        if (mem) {
            return gFastMem.Free(mem, size, name);
        }
    }

    AIPursuit(Sim::Param params);

    // Overrides: IUnknown
    ~AIPursuit() override;

    static Sim::IActivity *Construct(Sim::Param params);

    // Overrides: IPursuit
    Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() const override;

    Attrib::Gen::pursuitsupport *GetPursuitSupportAttrib() const;

    // Overrides: IPursuit
    void LockInPursuitAttribs() override;

    // Overrides: IPursuit
    uint32 CalcTotalCostToState() const override;

    // Overrides: IPursuit
    void AddVehicleToContingent(IVehicle *ivehicle) override;

    // Overrides: IAttachable
    void OnAttached(IAttachable *pOther) override;

    // Overrides: IAttachable
    void OnDetached(IAttachable *pOther) override;

    // Overrides: IPursuit
    void IncNumCopsDestroyed(IVehicle *ivehicle) override;

    void TrackVehicleCounts();

    // Overrides: IPursuit
    enum FormationType GetFormationType() const override;

    void InitFormation(int numCops);

    // Overrides: IPursuit
    void EndCurrentFormation() override;

    void AssignCopOffset(int cop, Pursuers &assignCopList, const UMath::Vector3 &pursuitOffset, const UMath::Vector3 &inPositionOffset,
                         const UCrc32 &ipg, bool information);

    void AssignChopperGoal(IPursuitAI *pursuitChopper);

    void EvenOutOffsets(Vector3List &copRelativePositions, FormationTargetList &formationOffsets);

    void AssignClosestOffsets(Vector3List &copRelativePositions, Pursuers &assignCopList, FormationTargetList &formationOffsets, bool information);

    bool SetupCollapse(const Pursuers &cops, int max_inner, float inner_radius, float outer_radius);

    void AssignCopsInCircle(CopAndAngle *copangles, int num, float radius, const UMath::Vector3 &front, const UMath::Vector3 &side);

    void UpdateFormation(float dT);

    void UpdateOutOfFormationOffsets();

    // Overrides: IPursuit
    bool IsPlayerPursuit() const override;

    // Overrides: IPursuit
    bool ContingentHasActiveCops() const override;

    // Overrides: ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // Overrides: IPursuit
    bool IsHeliInPursuit() const override;

    // Overrides: IPursuit
    bool ShouldEnd() const override;

    void GetAdjustedCopCounts(CopCountRecord *counts, int &numcounts);

    void RemoveUnwantedVehicles();

    void FleeCopOfType(UCrc32 type, int fleecount);

    // Overrides: IPursuit
    const char *CopRequest() override;

    // Overrides: IPursuit
    int RequestRoadBlock() override;

    // Overrides: IPursuit
    void AddRoadBlock(IRoadBlock *roadblock) override;

    // Overrides: IPursuit
    void ClearGroundSupportRequest() override;

    // Overrides: IPursuit
    bool SkidHitEnabled() const override;

    // Overrides: IPursuit
    GroundSupportRequest *RequestGroundSupport() override;

    // Overrides: IPursuit
    bool IsSupportVehicle(IVehicle *iv) override;

    // Overrides: IPursuit
    bool IsTarget(AITarget *aitarget) const override;

    // Overrides: IPursuit
    AITarget *GetTarget() const override;

    // Overrides: IPursuit
    bool IsFinisherActive() const override;

    // Overrides: IPursuit
    float TimeToFinisherAttempt() const override;

    // Overrides: IPursuit
    void BailPursuit() override;

    // Overrides: IPursuit
    float TimeUntilBusted() const override;

    bool IsAttemptingRoadBlock() const;

    // Overrides: IPursuit
    void NotifyCopDamaged(IVehicle *ivehicle) override;

    virtual void OnDebugDraw();

    // Overrides: IPursuit
    void SpikesHit(IVehicleAI *ivai) override;

    // Overrides: IPursuit
    void EndPursuitEnteringSafehouse() override;

    void UpdateJerk(float dt);

    // Overrides: IPursuit
    int GetNumCops() const override {
        return mIVehicleList.size();
    }

    // Overrides: IPursuit
    bool IsPerpBusted() const override {
        return mIsPerpBusted;
    }

    // Overrides: IPursuit
    bool AddVehicle(IVehicle *vehicle) override {
        bool result = Attach(vehicle);
        return result;
    }

    // Overrides: IPursuit
    bool RemoveVehicle(IVehicle *vehicle) override {
        bool result = Detach(vehicle);
        return result;
    }

    // Overrides: IPursuit
    float GetPursuitDuration() const override {
        return mTotalPursuitTime;
    }

    // Overrides: IPursuit
    float GetEvadeLevel() const override {
        return mEvadeLevel;
    }

    // Overrides: IPursuit
    float GetCoolDownTimeRemaining() const override {
        return mCoolDownTimeRemaining;
    }

    // Overrides: IPursuit
    float GetCoolDownTimeRequired() const override {
        return mCoolDownTimeRequired - 7.0f;
    }

    // Overrides: IPursuit
    float GetBackupETA() const override {
        return mBackupCountdownTimer;
    }

    // Overrides: IPursuit
    bool IsPerpInSight() const override {
        return mIsPerpInSight;
    }

    // Overrides: IPursuit
    bool IsPursuitBailed() const override {
        return mIsPursuitBailed;
    }

    // Overrides: IPursuit
    bool IsCollapseActive() const override {
        return mCollapseActive;
    }

    // Overrides: IPursuit
    bool AttemptingToReAquire() const override {
        return !mIsPerpInSight && !mIsPerpBusted ? !mIsPursuitBailed : false;
    }

    // Overrides: IPursuit
    const UMath::Vector3 &GetLastKnownLocation() const override {
        return mLastKnownLocation;
    }

    // Overrides: IPursuit
    IRoadBlock *GetRoadBlock() const override {
        return mRoadBlock;
    }

    // Overrides: IPursuit
    IVehicle *GetNearestCopInRoadblock(float *distance) override {
        if (distance) {
            *distance = mDistanceToNearestCopInRoadblock;
        }
        return mNearestCopInRoadblock;
    }

    // Overrides: IPursuit
    int GetNumCopsDestroyed() const override {
        return mCopsDestroyed;
    }

    // Overrides: IPursuit
    int GetNumCopsDamaged() const override {
        return mNumCopsDamaged;
    }

    // Overrides: IPursuit
    int GetTotalNumCopsInvolved() const override {
        return mTotalCopsInvolved;
    }

    // Overrides: IPursuit
    int GetNumCopsFullyEngaged() const override {
        return mNumCopsFullyEngaged;
    }

    // Overrides: IPursuit
    void NotifyRoadblockDodged() override {
        mNumRoadblocksDodged++;
    }

    // Overrides: IPursuit
    void NotifyRoadblockDeployed() override {
        mNumRoadblocksDeployed++;
    }

    // Overrides: IPursuit
    void NotifyPropertyDamaged(int cost) override {
        mPropertyDamageValue += cost;
        mPropertyDamageCount++;
    }

    // Overrides: IPursuit
    void NotifyTrafficCarHit() override {
        mNumTrafficCarsHit++;
    }

    // Overrides: IPursuit
    void NotifySpikeStripsDodged(int num) override {
        mNumSpikeStripsDodged += num;
    }

    // Overrides: IPursuit
    void NotifySpikeStripDeployed() override {
        mNumSpikeStripsDeployed++;
    }

    // Overrides: IPursuit
    void NotifyHeliSpikeStripDeployed(int num) override {
        mNumHeliSpikeStripsDeployed += num;
    }

    // Overrides: IPursuit
    void NotifyCopCarDeployed() override {
        mNumCopCarsDeployed++;
    }

    // Overrides: IPursuit
    void NotifySupportVehicleDeployed() override {
        mNumSupportVehiclesDeployed++;
    }

    // Overrides: IPursuit
    int GetNumRoadblocksDodged() const override {
        return mNumRoadblocksDodged;
    }

    // Overrides: IPursuit
    int GetNumRoadblocksDeployed() const override {
        return mNumRoadblocksDeployed;
    }

    // Overrides: IPursuit
    int GetValueOfPropertyDamaged() const override {
        return mPropertyDamageValue;
    }

    // Overrides: IPursuit
    int GetNumPropertyDamaged() const override {
        return mPropertyDamageCount;
    }

    // Overrides: IPursuit
    int GetNumTrafficCarsHit() const override {
        return mNumTrafficCarsHit;
    }

    // Overrides: IPursuit
    int GetNumSpikeStripsDodged() const override {
        return mNumSpikeStripsDodged;
    }

    // Overrides: IPursuit
    int GetNumSpikeStripsDeployed() const override {
        return mNumSpikeStripsDeployed;
    }

    // Overrides: IPursuit
    int GetNumHeliSpikeStripDeployed() const override {
        return mNumHeliSpikeStripsDeployed;
    }

    // Overrides: IPursuit
    int GetNumCopCarsDeployed() const override {
        return mNumCopCarsDeployed;
    }

    // Overrides: IPursuit
    int GetNumSupportVehiclesDeployed() const override {
        return mNumSupportVehiclesDeployed;
    }

    // Overrides: IPursuit
    ePursuitStatus GetPursuitStatus() const override {
        return mPursuitStatus;
    }

    // Overrides: IPursuit
    float GetTimeToBackupSpawned() const override {
        return mBackupCountdownTimer;
    }

    // Overrides: IPursuit
    bool PendingRoadBlockRequest() const override {
        return mNextRoadblockRequest;
    }

    // Overrides: IPursuit
    int GetNumHeliSpawns() const override {
        return mNumHeliSpawns;
    }

    // Overrides: IPursuit
    int GetCopDestroyedBonusMultiplier() const override {
        return mCopDestroyedBonusMultiplier;
    }

    // Overrides: IPursuit
    int GetMostRecentCopDestroyedRepPoints() const override {
        return mMostRecentCopDestroyedRepPoints;
    }

    // Overrides: IPursuit
    UCrc32 GetMostRecentCopDestroyedType() const override {
        return mMostRecentCopDestroyedType;
    }

    // Overrides: IPursuit
    int GetNumCopsInWave() const override {
        return mNumCopsRequiredToEvade;
    }

    // Overrides: IPursuit
    int GetNumCopsRemainingInWave() const override {
        return UMath::Max(mNumCopsRequiredToEvade - mNumFullyEngagedCopsEvaded, 1);
    }

    // Overrides: IPursuit
    bool PursuitMeterCanShowBusted() const override {
        return !mCoolDownMeterDisplayed || mPursuitMeterModeTimer > 2.5f;
    }

    // Overrides: IPursuit
    bool GetIsAJerk() const override {
        return mIsAJerk;
    }

    // Overrides: IPursuit
    float GetMinDistanceToTarget() const override {
        return mMinDistanceToTarget;
    }

    // Overrides: IPursuit
    void SetBustedTimerToZero() override {
        mBustedTimer = 0.0f;
    }

    // Overrides: IPursuit
    bool GetEnterSafehouseOnDone() override {
        return mEnterSafehouseOnDestruct;
    }

  private:
    HSIMTASK mSimulateTask;                     // offset 0x5C, size 0x4
    HSIMTASK mBustedTimerTask;                  // offset 0x60, size 0x4
    IVehicle::List mIVehicleList;               // offset 0x64, size 0x38
    AITarget *mTarget;                          // offset 0x9C, size 0x4
    IVehicle *mNearestCopInRoadblock;           // offset 0xA0, size 0x4
    float mDistanceToNearestCopInRoadblock;     // offset 0xA4, size 0x4
    PursuitFormation *mFormation;               // offset 0xA8, size 0x4
    IRoadBlock *mRoadBlock;                     // offset 0xAC, size 0x4
    ContingentVector mCopContingent;            // offset 0xB0, size 0x10
    int mCurrentPursuitLevel;                   // offset 0xC0, size 0x4
    float mBaseHeat;                            // offset 0xC4, size 0x4
    float mMaximumHeat;                         // offset 0xC8, size 0x4
    float mHeatScale;                           // offset 0xCC, size 0x4
    bool mAllowStatsToAccumulate;               // offset 0xD0, size 0x1
    float mInFormationTimer;                    // offset 0xD4, size 0x4
    float mBreakerTimer;                        // offset 0xD8, size 0x4
    float mTotalPursuitTime;                    // offset 0xDC, size 0x4
    bool mCollapseActive;                       // offset 0xE0, size 0x1
    int mFormationAttemptCount;                 // offset 0xE4, size 0x4
    FormationType mActiveFormation;             // offset 0xE8, size 0x4
    float mActiveFormationTime;                 // offset 0xEC, size 0x4
    float mRoadBlockTimer;                      // offset 0xF0, size 0x4
    float mSpawnCopTimer;                       // offset 0xF4, size 0x4
    float mSpawnHeliTimer;                      // offset 0xF8, size 0x4
    bool mDoTestForHeliSearch;                  // offset 0xFC, size 0x1
    bool mForceHeliSpawnNext;                   // offset 0x100, size 0x1
    Timer mTimeSinceSetupSpeech;                // offset 0x104, size 0x4
    float mBustedTimer;                         // offset 0x108, size 0x4
    float mBustedIncrement;                     // offset 0x10C, size 0x4
    float mBustedHUDTime;                       // offset 0x110, size 0x4
    bool mIsPerpBusted;                         // offset 0x114, size 0x1
    bool mIsPursuitBailed;                      // offset 0x118, size 0x1
    float mCopDestroyedBonusTimer;              // offset 0x11C, size 0x4
    int mCopDestroyedBonusMultiplier;           // offset 0x120, size 0x4
    int mMostRecentCopDestroyedRepPoints;       // offset 0x124, size 0x4
    UCrc32 mMostRecentCopDestroyedType;         // offset 0x128, size 0x4
    float mEvadeLevel;                          // offset 0x12C, size 0x4
    float mCoolDownTimeRemaining;               // offset 0x130, size 0x4
    float mCoolDownTimeRequired;                // offset 0x134, size 0x4
    float mPercentOfContingentEngaged;          // offset 0x138, size 0x4
    int mNumCopsFullyEngaged;                   // offset 0x13C, size 0x4
    float mPursuitMeter;                        // offset 0x140, size 0x4
    bool mIsPerpInSight;                        // offset 0x144, size 0x1
    UMath::Vector3 mLastKnownLocation;          // offset 0x148, size 0xC
    float mHiddenZoneTime;                      // offset 0x154, size 0x4
    float mTimeSinceAnyCopSawPerp;              // offset 0x158, size 0x4
    bool mCoolDownMeterDisplayed;               // offset 0x15C, size 0x1
    float mPursuitMeterModeTimer;               // offset 0x160, size 0x4
    int mRepPointsPerMinute;                    // offset 0x164, size 0x4
    int mTotalCopsInvolved;                     // offset 0x168, size 0x4
    int mCopsDestroyed;                         // offset 0x16C, size 0x4
    int mRepPointsFromCopsDisabled;             // offset 0x170, size 0x4
    int mNumCopsRequiredToEvade;                // offset 0x174, size 0x4
    int mNumCopsToTriggerBackupTime;            // offset 0x178, size 0x4
    int mNumFullyEngagedCopsEvaded;             // offset 0x17C, size 0x4
    int mNumHeliSpawns;                         // offset 0x180, size 0x4
    int mNumRoadblocksDodged;                   // offset 0x184, size 0x4
    int mNumRoadblocksDeployed;                 // offset 0x188, size 0x4
    int mNumCopsDamaged;                        // offset 0x18C, size 0x4
    int mNumCopsNeeded;                         // offset 0x190, size 0x4
    eCrossState mCrossState;                    // offset 0x194, size 0x4
    int mNumTrafficCarsHit;                     // offset 0x198, size 0x4
    int mNumSpikeStripsDodged;                  // offset 0x19C, size 0x4
    bool mFastSpawnNext;                        // offset 0x1A0, size 0x1
    int mPropertyDamageValue;                   // offset 0x1A4, size 0x4
    int mPropertyDamageCount;                   // offset 0x1A8, size 0x4
    int mNumSpikeStripsDeployed;                // offset 0x1AC, size 0x4
    int mNumHeliSpikeStripsDeployed;            // offset 0x1B0, size 0x4
    int mNumCopCarsDeployed;                    // offset 0x1B4, size 0x4
    int mNumSupportVehiclesDeployed;            // offset 0x1B8, size 0x4
    int mNumSupportVehiclesActive;              // offset 0x1BC, size 0x4
    bool mNextRoadblockRequest;                 // offset 0x1C0, size 0x1
    GroundSupportRequest mGroundSupportRequest; // offset 0x1C4, size 0x50
    float mSupportCheckTimer;                   // offset 0x214, size 0x4
    bool mSupportPriorityCheckDone;             // offset 0x218, size 0x1
    ePursuitStatus mPursuitStatus;              // offset 0x21C, size 0x4
    float mBackupCountdownTimer;                // offset 0x220, size 0x4
    float mMinDistanceToTarget;                 // offset 0x224, size 0x4
    UMath::Vector3 mJerkLagPosition;            // offset 0x228, size 0xC
    float mJerkLagDistance;                     // offset 0x234, size 0x4
    float mJerkLagSpeed;                        // offset 0x238, size 0x4
    bool mIsAJerk;                              // offset 0x23C, size 0x1
    int mNumRBCopsAdded;                        // offset 0x240, size 0x4
    bool mEnterSafehouseOnDestruct;             // offset 0x244, size 0x1
};

bool IsValidPursuitCarName(const char *name);
const char *GetRandomValidCopCar();
Attrib::Gen::pursuitlevels *GetGlobalPursuitLevelAttrib();

#endif
