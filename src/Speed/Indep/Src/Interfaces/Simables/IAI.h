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

// TODO move?
enum eLaneSelection {
    SELECT_CENTER_LANE = 0,
    SELECT_CURRENT_LANE = 1,
    SELECT_VALID_LANE = 2,
};

struct IVehicle;

DECLARE_CONTAINER_TYPE(IRoadBlockVehicles);
DECLARE_CONTAINER_TYPE(IRoadBlockSmackables);

struct IPursuit;

class IRoadBlock : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRoadBlock, 8> {
  protected:
    ~IRoadBlock() override {}

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRoadBlock(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    typedef struct UTL::Std::vector<IVehicle *, _type_IRoadBlockVehicles> Vehicles;
    typedef struct UTL::Std::vector<IPlaceableScenery *, _type_IRoadBlockSmackables> Smackables;

    virtual bool AddVehicle(IVehicle *vehicle) = 0;
    virtual void AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) = 0;
    virtual bool RemoveVehicle(IVehicle *vehicle) = 0;
    virtual void ReleaseAllSmackables() = 0;
    virtual int GetNumCops() = 0;
    virtual void SetPursuit(IPursuit *pursuit) = 0;
    virtual IPursuit *GetPursuit() = 0;
    virtual void SetDodged(bool dodged) = 0;
    virtual bool GetDodged() = 0;
    virtual short GetNumCopsDamaged() = 0;
    virtual short GetNumCopsDestroyed() = 0;
    virtual void IncNumCopsDamaged() = 0;
    virtual void IncNumCopsDestroyed() = 0;
    virtual const UMath::Vector3 &GetRoadBlockCentre() = 0;
    virtual const UMath::Vector3 &GetRoadBlockDir() = 0;
    virtual void SetRoadBlockCentre(const UMath::Vector3 &centre, const UMath::Vector3 &dir) = 0;
    virtual float GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) = 0;
    virtual short GetNumSpikeStrips() = 0;
    virtual bool IsPerpCheating() const = 0;
    virtual const Vehicles &GetVehicles() const = 0;
    virtual const Smackables &GetSmackables() const = 0;
    virtual IVehicle *IsComprisedOf(HSIMABLE obj) = 0;
};

class IVehicleAI : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IVehicleAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IVehicleAI() {}

    virtual ISimable *GetSimable() const = 0;
    virtual IVehicle *GetVehicle() const = 0;
    virtual const struct AISplinePath *GetSplinePath() = 0;
    virtual void SetReverseOverride(float time) = 0;
    virtual bool GetReverseOverride() = 0;
    virtual unsigned int GetDriveFlags() const = 0;
    virtual void ClearDriveFlags() = 0;
    virtual void DoReverse() = 0;
    virtual void DoSteering() = 0;
    virtual void DoGasBrake() = 0;
    virtual void DoDriving(unsigned int flags) = 0;
    virtual void DoNOS() = 0;
    virtual float GetDriveSpeed() = 0;
    virtual void SetDriveSpeed(float driveSpeed) = 0;
    virtual void SetDriveTarget(const UMath::Vector3 &dest) = 0;
    virtual float GetLookAhead() = 0;
    virtual const UMath::Vector3 &GetDriveTarget() = 0;
    virtual WRoadNav *GetDriveToNav() = 0;
    virtual bool GetDrivableToDriveToNav() const = 0;
    virtual void ResetDriveToNav(eLaneSelection lane_selection) = 0;
#ifdef EA_BUILD_A124
    virtual void ResetDriveToNav(UMath::Vector3 &target) = 0;
