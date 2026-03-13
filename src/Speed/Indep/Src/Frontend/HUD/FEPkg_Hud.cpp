#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeAutoSaveIcon.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeBustedMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCostToState.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeDragTachometer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGetawayMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeOnlineHudSupport.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeReputation.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeShiftUpdater.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeSpeedBreakerMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeSpeedometer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTurboMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

extern FEString *FEngFindString(const char *, int);

FEngHud::FEngHud(ePlayerHudType ht, const char *pkg_name, IPlayer *player, int player_number)
    : UTL::COM::Object(0x14) //
    , IHud(this) //
    , mActionQ(true)
{
    mPlayerHudType = ht;
    PlayerNumber = player_number;
    pPlayer = player;
    mInPursuit = false;
    mHasTurbo = false;
    pSpeedometer = nullptr;
    pTachometer = nullptr;
    pTachometerDrag = nullptr;
    pShiftUpdater = nullptr;
    pCostToState = nullptr;
    pReputation = nullptr;
    pHeatMeter = nullptr;
    pTurboMeter = nullptr;
    pEngineTemp = nullptr;
    pNitrous = nullptr;
    pSpeedBreakerMeter = nullptr;
    pRaceOverMessage = nullptr;
    pGenericMessage = nullptr;
    pAutoSaveIcon = nullptr;
    pRaceInformation = nullptr;
    pLeaderBoard = nullptr;
    pPursuitBoard = nullptr;
    pMilestoneBoard = nullptr;
    pBustedMeter = nullptr;
    pTimeExtension = nullptr;
    pWrongWIndi = nullptr;
    pOnlineSupport = nullptr;
    p321Go = nullptr;
    pRadarDetector = nullptr;
    pMinimap = nullptr;
    pGetAwayMeter = nullptr;
    pMenuZoneTrigger = nullptr;
    pInfractions = nullptr;
    mCurrentWidescreenSetting = false;
    CurrentHudFeatures = 0;
    pPackageName = pkg_name;

    if (ht != PHT_SPLIT2 && ht != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.LoadRequiredResources(ht, pkg_name);
    }

    cFEng::mInstance->PushNoControlPackage(pkg_name, static_cast< FE_PACKAGE_PRIORITY >(0x66));
    FEngSetAllObjectsInPackageVisibility(pkg_name, false);

    pSpeedometer = new Speedometer(this, pPackageName, player_number);
    pRaceInformation = new RaceInformation(this, pkg_name, player_number);
    pLeaderBoard = new LeaderBoard(this, pkg_name, player_number);
    pNitrous = new NitrousGauge(this, pkg_name, player_number);
    pRaceOverMessage = new RaceOverMessage(this, pkg_name, player_number);
    pGenericMessage = new GenericMessage(this, pkg_name, player_number);
    pTurboMeter = new TurboMeter(this, pkg_name, player_number);
    pWrongWIndi = new WrongWIndi(this, pkg_name, player_number);
    p321Go = new Countdown(this, pkg_name, player_number);

    if (mPlayerHudType == PHT_DRAG || mPlayerHudType == PHT_DRAG_SPLIT1 || mPlayerHudType == PHT_DRAG_SPLIT2) {
        pEngineTemp = new EngineTempGauge(this, pkg_name, player_number);
        pTachometerDrag = new DragTachometer(this, pPackageName, player_number);
        pShiftUpdater = new ShiftUpdater(this, pPackageName, player_number);
    } else {
        pTimeExtension = new TimeExtension(this, pkg_name, player_number);
        pTachometer = new Tachometer(this, pPackageName, player_number);

        if (mPlayerHudType == PHT_STANDARD) {
            pHeatMeter = new HeatMeter(this, pkg_name, player_number);
            pCostToState = new CostToState(this, pkg_name, player_number);
            pReputation = new Reputation(this, pkg_name, player_number);
            pPursuitBoard = new PursuitBoard(this, pkg_name, player_number);
            pMilestoneBoard = new MilestoneBoard(this, pkg_name, player_number);
            pBustedMeter = new BustedMeter(this, pkg_name, player_number);
            pMenuZoneTrigger = new MenuZoneTrigger(this, pkg_name, player_number);
            pInfractions = new Infractions(this, pkg_name, player_number);
            pRadarDetector = new RadarDetector(this, pkg_name, player_number);
        }

        if (mPlayerHudType == PHT_STANDARD || mPlayerHudType == PHT_SPLIT1) {
            pMinimap = new Minimap(pkg_name, player_number);
        }
    }

    if (mPlayerHudType == PHT_STANDARD || mPlayerHudType == PHT_DRAG) {
        pSpeedBreakerMeter = new SpeedBreakerMeter(this, pkg_name, player_number);
        pGetAwayMeter = new GetAwayMeter(this, pkg_name, player_number);
        pAutoSaveIcon = new AutoSaveIcon(this, pkg_name, player_number);
    }

    if (TheOnlineManager.IsOnlineRace()) {
        pOnlineSupport = new OnlineHUDSupport(pkg_name);
    } else {
        FEngSetInvisible(static_cast< FEObject * >(FEngFindString(pkg_name, static_cast< int >(0xC18C12FD))));
        FEngSetInvisible(static_cast< FEObject * >(FEngFindString(pkg_name, static_cast< int >(0xC18C12FE))));
    }

    CurrentHudFeatures = 0;
    SetHudFeatures(0xFFFFFFFF);
    SetHudFeatures(0);
    JoyEnable();
}
