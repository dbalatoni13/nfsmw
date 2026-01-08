#ifndef INTERFACES_SIMABLES_IAI_H
#define INTERFACES_SIMABLES_IAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitescalation.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// TODO move
enum eLaneSelection {
    SELECT_VALID_LANE = 2,
    SELECT_CURRENT_LANE = 1,
    SELECT_CENTER_LANE = 0,
};

struct IVehicle;

struct _type_IRoadBlockVehicles {
    const char *name() {
        return "IRoadBlockVehicles";
    }
};

struct _type_IRoadBlockSmackables {
    const char *name() {
        return "IRoadBlockSmackables";
    }
};

struct IPursuit;

class IRoadBlock : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRoadBlock, 8> {
  protected:
    virtual ~IRoadBlock();

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRoadBlock(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    typedef struct UTL::Std::vector<IVehicle *, _type_IRoadBlockVehicles> Vehicles;
    typedef struct UTL::Std::vector<IPlaceableScenery *, _type_IRoadBlockSmackables> Smackables;

    virtual void SetPursuit(IPursuit *pursuit);
    virtual IPursuit *GetPursuit();
    virtual void SetDodged(bool dodged);
    virtual bool GetDodged();
    virtual void IncNumCopsDestroyed();
    virtual short GetNumCopsDestroyed();
    virtual void IncNumCopsDamaged();
    virtual short GetNumCopsDamaged();
    virtual const UMath::Vector3 &GetRoadBlockCentre();
    virtual const UMath::Vector3 &GetRoadBlockDir();
    virtual void SetRoadBlockCentre(const UMath::Vector3 &centre, const UMath::Vector3 &dir);
    virtual short GetNumSpikeStrips();
    virtual const Vehicles &GetVehicles() const;
    virtual const Smackables &GetSmackables() const;
    virtual bool IsPerpCheating() const;
    virtual bool AddVehicle(IVehicle *vehicle);
    virtual void AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip);
    virtual bool RemoveVehicle(IVehicle *vehicle);
    virtual void ReleaseAllSmackables();
    virtual float GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle);
    virtual int GetNumCops();
    virtual IVehicle *IsComprisedOf(HSIMABLE obj);
};

class IVehicleAI : public UTL::COM::IUnknown {
  protected:
    virtual ~IVehicleAI();

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IVehicleAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ISimable *GetSimable() const;
    virtual IVehicle *GetVehicle() const;
    virtual const struct AISplinePath *GetSplinePath();
    virtual void SetReverse(float time);
    virtual bool GetReverse();
    virtual unsigned int GetDriveFlags() const;
    virtual void ClearDriveFlags();
    virtual void DoReverse();
    virtual void DoSteering();
    virtual void DoGasBrake();
    virtual void DoDriving(unsigned int flags);
    virtual void DoNOS();
    virtual float GetDriveSpeed() const;
    virtual void SetDriveSpeed(float driveSpeed);
    virtual void SetDriveTarget(const UMath::Vector3 &dest);
    virtual float GetLookAhead();
    virtual const UMath::Vector3 &GetDriveTarget() const;
    virtual WRoadNav *GetDriveToNav();
    virtual bool GetDrivableToDriveToNav();
    virtual void ResetDriveToNav(eLaneSelection lane_selection);
    virtual bool ResetVehicleToRoadNav(WRoadNav *other_nav);
    virtual bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep);
    virtual bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector);
    virtual float GetPathDistanceRemaining();
    virtual struct AITarget *GetTarget() const;
    virtual bool GetDrivableToTargetPos() const;
    virtual const AvoidableList &GetAvoidableList();
    virtual void SetAvoidableRadius(float radius);
    virtual float GetTopSpeed() const;
    virtual float GetAcceleration(float at) const;
    virtual bool GetWorldAvoidanceInfo(float dT, UMath::Vector3 &leftCollNormal, UMath::Vector3 &rightCollNormal) const;
    virtual WRoadNav *GetCollNav(const UMath::Vector3 &forwardVector, float predictTime);
    virtual float GetLastSpawnTime();
    virtual void SetSpawned();
    virtual void UnSpawn();
    virtual bool CanRespawn(bool respawnAvailable);
    virtual const Attrib::Gen::aivehicle &GetAttributes() const;
    virtual void EnableSimplePhysics();
    virtual void DisableSimplePhysics();
    virtual IPursuit *GetPursuit();
    virtual IRoadBlock *GetRoadBlock();
    virtual const UMath::Vector3 &GetSeekAheadPosition();
    virtual const UMath::Vector3 &GetFarFuturePosition();
    virtual const UMath::Vector3 &GetFarFutureDirection();
    virtual WRoadNav *GetCurrentRoad();
    virtual WRoadNav *GetFutureRoad();
    virtual bool IsCurrentGoal(const UCrc32 &);
    virtual UCrc32 &GetGoalName();
    virtual bool IsCurrentAction(const UCrc32 &);
    virtual UCrc32 GetActionName();
    virtual float GetSkill() const;
    virtual float GetShortcutSkill() const;
    virtual float GetPercentRaceComplete() const;
};

