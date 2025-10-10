#ifndef INTERFACES_SIMABLES_IAI_H
#define INTERFACES_SIMABLES_IAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/World/WRoadNetwork.hpp"

// TODO move
enum eLaneSelection {
    SELECT_VALID_LANE = 2,
    SELECT_CURRENT_LANE = 1,
    SELECT_CENTER_LANE = 0,
};

struct IVehicle;

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
    virtual void SetReverseOverride(float time);
    virtual bool GetReverseOverride();
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
    virtual bool ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep);
    virtual bool ResetVehicleToRoadNav(WRoadNav *other_nav);
    virtual bool ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector);
    virtual float GetPathDistanceRemaining();
    virtual struct AITarget *GetTarget() const;
    virtual bool GetDrivableToTargetPos() const;
    virtual const UTL::Std::list<struct AIAvoidable *, _type_AIAvoidableNeighbors> &GetAvoidableList();
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
    virtual struct IPursuit *GetPursuit();
    virtual struct IRoadBlock *GetRoadBlock();
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
  protected:
    virtual ~ITrafficAI();

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITrafficAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual void StartDriving(float speed);
};

class IHumanAI : public UTL::COM::IUnknown {
  protected:
    virtual ~IHumanAI();

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHumanAI(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

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

#endif