#endif
    virtual bool ResetVehicleToRoadNav(WRoadNav *other_nav) = 0;
    virtual bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep) = 0;
    virtual bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector) = 0;
    virtual float GetPathDistanceRemaining() = 0;
    virtual struct AITarget *GetTarget() const = 0;
    virtual bool GetDrivableToTargetPos() const = 0;
    virtual const AvoidableList &GetAvoidableList() = 0;
    virtual void SetAvoidableRadius(float radius) = 0;
    virtual float GetTopSpeed() const = 0;
    virtual float GetAcceleration(float at) const = 0;
    virtual bool GetWorldAvoidanceInfo(float dT, UMath::Vector3 &leftCollNormal, UMath::Vector3 &rightCollNormal) const = 0;
    virtual WRoadNav *GetCollNav(const UMath::Vector3 &forwardVector, float predictTime) = 0;
    virtual float GetLastSpawnTime() = 0;
    virtual void SetSpawned() = 0;
    virtual void UnSpawn() = 0;
    virtual bool CanRespawn(bool respawnAvailable) = 0;
    virtual const Attrib::Gen::aivehicle &GetAttributes() const = 0;
    virtual void EnableSimplePhysics() = 0;
    virtual void DisableSimplePhysics() = 0;
    virtual IPursuit *GetPursuit() = 0;
    virtual IRoadBlock *GetRoadBlock() = 0;
    virtual const UMath::Vector3 &GetSeekAheadPosition() = 0;
    virtual const UMath::Vector3 &GetFarFuturePosition() = 0;
    virtual const UMath::Vector3 &GetFarFutureDirection() = 0;
    virtual WRoadNav *GetCurrentRoad() = 0;
    virtual WRoadNav *GetFutureRoad() = 0;
    virtual bool IsCurrentGoal(const UCrc32 &name) = 0;
    virtual UCrc32 &GetGoalName() = 0;
    virtual bool IsCurrentAction(const UCrc32 &name) = 0;
    virtual UCrc32 GetActionName() = 0;
    virtual float GetSkill() const = 0;
    virtual float GetShortcutSkill() const = 0;
    virtual float GetPercentRaceComplete() const = 0;
};