class ITrafficAI : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITrafficAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~ITrafficAI();

  public:
    virtual void StartDriving(float speed);
};

class IHumanAI : public UTL::COM::IUnknown {
  protected:
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHumanAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IHumanAI() override {}

  public:
    virtual void ChangeDragLanes(bool left);
    virtual bool IsPlayerSteering();
    virtual bool GetAiControl() const;
    virtual void SetAiControl(bool ai_control);
    virtual void SetWorldMoment(const UMath::Vector3 &position, float radius);
    virtual const UMath::Vector3 &GetWorldMomentPosition();
    virtual float GetWorldMomentRadius();
    virtual void ClearWorldMoment();
    virtual bool IsFacingWrongWay() const;
};

// TODO where with these two?
enum ePursuitStatus {
    PS_INITIAL_CHASE = 0,
    PS_BACKUP_REQUESTED = 1,
    PS_COOL_DOWN = 2,
    PS_BUSTED = 3,
    PS_EVADED = 4,
};

// total size: 0x50
struct GroundSupportRequest {
    enum Status {
        NOT_ACTIVE = 0,
        PENDING = 1,
        ACTIVE = 2,
    };

    GroundSupportRequest() {}

    ~GroundSupportRequest() {}

    void Reset();
    void Update(float dT);

    const HeavySupport *mHeavySupport;                // offset 0x0, size 0x4
    const LeaderSupport *mLeaderSupport;              // offset 0x4, size 0x4
    IVehicle::List mIVehicleList;                     // offset 0x8, size 0x38
    UTL::Std::list<char *, _type_list> mVehicleGoals; // offset 0x40, size 0x8
    float mSupportTimer;                              // offset 0x48, size 0x4
    Status mSupportRequestStatus;                     // offset 0x4C, size 0x4
};

class IPursuit : public UTL::COM::IUnknown, public UTL::Collections::Listable<IPursuit, 8> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IPursuit(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IPursuit() override;

  public:
    virtual bool IsTarget(AITarget *aitarget) const;
    virtual AITarget *GetTarget() const;
    virtual int GetNumCops() const;
    virtual int GetNumHeliSpawns() const;
    virtual int GetNumCopsFullyEngaged() const;
    virtual float GetPursuitDuration() const;
    virtual float GetEvadeLevel() const;
    virtual float GetCoolDownTimeRemaining() const;
    virtual float GetCoolDownTimeRequired() const;
    virtual bool IsPerpInSight() const;
    virtual bool IsPursuitBailed() const;
    virtual bool IsCollapseActive() const;
    virtual bool AttemptingToReAquire() const;
    virtual const UMath::Vector3 &GetLastKnownLocation() const;
    virtual IRoadBlock *GetRoadBlock() const;
    virtual IVehicle *GetNearestCopInRoadblock(float *distance);
    virtual GroundSupportRequest *RequestGroundSupport();
    virtual void ClearGroundSupportRequest();
    virtual bool IsSupportVehicle(IVehicle *iv);
    virtual const char *CopRequest();
    virtual bool PendingRoadBlockRequest() const;
    virtual bool IsFinisherActive() const;
    virtual float TimeToFinisherAttempt() const;
    virtual float TimeUntilBusted() const;
    virtual bool PursuitMeterCanShowBusted() const;
    virtual FormationType GetFormationType() const;
    virtual void EndCurrentFormation();
    virtual bool ShouldEnd() const;
    virtual bool IsPerpBusted() const;
    virtual bool AddVehicle(IVehicle *vehicle);
    virtual bool RemoveVehicle(IVehicle *vehicle);
    virtual void AddRoadBlock(IRoadBlock *roadblock);
    virtual int RequestRoadBlock();
    virtual bool IsHeliInPursuit() const;
    virtual bool IsPlayerPursuit() const;
    virtual bool ContingentHasActiveCops() const;
    virtual int GetNumCopsDamaged() const;
    virtual int GetNumCopsDestroyed() const;
    virtual void IncNumCopsDestroyed(IVehicle *ivehicle);
    virtual int GetTotalNumCopsInvolved() const;
    virtual int GetNumRoadblocksDodged() const;
    virtual int GetNumRoadblocksDeployed() const;
    virtual int GetValueOfPropertyDamaged() const;
    virtual int GetNumPropertyDamaged() const;
    virtual int GetNumTrafficCarsHit() const;
    virtual int GetNumSpikeStripsDodged() const;
    virtual int GetNumSpikeStripsDeployed() const;
    virtual int GetNumHeliSpikeStripDeployed() const;
    virtual int GetNumCopCarsDeployed() const;
    virtual int GetNumSupportVehiclesDeployed() const;
    virtual int GetNumCopsInWave() const;
    virtual int GetNumCopsRemainingInWave() const;
    virtual int GetCopDestroyedBonusMultiplier() const;
    virtual int GetMostRecentCopDestroyedRepPoints() const;
    virtual UCrc32 GetMostRecentCopDestroyedType() const;
    virtual uint32 CalcTotalCostToState() const;
    virtual void AddVehicleToContingent(IVehicle *ivehicle);
    virtual void NotifyRoadblockDodged();
    virtual void NotifyRoadblockDeployed();
    virtual void NotifyCopDamaged(IVehicle *ivehicle);
    virtual void NotifyPropertyDamaged(int cost);
    virtual void NotifyTrafficCarHit();
    virtual void NotifySpikeStripsDodged(int num);
    virtual void NotifySpikeStripDeployed();
    virtual void NotifyHeliSpikeStripDeployed(int num);
    virtual void NotifyCopCarDeployed();
    virtual void NotifySupportVehicleDeployed();
    virtual void BailPursuit();
    virtual ePursuitStatus GetPursuitStatus() const;
    virtual float GetTimeToBackupSpawned() const;
    virtual bool SkidHitEnabled() const;
    virtual float GetBackupETA() const;
    virtual bool GetIsAJerk() const;
    virtual float GetMinDistanceToTarget() const;
    virtual void SpikesHit(IVehicleAI *ivai);
    virtual void EndPursuitEnteringSafehouse();
    virtual bool GetEnterSafehouseOnDone();
    virtual void LockInPursuitAttribs();
    virtual void SetBustedTimerToZero();
    virtual Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() const;
};

