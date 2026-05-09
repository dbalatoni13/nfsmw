#ifndef INTERFACES_IFENGHUD_H
#define INTERFACES_IFENGHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class IHud : public UTL::COM::IUnknown, public UTL::Collections::Listable<IHud, 2> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHud(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IHud();

    virtual void Update(class IPlayer *player, float dT);
    virtual void Release();
    virtual void HideAll();
    virtual void FadeAll(bool fadeIn);
    virtual bool AreResourcesLoaded();
    virtual bool IsHudVisible();
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetHasTurbo(bool hasTurbo);
    virtual void JoyEnable();
    virtual void JoyDisable();
    virtual void RefreshMiniMapItems();
};

enum GenericMessage_Priority {
    GenericMessage_Priority_None = 0,
    GenericMessage_Priority_5 = 1,
    GenericMessage_Priority_4 = 2,
    GenericMessage_Priority_3 = 3,
    GenericMessage_Priority_2 = 4,
    GenericMessage_Priority_1 = 5,
};

class IGenericMessage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IGenericMessage() {}
    IGenericMessage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual bool RequestGenericMessage(const char *string, bool singleFrame, uint32 fengHash, uint32 iconTextureHash, uint32 iconFengHash,
                                       GenericMessage_Priority priority);
    virtual void RequestGenericMessageZoomOut(uint32 fengHash);
    virtual bool IsGenericMessageShowing();
    virtual GenericMessage_Priority GetCurrentGenericMessagePriority();
};

class IRaceOverMessage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IRaceOverMessage() {}
    IRaceOverMessage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void RequestRaceOverMessage(class IPlayer *player);
    virtual void DismissRaceOverMessage();
    virtual bool ShouldShowRaceOverMessage();
};

class IRaceInformation : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IRaceInformation() {}
    IRaceInformation(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetNumRacers(int numRacers);
    virtual void SetNumLaps(int numLaps);
    virtual void SetPlayerPosition(int position);
    virtual void SetPlayerLapNumber(int lapNumber);
    virtual void SetPlayerLapTime(float lapTime);
    virtual void SetSuddenDeathMode(bool suddenDeath);
    virtual void SetPlayerPercentComplete(float percent);
    virtual void SetPlayerTollboothsCrossed(int num);
    virtual void SetNumTollbooths(int num);
};

class ILeaderBoard : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ILeaderBoard() {}
    ILeaderBoard(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetNumRacers(int numRacers);
    virtual void SetNumLaps(int numLaps);
    virtual void SetPlayerIndex(int index);
    virtual void SetRacerName(int index, const char *name);
    virtual void SetRacerNum(int index, int num);
    virtual void SetRacerTotalPoints(int index, float points);
    virtual void SetRacerNumLapsCompleted(int index, int laps);
    virtual void SetRacerPercentComplete(int index, float percent);
    virtual void SetRacerHasHeadset(int index, bool hasHeadset);
    virtual void SetRacerIsBusted(int index, bool isBusted);
    virtual void SetRacerIsKoed(int index, bool isKoed);
};

class ITurbometer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ITurbometer() {}
    ITurbometer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetInductionPsi(float psi);
};

enum eRaceCountdownNumber {
    RACE_COUNTDOWN_NUMBER_NONE = -1,
    RACE_COUNTDOWN_NUMBER_GO = 0,
    RACE_COUNTDOWN_NUMBER_1 = 1,
    RACE_COUNTDOWN_NUMBER_2 = 2,
    RACE_COUNTDOWN_NUMBER_3 = 3,
    RACE_COUNTDOWN_NUMBER_4 = 4,
};

class ICountdown : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ICountdown() {}
    ICountdown(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void BeginCountdown();
    virtual bool IsActive();
    virtual float GetSecondsBeforeRaceStart();
};

class ITachometerDrag : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
    ITachometerDrag(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~ITachometerDrag() {}
};

class IShiftUpdater : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IShiftUpdater() {}
    IShiftUpdater(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction);
    virtual void SetEngineBlown(bool blown);
    virtual void SetEngineTemp(float temp);
};

class ITimeExtension : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ITimeExtension() {}
    ITimeExtension(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetPlayerLapTime(float lapTime);
    virtual void RequestTimeExtensionMessage(float time);
};

class IPursuitBoard : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IPursuitBoard() {}
    IPursuitBoard(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetIsHiding(bool isHiding);
    virtual void SetTimeUntilHidden(float time);
    virtual void SetTimeUntilBusted(float time, bool bIsBusted);
    virtual void SetTimeUntilBackup(float time);
    virtual void SetIsInView(bool inView);
    virtual void SetPursuitDuration(float duration);
    virtual void SetCooldownTimeRemaining(float time);
    virtual void SetCooldownTimeRequired(float time);
    virtual void SetNumCopsInPursuit(int num);
    virtual void SetNumCopsDestroyed(int numCops, UCrc32 lastCopDestroyedType, int lastCopDestroyedMultiplier, int lastCopDestroyedRep);
    virtual void SetNumCopsDamaged(int num);
    virtual void SetTotalNumCopsInvolved(int num);
    virtual void SetHeliInvolvedInPursuit(bool involved);
    virtual void SetPursuitRep(int rep);
};

class IMilestoneBoard : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IMilestoneBoard() {}
    IMilestoneBoard(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetChallengeSeries(bool challenge);
    virtual void SetNumberOfMilestones(int num);
    virtual void SetMilestoneIconHash(int index, unsigned int hash);
    virtual void SetMilestoneType(int index, unsigned int type);
    virtual void SetMilestoneGoal(int index, float goal);
    virtual void SetMilestoneComplete(int index, bool complete);
    virtual void SetMilestoneCurrValue(int index, float value);
    virtual void SetMilestoneHeaderHash(int index, int hash);
};

class IBustedMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IBustedMeter() {}
    IBustedMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetIsHiding(bool isHiding);
    virtual void SetTimeUntilBusted(float time);
    virtual void SetIsBusted(bool busted);
};

class IMenuZoneTrigger : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IMenuZoneTrigger() {}
    IMenuZoneTrigger(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual bool ShouldSeeMenuZoneCluster();
    virtual bool IsPlayerInsideTrigger();
    virtual bool IsType(const char *type);
    virtual void EnterTrigger(class GRuntimeInstance *activity);
    virtual void EnterTrigger(const char *zoneType);
    virtual void ExitTrigger();
    virtual void RequestCingularLogo();
    // virtual void EnterTriggerForAutoSave();
    // virtual void ExitTriggerForAutoSave();
    virtual void RequestEventInfoDialog(int index);
    virtual void RequestZoneInfoDialog(int index);
    virtual void RequestDoAction();
};

class IInfractions : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IInfractions() {}
    IInfractions(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void RequestInfraction(unsigned int type, int count);
};

class IRadarDetector : public UTL::COM::IUnknown {
  public:
    enum RadarTarget {
        RADAR_TARGET_NONE = 0,
        RADAR_TARGET_COP = 1,
        RADAR_TARGET_CAMERA = 2,
    };

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IRadarDetector() {}
    IRadarDetector(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetTarget(RadarTarget targetType, float range, float direction);
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetIsCoolingDown(bool coolingDown);
};

class IAutoSaveIcon : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
    IAutoSaveIcon(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IAutoSaveIcon() {}

  public:
    virtual void RequestAutoSaveIcon();
    virtual bool IsAutoSaveIconShowing();
};

#endif
