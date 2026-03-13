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
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

struct FadeScreen {
    static bool IsFadeScreenOn();
};

extern bool bIsRestartingRace;

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

FEngHud::~FEngHud() {
    delete pSpeedometer;
    pSpeedometer = nullptr;
    delete pTachometer;
    pTachometer = nullptr;
    delete pTachometerDrag;
    pTachometerDrag = nullptr;
    delete pShiftUpdater;
    pShiftUpdater = nullptr;
    delete pTurboMeter;
    pTurboMeter = nullptr;
    delete pEngineTemp;
    pEngineTemp = nullptr;
    delete pNitrous;
    pNitrous = nullptr;
    delete pSpeedBreakerMeter;
    pSpeedBreakerMeter = nullptr;
    delete pRaceOverMessage;
    pRaceOverMessage = nullptr;
    delete pGenericMessage;
    pGenericMessage = nullptr;
    delete pRaceInformation;
    pRaceInformation = nullptr;
    delete pLeaderBoard;
    pLeaderBoard = nullptr;
    delete pPursuitBoard;
    pPursuitBoard = nullptr;
    delete pMilestoneBoard;
    pMilestoneBoard = nullptr;
    delete pBustedMeter;
    pBustedMeter = nullptr;
    delete pTimeExtension;
    pTimeExtension = nullptr;
    delete pCostToState;
    pCostToState = nullptr;
    delete pReputation;
    pReputation = nullptr;
    delete pHeatMeter;
    pHeatMeter = nullptr;
    delete pWrongWIndi;
    pWrongWIndi = nullptr;
    delete pOnlineSupport;
    pOnlineSupport = nullptr;
    delete p321Go;
    p321Go = nullptr;
    delete pRadarDetector;
    pRadarDetector = nullptr;
    delete pMinimap;
    pMinimap = nullptr;
    delete pGetAwayMeter;
    pGetAwayMeter = nullptr;
    delete pMenuZoneTrigger;
    pMenuZoneTrigger = nullptr;
    delete pInfractions;
    pInfractions = nullptr;

    cFEng::mInstance->PopNoControlPackage(pPackageName);

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.UnloadRequiredResources(mPlayerHudType);
    }
}

