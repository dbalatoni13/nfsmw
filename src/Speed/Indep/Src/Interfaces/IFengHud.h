#ifndef __IFENGHUD_H
#define __IFENGHUD_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class IHud : public UTL::COM::IUnknown, public UTL::Collections::Listable<IHud, 2> {
  public:
    DECL_INTERFACE(IHud);

    virtual void Update(class IPlayer *player, float dT) = 0;
    virtual void Release() = 0;
    virtual void HideAll() = 0;
    virtual void FadeAll(bool fadeIn) = 0;
    virtual bool AreResourcesLoaded() = 0;
    virtual bool IsHudVisible() = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetHasTurbo(bool hasTurbo) = 0;
    virtual void JoyEnable() = 0;
    virtual void JoyDisable() = 0;
    virtual void RefreshMiniMapItems() = 0;
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
    DECL_INTERFACE(IGenericMessage);

    virtual bool RequestGenericMessage(const char *string, bool singleFrame, uint32 fengHash, uint32 iconTextureHash, uint32 iconFengHash,
                                       GenericMessage_Priority priority);
    virtual void RequestGenericMessageZoomOut(uint32 fengHash) = 0;
    virtual bool IsGenericMessageShowing() = 0;
    virtual GenericMessage_Priority GetCurrentGenericMessagePriority() = 0;
};

class IRaceOverMessage : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRaceOverMessage);

    virtual void RequestRaceOverMessage(class IPlayer *player) = 0;
    virtual void DismissRaceOverMessage() = 0;
    virtual bool ShouldShowRaceOverMessage() = 0;
};

class IRaceInformation : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRaceInformation);

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
    DECL_INTERFACE(ILeaderBoard);

    virtual void SetNumRacers(int numRacers) = 0;
    virtual void SetNumLaps(int numLaps) = 0;
    virtual void SetPlayerIndex(int index) = 0;
    virtual void SetRacerName(int index, const char *name) = 0;
    virtual void SetRacerNum(int index, int num) = 0;
    virtual void SetRacerTotalPoints(int index, float points) = 0;
    virtual void SetRacerNumLapsCompleted(int pos, int numLaps, float time, IPlayer *player) = 0;
    virtual void SetRacerPercentComplete(int pos, float percent, float time, struct IPlayer *player) = 0;
    virtual void SetRacerHasHeadset(int pos, bool racerHasHeadset) = 0;
    virtual void SetRacerIsBusted(int pos, bool busted) = 0;
    virtual void SetRacerIsKoed(int index, bool koed) = 0;
};

class ITurbometer : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITurbometer);

    virtual void SetInductionPsi(float psi) = 0;
};

class ICountdown : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ICountdown);

    virtual void BeginCountdown() = 0;
    virtual bool IsActive() = 0;
    virtual float GetSecondsBeforeRaceStart() = 0;
};

class ITachometerDrag : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITachometerDrag);

    virtual void SetRpm(float rpm) = 0;
    virtual void SetRevLimiter(float redline, float maxrpm) = 0;
    virtual void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) = 0;
    virtual void SetShifting(bool shifting) = 0;
    virtual void SetInPerfectLaunchRange(bool inRange) = 0;
};

class IShiftUpdater : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IShiftUpdater);

    virtual void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction) = 0;
    virtual void SetEngineBlown(bool blown) = 0;
    virtual void SetEngineTemp(float temp) = 0;
};

class ITimeExtension : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITimeExtension);

    virtual void SetPlayerLapTime(float time) = 0;
    virtual void RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) = 0;
};

class IPursuitBoard : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IPursuitBoard);

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

class IMilestoneBoard : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IMilestoneBoard);

    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetChallengeSeries(bool challenge) = 0;
    virtual void SetNumberOfMilestones(int num) = 0;
    virtual void SetMilestoneIconHash(int milestoneNum, int hash) = 0;
    virtual void SetMilestoneType(int index, unsigned int type) = 0;
    virtual void SetMilestoneGoal(int index, float goal) = 0;
    virtual void SetMilestoneComplete(int index, bool complete) = 0;
    virtual void SetMilestoneCurrValue(int index, float value) = 0;
    virtual void SetMilestoneHeaderHash(int index, int hash) = 0;
};

class IBustedMeter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IBustedMeter);

    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetIsHiding(bool isHiding) = 0;
    virtual void SetTimeUntilBusted(float time) = 0;
    virtual void SetIsBusted(bool isBusted) = 0;
};

class ISpeedometer : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISpeedometer);

    virtual void SetSpeed(float speed) = 0;
};

class IReputation : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IReputation);

    virtual void SetReputationCareer(int rep) = 0;
    virtual void SetReputationPursuit(int rep) = 0;
};

class ISpeedBreakerMeter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ISpeedBreakerMeter);

    virtual void SetPursuitLevel(float level) = 0;
};

class ICostToState : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ICostToState);

    virtual void SetCostToState(int cost) = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
};

class IWrongWay : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IWrongWay);

    virtual void SetWrongWay(bool isWrongWay) = 0;
};

class ITachometer : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITachometer);

    virtual void SetRpm(float rpm) = 0;
    virtual void SetRevLimiter(float redline, float maxrpm) = 0;
    virtual void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) = 0;
    virtual void SetShifting(bool shifting) = 0;
    virtual void SetInPerfectLaunchRange(bool inRange) = 0;
};

class IGetAwayMeter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IGetAwayMeter);

    virtual void SetGetAwayDistance(float distance) = 0;
};

class IEngineTempGauge : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEngineTempGauge);

    virtual void SetEngineTemp(float temp) = 0;
};

class IMenuZoneTrigger : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IMenuZoneTrigger);

    virtual bool ShouldSeeMenuZoneCluster() = 0;
    virtual bool IsPlayerInsideTrigger() = 0;
    virtual bool IsType(const char *type) = 0;
    virtual void EnterTrigger(class GRuntimeInstance *activity) = 0;
    virtual void EnterTrigger(const char *zoneType) = 0;
    virtual void ExitTrigger() = 0;
    virtual void RequestCingularLogo() = 0;
    // virtual void EnterTriggerForAutoSave() = 0;
    // virtual void ExitTriggerForAutoSave() = 0;
    virtual void RequestEventInfoDialog(int index) = 0;
    virtual void RequestZoneInfoDialog(int index) = 0;
    virtual void RequestDoAction() = 0;
};

class IInfractions : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IInfractions);

    virtual void RequestInfraction(const char *infractionString) = 0;
};

class IRadarDetector : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRadarDetector);

    enum RadarTarget {
        RADAR_TARGET_NONE = 0,
        RADAR_TARGET_COP = 1,
        RADAR_TARGET_CAMERA = 2,
    };

    virtual void SetTarget(RadarTarget targetType, float range, float direction) = 0;
    virtual void SetInPursuit(bool inPursuit) = 0;
    virtual void SetIsCoolingDown(bool coolingDown) = 0;
};

class IAutoSaveIcon : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAutoSaveIcon);

    virtual void RequestAutoSaveIcon() = 0;
    virtual bool IsAutoSaveIconShowing() = 0;
};

class IHeatMeter : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IHeatMeter);

    virtual void SetVehicleHeat(float heat) = 0;
    virtual void SetPursuitHeat(float heat) = 0;
};

class INos : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(INos);

    virtual void SetNos(float nos) = 0;
};

#endif
