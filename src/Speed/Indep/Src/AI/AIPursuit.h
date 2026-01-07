#ifndef AI_AIPURSUIT_H
#define AI_AIPURSUIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

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
    ~IPursuit();

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
    virtual struct IRoadBlock *GetRoadBlock() const;
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
    virtual void AddRoadBlock(struct IRoadBlock *roadblock);
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
    virtual Attrib::Gen::pursuitlevels *GetPursuitLevelAttrib() const;
    // TODO figure out where this is
    virtual void SetBustedTimerToZero();
};

#endif