void FEngHud::Update(IPlayer *player, float dT) {
    ProfileNode profile_node("FEngHud::Update", 0);

    unsigned long long hudFeatures = DetermineHudFeatures(player);
    if (hudFeatures != CurrentHudFeatures) {
        SetHudFeatures(hudFeatures);
    }

    if (mActionQ.IsEnabled()) {
        bool loading = TheGameFlowManager.IsLoading();
        if (!loading && !bIsRestartingRace && !UTL::Collections::Singleton< INIS >::Get()
            && FadeScreen::IsFadeScreenOn()) {
            new EFadeScreenOff(0x14035fb);
        }
    }

    SetWideScreenMode();

    if (hudFeatures != 0) {
        if (pSpeedometer && pSpeedometer->IsElementVisible()) {
            pSpeedometer->Update(player);
        }
        if (pTachometer && pTachometer->IsElementVisible()) {
            pTachometer->Update(player);
        }
        if (pTachometerDrag && pTachometerDrag->IsElementVisible()) {
            pTachometerDrag->Update(player);
        }
        if (pShiftUpdater && pShiftUpdater->IsElementVisible()) {
            pShiftUpdater->Update(player);
        }
        if (pMinimap && pMinimap->IsElementVisible()) {
            pMinimap->Update(player);
        }
        if (pRaceInformation && pRaceInformation->IsElementVisible()) {
            pRaceInformation->Update(player);
        }
        if (pLeaderBoard && pLeaderBoard->IsElementVisible()) {
            pLeaderBoard->Update(player);
        }
        if (pPursuitBoard && pPursuitBoard->IsElementVisible()) {
            pPursuitBoard->Update(player);
        }
        if (pMilestoneBoard && pMilestoneBoard->IsElementVisible()) {
            pMilestoneBoard->Update(player);
        }
        if (pBustedMeter && pBustedMeter->IsElementVisible()) {
            pBustedMeter->Update(player);
        }
        if (pTimeExtension && pTimeExtension->IsElementVisible()) {
            pTimeExtension->Update(player);
        }
        if (pCostToState && pCostToState->IsElementVisible()) {
            pCostToState->Update(player);
        }
        if (pReputation && pReputation->IsElementVisible()) {
            pReputation->Update(player);
        }
        if (pHeatMeter && pHeatMeter->IsElementVisible()) {
            pHeatMeter->Update(player);
        }
        if (pNitrous && pNitrous->IsElementVisible()) {
            pNitrous->Update(player);
        }
        if (pSpeedBreakerMeter && pSpeedBreakerMeter->IsElementVisible()) {
            pSpeedBreakerMeter->Update(player);
        }
        if (pGetAwayMeter && pGetAwayMeter->IsElementVisible()) {
            pGetAwayMeter->Update(player);
        }
        if (pRaceOverMessage && pRaceOverMessage->IsElementVisible()) {
            pRaceOverMessage->Update(player);
        }
        if (pGenericMessage && pGenericMessage->IsElementVisible()) {
            pGenericMessage->Update(player);
        }
        if (pTurboMeter && pTurboMeter->IsElementVisible()) {
            pTurboMeter->Update(player);
        }
        if (pEngineTemp && pEngineTemp->IsElementVisible()) {
            pEngineTemp->Update(player);
        }
        if (p321Go && p321Go->IsElementVisible()) {
            p321Go->Update(player);
        }
        if (pRadarDetector && pRadarDetector->IsElementVisible()) {
            pRadarDetector->Update(player);
        }
        if (pMenuZoneTrigger && pMenuZoneTrigger->IsElementVisible()) {
            pMenuZoneTrigger->Update(player);
        }
        if (pWrongWIndi && pWrongWIndi->IsElementVisible()) {
            pWrongWIndi->Update(player);
        }
        if (pOnlineSupport) {
            pOnlineSupport->Update(player);
        }
        if (pInfractions && pInfractions->IsElementVisible()) {
            pInfractions->Update(player);
        }

        if (MemoryCard::s_pThis->m_bAutoSaveRequested) {
            MemoryCard::s_pThis->m_bHUDLoaded = true;
        }
    }

    JoyHandle(player);
}

void FEngHud::SetInPursuit(bool inPursuit) {
    if (mInPursuit != inPursuit) {
        mInPursuit = inPursuit;
    }
}

void FEngHud::SetHasTurbo(bool hasTurbo) {
    mHasTurbo = hasTurbo;
}

bool FEngHud::IsHudVisible() {
    return CurrentHudFeatures != 0;
}

void FEngHud::HideAll() {
    SetHudFeatures(0);
}

void FEngHud::Release() {
    delete this;
}

bool FEngHud::AreResourcesLoaded() {
    return TheHudResourceManager.AreResourcesLoaded(mPlayerHudType);
}

void FEngHud::RefreshMiniMapItems() {
    if (pMinimap) {
        static_cast< Minimap * >(pMinimap)->RefreshMapItems();
    }
}

OnlineHUDSupport *FEngHud::GetOnlineHUDSupport() {
    return static_cast< OnlineHUDSupport * >(pOnlineSupport);
}

extern const char lbl_803E4E0C[];
extern const char lbl_803E572C[];