// TODO where?
enum SirenState {
    SIREN_OFF = -1,
    SIREN_WAIL = 0,
    SIREN_YELP = 1,
    SIREN_SCREAM = 2,
    SIREN_DIE = 3,
};

class IPursuitAI : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IPursuitAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IPursuitAI() override;

  public:
    virtual void StartPatrol();
    virtual void StartRoadBlock();
    virtual void StartFlee();
    virtual void SetInPursuit(bool inPursuit) {}
    virtual bool GetInPursuit() {}
    virtual void StartPursuit(AITarget *target, ISimable *itargetSimable);
    virtual void DoInPositionGoal();
    virtual void EndPursuit();
    virtual AITarget *GetPursuitTarget();
    virtual bool StartSupportGoal();
    virtual AITarget *PursuitRequest();
    virtual void SetInFormation(bool inFormation);
    virtual bool GetInFormation();
    virtual void SetInPosition(bool inPosition);
    virtual bool GetInPosition();
    virtual void SetPursuitOffset(const UMath::Vector3 &offset);
    virtual const UMath::Vector3 &GetPursuitOffset() const;
    virtual void SetInPositionGoal(const UCrc32 &ipg);
    virtual const UCrc32 &GetInPositionGoal() const;
    virtual void SetInPositionOffset(const UMath::Vector3 &offset);
    virtual const UMath::Vector3 &GetInPositionOffset() const;
    virtual void SetBreaker(bool breaker);
    virtual bool GetBreaker();
    virtual void SetChicken(bool chicken);
    virtual bool GetChicken();
    virtual void SetDamagedByPerp(bool damaged);
    virtual bool GetDamagedByPerp();
    virtual SirenState GetSirenState() const;
    virtual float GetTimeSinceTargetSeen() const;
    virtual void ZeroTimeSinceTargetSeen();
    virtual bool CanSeeTarget(AITarget *target);
    virtual const UCrc32 &GetSupportGoal() const;
    virtual void SetSupportGoal(UCrc32 sg);
    virtual void SetWithinEngagementRadius();
    virtual bool WasWithinEngagementRadius() const;
};

class IPerpetrator : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IPerpetrator(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IPerpetrator() override {}

  public:
    static const int MaxiumumHeat;

    // Virtual methods
    virtual float GetHeat() const;
    virtual void SetHeat(float heat);
    virtual Attrib::Gen::pursuitescalation *GetPursuitEscalationAttrib();
    virtual Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib();
    virtual Attrib::Gen::pursuitsupport *GetPursuitSupportAttrib();
    virtual void AddCostToState(int cost);
    virtual int GetCostToState() const;
    virtual void SetCostToState(int cts);
    virtual bool IsRacing() const;
    virtual bool IsBeingPursued() const;
    virtual bool IsHiddenFromCars() const;
    virtual bool IsHiddenFromHelicopters() const;
    virtual bool IsPartiallyHidden(float &HowHidden) const;
    virtual void AddToPendingRepPointsFromCopDestruction(int amount);
    virtual void AddToPendingRepPointsNormal(int amount);
    virtual int GetPendingRepPointsNormal() const;
    virtual int GetPendingRepPointsFromCopDestruction() const;
    virtual void ClearPendingRepPoints();
    virtual void SetRacerInfo(GRacerInfo *info);
    virtual GRacerInfo *GetRacerInfo() const;
    virtual float GetLastTrafficHitTime() const;
    virtual void Set911CallTime(float time);
    virtual float Get911CallTime() const;
};

#endif
