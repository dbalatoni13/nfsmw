#ifndef INTERFACES_IFENGHUD_H
#define INTERFACES_IFENGHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

class IHud : public UTL::COM::IUnknown, public UTL::Collections::Listable<IHud, 2> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHud(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IHud() {}

    virtual void Update(class IPlayer *player, float dT) = 0;
    virtual void Release() = 0;
    virtual void HideAll() = 0;
    virtual void FadeAll(bool fade) = 0;
    virtual bool AreResourcesLoaded() = 0;
    virtual bool IsHudVisible() = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetHasTurbo(bool hasTurbo) = 0;
    virtual void JoyEnable() = 0;
    virtual void JoyDisable() = 0;
    virtual void RefreshMiniMapItems() = 0;
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
    virtual void BeginCountdown() = 0;
    virtual bool IsActive() = 0;
    virtual float GetSecondsBeforeRaceStart() = 0;
};

enum GenericMessage_Priority {
    GenericMessage_Priority_None = 0,
    GenericMessage_Priority_5 = 1,
    GenericMessage_Priority_4 = 2,
    GenericMessage_Priority_3 = 3,
    GenericMessage_Priority_2 = 4,
    GenericMessage_Priority_1 = 5,
};

class IInfractions : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IInfractions(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IInfractions() {}

  public:
    virtual void RequestInfraction(const char *string) = 0;
};

class IGenericMessage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IGenericMessage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IGenericMessage() {}

  public:
    virtual bool RequestGenericMessage(const char *string, bool singleFrame, unsigned int fengHash, unsigned int iconTextureHash,
                                       unsigned int iconFengHash, GenericMessage_Priority priority);
    virtual void RequestGenericMessageZoomOut(unsigned int fengHash) = 0;
    virtual bool IsGenericMessageShowing() = 0;
    virtual GenericMessage_Priority GetCurrentGenericMessagePriority() = 0;
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
    virtual void RequestAutoSaveIcon() = 0;
    virtual bool IsAutoSaveIconShowing() = 0;
};

class IRaceOverMessage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRaceOverMessage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IRaceOverMessage() {}

  public:
    virtual void RequestRaceOverMessage(class IPlayer *player) = 0;
    virtual void DismissRaceOverMessage() = 0;
    virtual bool ShouldShowRaceOverMessage() = 0;
};

class ITimeExtension : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITimeExtension(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~ITimeExtension() {}

  public:
    virtual void SetPlayerLapTime(float time) = 0;
    virtual void RequestTimeExtensionMessage(class IPlayer *player, float time) = 0;
};

class IGetAwayMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IGetAwayMeter() {}
    IGetAwayMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetGetAwayDistance(float distance) = 0;
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
    virtual void SetTarget(RadarTarget targetType, float range, float direction) = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetIsCoolingDown(bool coolingDown) = 0;
};

class IHeatMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHeatMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IHeatMeter() {}

  public:
    virtual void SetVehicleHeat(float heat) = 0;
    virtual void SetPursuitHeat(float heat) = 0;
};

class ICostToState : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ICostToState() {}
    ICostToState(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetCostToState(int cost) = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
};

class IReputation : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IReputation(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IReputation() {}

  public:
    virtual void SetReputationCareer(int rep) = 0;
    virtual void SetReputationPursuit(int rep) = 0;
};

class INos : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~INos() {}
    INos(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetNos(float nos) = 0;
};

class ISpeedBreakerMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ISpeedBreakerMeter() {}
    ISpeedBreakerMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetPursuitLevel(float level) = 0;
};

class IEngineTempGauge : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IEngineTempGauge() {}
    IEngineTempGauge(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetEngineTemp(float temp) = 0;
};

class ISpeedometer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ISpeedometer() {}
    ISpeedometer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetSpeed(float speed) = 0;
};