class ITrafficAI : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITrafficAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~ITrafficAI() override {}

  public:
    virtual void StartDriving(float speed) = 0;
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
    virtual void ChangeDragLanes(bool left) = 0;
    virtual bool IsPlayerSteering() = 0;
    virtual bool GetAiControl() = 0;
    virtual void SetAiControl(bool ai_control) = 0;
    virtual void SetWorldMoment(const UMath::Vector3 &position, float radius) = 0;
    virtual const UMath::Vector3 &GetWorldMomentPosition() = 0;
    virtual float GetWorldMomentRadius() = 0;
    virtual void ClearWorldMoment() = 0;
    virtual bool IsFacingWrongWay() const = 0;
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

    GroundSupportRequest()
        : mHeavySupport(nullptr),  //
          mLeaderSupport(nullptr), //
          mSupportTimer(0.0f),     //
          mSupportRequestStatus(NOT_ACTIVE) {}

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
    ~IPursuit() override {}

  public:
    virtual bool IsTarget(AITarget *aitarget) const = 0;
    virtual AITarget *GetTarget() const = 0;
    virtual int GetNumCops() const = 0;
    virtual int GetNumHeliSpawns() const = 0;
    virtual int GetNumCopsFullyEngaged() const = 0;
    virtual float GetPursuitDuration() const = 0;
    virtual float GetEvadeLevel() const = 0;
    virtual float GetCoolDownTimeRemaining() const = 0;
    virtual float GetCoolDownTimeRequired() const = 0;
    virtual int IsPerpInSight() const = 0;
    virtual int IsPursuitBailed() const = 0;
    virtual bool IsCollapseActive() const = 0;
    virtual bool AttemptingToReAquire() const = 0;
    virtual const UMath::Vector3 &GetLastKnownLocation() const = 0;
    virtual IRoadBlock *GetRoadBlock() const = 0;
    virtual IVehicle *GetNearestCopInRoadblock(float *distance) = 0;
    virtual GroundSupportRequest *RequestGroundSupport() = 0;
    virtual void ClearGroundSupportRequest() = 0;
    virtual bool IsSupportVehicle(IVehicle *iv) = 0;
    virtual const char *CopRequest() = 0;
    virtual bool PendingRoadBlockRequest() const = 0;
    virtual bool IsFinisherActive() const = 0;
    virtual float TimeToFinisherAttempt() const = 0;
    virtual float TimeUntilBusted() const = 0;
    virtual bool PursuitMeterCanShowBusted() const = 0;
    virtual FormationType GetFormationType() const = 0;
    virtual void EndCurrentFormation() = 0;
    virtual bool ShouldEnd() const = 0;
    virtual int IsPerpBusted() const = 0;
    virtual bool AddVehicle(IVehicle *vehicle) = 0;
    virtual bool RemoveVehicle(IVehicle *vehicle) = 0;
    virtual void AddRoadBlock(IRoadBlock *roadblock) = 0;
    virtual int RequestRoadBlock() = 0;
    virtual bool IsHeliInPursuit() const = 0;
    virtual bool IsPlayerPursuit() const = 0;
    virtual bool ContingentHasActiveCops() const = 0;
    virtual int GetNumCopsDamaged() const = 0;
    virtual int GetNumCopsDestroyed() const = 0;
    virtual void IncNumCopsDestroyed(IVehicle *ivehicle) = 0;
    virtual int GetTotalNumCopsInvolved() const = 0;
    virtual int GetNumRoadblocksDodged() const = 0;
    virtual int GetNumRoadblocksDeployed() const = 0;
    virtual int GetValueOfPropertyDamaged() const = 0;
    virtual int GetNumPropertyDamaged() const = 0;
    virtual int GetNumTrafficCarsHit() const = 0;
    virtual int GetNumSpikeStripsDodged() const = 0;
    virtual int GetNumSpikeStripsDeployed() const = 0;
    virtual int GetNumHeliSpikeStripDeployed() const = 0;
    virtual int GetNumCopCarsDeployed() const = 0;
    virtual int GetNumSupportVehiclesDeployed() const = 0;
    virtual int GetNumCopsInWave() const = 0;
    virtual int GetNumCopsRemainingInWave() const = 0;
    virtual int GetCopDestroyedBonusMultiplier() const = 0;
    virtual int GetMostRecentCopDestroyedRepPoints() const = 0;
    virtual UCrc32 GetMostRecentCopDestroyedType() const = 0;
    virtual uint32 CalcTotalCostToState() const = 0;
    virtual void AddVehicleToContingent(IVehicle *ivehicle) = 0;
    virtual void NotifyRoadblockDodged() = 0;
    virtual void NotifyRoadblockDeployed() = 0;
    virtual void NotifyCopDamaged(IVehicle *ivehicle) = 0;
    virtual void NotifyPropertyDamaged(int cost) = 0;
    virtual void NotifyTrafficCarHit() = 0;
    virtual void NotifySpikeStripsDodged(int num) = 0;
    virtual void NotifySpikeStripDeployed() = 0;
    virtual void NotifyHeliSpikeStripDeployed(int num) = 0;
    virtual void NotifyCopCarDeployed() = 0;
    virtual void NotifySupportVehicleDeployed() = 0;
    virtual void BailPursuit() = 0;
    virtual ePursuitStatus GetPursuitStatus() const = 0;
    virtual float GetTimeToBackupSpawned() const = 0;
    virtual bool SkidHitEnabled() const = 0;
    virtual float GetBackupETA() const = 0;
    virtual bool GetIsAJerk() const = 0;
    virtual float GetMinDistanceToTarget() const = 0;
    virtual void SpikesHit(IVehicleAI *ivai) = 0;
    virtual void EndPursuitEnteringSafehouse() = 0;
    virtual bool GetEnterSafehouseOnDone() = 0;
    virtual void LockInPursuitAttribs() = 0;
    virtual void SetBustedTimerToZero() = 0;
    virtual Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() const = 0;
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
    ~IPursuitAI() override {}

  public:
    virtual void StartPatrol() = 0;
    virtual void StartRoadBlock() = 0;
    virtual void StartFlee() = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual bool GetInPursuit() = 0;
    virtual void StartPursuit(AITarget *target, ISimable *itargetSimable) = 0;
    virtual void DoInPositionGoal() = 0;
    virtual void EndPursuit() = 0;
    virtual AITarget *GetPursuitTarget() = 0;
    virtual bool StartSupportGoal() = 0;
    virtual AITarget *PursuitRequest() = 0;
    virtual void SetInFormation(bool inFormation) = 0;
    virtual bool GetInFormation() = 0;
    virtual void SetInPosition(bool inPosition) = 0;
    virtual bool GetInPosition() = 0;
    virtual void SetPursuitOffset(const UMath::Vector3 &offset) = 0;
    virtual const UMath::Vector3 &GetPursuitOffset() const = 0;
    virtual void SetInPositionGoal(const UCrc32 &ipg) = 0;
    virtual const UCrc32 &GetInPositionGoal() const = 0;
    virtual void SetInPositionOffset(const UMath::Vector3 &offset) = 0;
    virtual const UMath::Vector3 &GetInPositionOffset() const = 0;
    virtual void SetBreaker(bool breaker) = 0;
    virtual bool GetBreaker() = 0;
    virtual void SetChicken(bool chicken) = 0;
    virtual bool GetChicken() = 0;
    virtual void SetDamagedByPerp(bool damaged) = 0;
    virtual bool GetDamagedByPerp() = 0;
    virtual SirenState GetSirenState() const = 0;
    virtual float GetTimeSinceTargetSeen() const = 0;
    virtual void ZeroTimeSinceTargetSeen() = 0;
    virtual bool CanSeeTarget(AITarget *target) = 0;
    virtual const UCrc32 &GetSupportGoal() const = 0;
    virtual void SetSupportGoal(UCrc32 sg) = 0;
    virtual void SetWithinEngagementRadius() = 0;
    virtual bool WasWithinEngagementRadius() const = 0;
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
    virtual float GetHeat() const = 0;
    virtual void SetHeat(float heat) = 0;
    virtual Attrib::Gen::pursuitescalation *GetPursuitEscalationAttrib() = 0;
    virtual Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() = 0;
    virtual Attrib::Gen::pursuitsupport *GetPursuitSupportAttrib() = 0;
    virtual void AddCostToState(int cost) = 0;
    virtual int GetCostToState() const = 0;
    virtual void SetCostToState(int cts) = 0;
    virtual bool IsRacing() const = 0;
    virtual bool IsBeingPursued() const = 0;
    virtual bool IsHiddenFromCars() const = 0;
    virtual bool IsHiddenFromHelicopters() const = 0;
    virtual bool IsPartiallyHidden(float &HowHidden) const = 0;
    virtual void AddToPendingRepPointsFromCopDestruction(int amount) = 0;
    virtual void AddToPendingRepPointsNormal(int amount) = 0;
    virtual int GetPendingRepPointsNormal() const = 0;
    virtual int GetPendingRepPointsFromCopDestruction() const = 0;
    virtual void ClearPendingRepPoints() = 0;
    virtual void SetRacerInfo(GRacerInfo *info) = 0;
    virtual GRacerInfo *GetRacerInfo() const = 0;
    virtual float GetLastTrafficHitTime() const = 0;
    virtual void Set911CallTime(float time) = 0;
    virtual float Get911CallTime() const = 0;
};