void FEngHud::FadeAll(bool fadeIn) {
    if (fadeIn) {
        cFEng::Get()->QueuePackageMessage(0xBCC00F05, pPackageName, nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper(lbl_803E572C), pPackageName, nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(0x54C20A66, pPackageName, nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper(lbl_803E4E0C), pPackageName, nullptr);
    }
}

void FEngHud::SetHudFeatures(unsigned long long hud_features) {
    unsigned long long diff = CurrentHudFeatures ^ hud_features;
    if (pSpeedometer != nullptr && (diff & 0x8000000)) {
        pSpeedometer->Toggle(hud_features);
    }
    if (pTachometer != nullptr && (diff & 0x2)) {
        pTachometer->Toggle(hud_features);
    }
    if (pTachometerDrag != nullptr && (diff & 0x2)) {
        pTachometerDrag->Toggle(hud_features);
    }
    if (pShiftUpdater != nullptr && (diff & 0x20000000)) {
        pShiftUpdater->Toggle(hud_features);
    }
    if (pTurboMeter != nullptr && (diff & 0x20000)) {
        pTurboMeter->Toggle(hud_features);
    }
    if (pEngineTemp != nullptr && (diff & 0x40)) {
        pEngineTemp->Toggle(hud_features);
    }
    if (pNitrous != nullptr && (diff & 0x800)) {
        pNitrous->Toggle(hud_features);
    }
    if (pSpeedBreakerMeter != nullptr && (diff & 0x40000)) {
        pSpeedBreakerMeter->Toggle(hud_features);
    }
    if (pHeatMeter != nullptr && (diff & 0x4000)) {
        pHeatMeter->Toggle(hud_features);
    }
    if (pMinimap != nullptr && (diff & 0x10000)) {
        pMinimap->Toggle(hud_features);
    }
    if (pGetAwayMeter != nullptr && (diff & 0x200)) {
        pGetAwayMeter->Toggle(hud_features);
    }
    if (pMenuZoneTrigger != nullptr && (diff & 0x400000)) {
        pMenuZoneTrigger->Toggle(hud_features);
    }
    if (pRaceInformation != nullptr && (diff & 0x4000000)) {
        pRaceInformation->Toggle(hud_features);
    }
    if (pLeaderBoard != nullptr) {
        if ((diff & 0x8) || (diff & 0x10)) {
            pLeaderBoard->Toggle(hud_features);
        }
    }
    if (pPursuitBoard != nullptr && (diff & 0x100000)) {
        pPursuitBoard->Toggle(hud_features);
    }
    if (pMilestoneBoard != nullptr && (diff & 0x400000000ULL)) {
        pMilestoneBoard->Toggle(hud_features);
    }
    if (pBustedMeter != nullptr && (diff & 0x800000)) {
        pBustedMeter->Toggle(hud_features);
    }
    if (pTimeExtension != nullptr && (diff & 0x2000000)) {
        pTimeExtension->Toggle(hud_features);
    }
    if (pCostToState != nullptr && (diff & 0x1000)) {
        pCostToState->Toggle(hud_features);
    }
    if (pReputation != nullptr && (diff & 0x1000)) {
        pReputation->Toggle(hud_features);
    }
    if (pWrongWIndi != nullptr && (diff & 0x20)) {
        pWrongWIndi->Toggle(hud_features);
    }
    if (pRaceOverMessage != nullptr && (diff & 0x4)) {
        pRaceOverMessage->Toggle(hud_features);
    }
    if (pGenericMessage != nullptr && (diff & 0x1000000)) {
        pGenericMessage->Toggle(hud_features);
    }
    if (pRadarDetector != nullptr && (diff & 0x200000)) {
        pRadarDetector->Toggle(hud_features);
    }
    if (p321Go != nullptr && (diff & 0x400)) {
        p321Go->Toggle(hud_features);
    }
    if (pInfractions != nullptr && (diff & 0x200000000ULL)) {
        pInfractions->Toggle(hud_features);
    }
    CurrentHudFeatures = hud_features;
}

void FEngHud::JoyEnable() {
    int port = pPlayer->GetControllerPort();
    if (!mActionQ.IsEnabled()) {
        mActionQ.SetPort(port);
        mActionQ.Enable(true);
        mActionQ.Flush();
    }
}

void FEngHud::JoyDisable() {
    pPlayer->GetControllerPort();
    if (mActionQ.IsEnabled()) {
        mActionQ.Enable(false);
        mActionQ.Flush();
    }
}

void FEngHud::SetWideScreenMode() {
    int widescreen = FEDatabase->GetVideoSettings()->WideScreen;
    if (mCurrentWidescreenSetting != widescreen) {
        mCurrentWidescreenSetting = widescreen;
        if (widescreen != 0) {
            cFEng::Get()->QueuePackageMessage(0x62ED04EC, pPackageName, nullptr);
            if (pMinimap) {
                static_cast< Minimap * >(pMinimap)->AdjustForWidescreen(true);
            }
        } else {
            cFEng::Get()->QueuePackageMessage(0x53EC068C, pPackageName, nullptr);
            if (pMinimap) {
                static_cast< Minimap * >(pMinimap)->AdjustForWidescreen(false);
            }
        }
    }
}