class ITachometer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITachometer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual void SetRpm(float rpm) = 0;
    virtual void SetRevLimiter(float redline, float maxRpm) = 0;
    virtual void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) = 0;
    virtual void SetShifting(bool shifting) = 0;
    virtual void SetInPerfectLaunchRange(bool inRange) = 0;

  protected:
    virtual ~ITachometer() {}
};

class IWrongWay : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~IWrongWay() {}
    IWrongWay(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual void SetWrongWay(bool isWrongWay) = 0;
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
    virtual void SetNumRacers(int numRacers) = 0;
    virtual void SetNumLaps(int numLaps) = 0;
    virtual void SetPlayerPosition(int position) = 0;
    virtual void SetPlayerLapNumber(int lapNumber) = 0;
    virtual void SetPlayerLapTime(float lapTime) = 0;
    virtual void SetSuddenDeathMode(bool suddenDeath) = 0;
    virtual void SetPlayerPercentComplete(float percent) = 0;
    virtual void SetPlayerTollboothsCrossed(int num) = 0;
    virtual void SetNumTollbooths(int num) = 0;
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
    virtual void SetNumRacers(int numRacers) = 0;
    virtual void SetNumLaps(int numLaps) = 0;
    virtual void SetPlayerIndex(int index) = 0;
    virtual void SetRacerName(int index, const char *name) = 0;
    virtual void SetRacerNum(int index, int num) = 0;
    virtual void SetRacerTotalPoints(int index, float points) = 0;
    virtual void SetRacerNumLapsCompleted(int index, int laps, float time, class IPlayer *player) = 0;
    virtual void SetRacerPercentComplete(int index, float percent, float time, class IPlayer *player) = 0;
    virtual void SetRacerHasHeadset(int index, bool hasHeadset) = 0;
    virtual void SetRacerIsBusted(int index, bool isBusted) = 0;
    virtual void SetRacerIsKoed(int index, bool isKoed) = 0;
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
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetChallengeSeries(bool challenge) = 0;
    virtual void SetNumberOfMilestones(int num) = 0;
    virtual void SetMilestoneIconHash(int index, int hash) = 0;
    virtual void SetMilestoneType(int index, unsigned int type) = 0;
    virtual void SetMilestoneGoal(int index, float goal) = 0;
    virtual void SetMilestoneComplete(int index, bool complete) = 0;
    virtual void SetMilestoneCurrValue(int index, float value) = 0;
    virtual void SetMilestoneHeaderHash(int index, int hash) = 0;
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
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetIsHiding(bool isHiding) = 0;
    virtual void SetTimeUntilHidden(float time) = 0;
    virtual void SetTimeUntilBusted(float time, bool bIsBusted) = 0;
    virtual void SetTimeUntilBackup(float time) = 0;
    virtual void SetIsInView(bool inView) = 0;
    virtual void SetPursuitDuration(float duration) = 0;
    virtual void SetCooldownTimeRemaining(float time) = 0;
    virtual void SetCooldownTimeRequired(float time) = 0;
    virtual void SetNumCopsInPursuit(int num) = 0;
    virtual void SetNumCopsDestroyed(int numCops, UCrc32 lastCopDestroyedType, int lastCopDestroyedMultiplier, int lastCopDestroyedRep) = 0;
    virtual void SetNumCopsDamaged(int num) = 0;
    virtual void SetTotalNumCopsInvolved(int num) = 0;
    virtual void SetHeliInvolvedInPursuit(bool involved) = 0;
    virtual void SetPursuitRep(int rep) = 0;
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
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetIsHiding(bool isHiding) = 0;
    virtual void SetTimeUntilBusted(float time) = 0;
    virtual void SetIsBusted(bool busted) = 0;
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
    virtual void SetInductionPsi(float psi) = 0;
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
    virtual void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction) = 0;
    virtual void SetEngineBlown(bool blown) = 0;
    virtual void SetEngineTemp(float temp) = 0;
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

#endif