class IAIHelicopter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IAIHelicopter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IAIHelicopter() override {}

  public:
    // Virtual methods
    virtual float GetDesiredHeightOverDest() const = 0;
    virtual void SetDesiredHeightOverDest(const float height) = 0;
    virtual void SetLookAtPosition(UMath::Vector3 la) = 0;
    virtual UMath::Vector3 GetLookAtPosition() const = 0;
    virtual void SetDestinationVelocity(const UMath::Vector3 &v) = 0;
    virtual void SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest) = 0;
    virtual bool StartPathToPoint(UMath::Vector3 &point) = 0;
    virtual bool StrafeToDestIsSet() const = 0;
    virtual void SetStrafeToDest(bool strafe) = 0;
    virtual bool FilterHeliAltitude(UMath::Vector3 &point) = 0;
    virtual void RestrictPointToRoadNet(UMath::Vector3 &seekPosition) = 0;
    virtual void SetFuelFull() = 0;
    virtual float GetFuelTimeRemaining() = 0;
    virtual void SetShadowScale(float s) = 0;
    virtual float GetShadowScale() = 0;
    virtual void SetDustStormIntensity(float d) = 0;
    virtual float GetDustStormIntensity() = 0;
};

struct RacePreparationInfo {
    enum eFlags {
        RESET_DAMAGE = 1,
    };

    RacePreparationInfo() {}

    UMath::Vector3 Position;  // offset 0x0, size 0xC
    float Speed;              // offset 0xC, size 0x4
    UMath::Vector3 Direction; // offset 0x10, size 0xC
    float HeatLevel;          // offset 0x1C, size 0x4
    unsigned int Flags;       // offset 0x20, size 0x4
};

class IRacer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRacer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~IRacer() override {}

  public:
    // Virtual methods
    // TODO fix order
    virtual void StartRace(DriverStyle style) = 0;
    virtual void QuitRace() override = 0;
    virtual void PrepareForRace(const RacePreparationInfo &rpi) = 0;
};

#endif
