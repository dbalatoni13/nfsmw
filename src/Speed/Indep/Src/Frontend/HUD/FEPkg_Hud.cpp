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

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowSMS.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOn.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"

struct FadeScreen : MenuScreen {
    static bool IsFadeScreenOn();
    FadeScreen(ScreenConstructorData *);
    ~FadeScreen() override;
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
};

extern bool bIsRestartingRace;

extern FEString *FEngFindString(const char *, int);

extern const char *HudSingleRaceTexturePackFilename;
extern const char *HudDragTexturePackFilename;
extern const char *HudSplitScreenTexturePackFilename;
extern const char *HudDragSplitScreenTexturePackFilename;

const char *HudResourceManager::GetHudTexPackFilename(ePlayerHudType ht) {
    if (ht == PHT_DRAG) {
        return HudDragTexturePackFilename;
    }
    if (static_cast<unsigned int>(ht - 3) < 2) {
        return HudSplitScreenTexturePackFilename;
    }
    if (static_cast<unsigned int>(ht - 5) >= 2) {
        return HudSingleRaceTexturePackFilename;
    }
    return HudDragSplitScreenTexturePackFilename;
}

const char *HudResourceManager::GetHudFengName(ePlayerHudType ht) {
    switch (ht) {
    case PHT_DRAG:
        return "HUD_Drag.fng";
    case PHT_SPLIT1:
        return "HUD_Player1.fng";
    case PHT_SPLIT2:
        return "HUD_Player2.fng";
    case PHT_DRAG_SPLIT1:
        return "HUD_Drag_Player1.fng";
    case PHT_DRAG_SPLIT2:
        return "HUD_Drag_Player2.fng";
    default:
        return "HUD_SingleRace.fng";
    }
}

bool HudResourceManager::AreResourcesLoaded(ePlayerHudType ht) {
    if (mHudResourcesState == HRM_LOADED) {
        if (ht == PHT_SPLIT2) {
            return LoadingResourcesForHudType == PHT_SPLIT1;
        }
        if (ht == PHT_DRAG_SPLIT2) {
            return LoadingResourcesForHudType == PHT_DRAG_SPLIT1;
        }
        if (LoadingResourcesForHudType == ht) {
            return true;
        }
    }
    return false;
}

bool FEngHud::ShouldRearViewMirrorBeVisible(EVIEW_ID viewId) {
    eView *view = eGetView(viewId, false);
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player) {
        IHud *hud = player->GetHud();
        if (hud && !player->GetHud()->IsHudVisible()) {
            return false;
        }
    }

    CameraMover *camMover = nullptr;
    if (view) {
        camMover = view->GetCameraMover();
    }

    if (camMover && camMover->GetType() == CM_DRIVE_CUBIC) {
        if (camMover->GetLookbackAngle()) {
            return false;
        }
    }

    if (FEManager::ShouldPauseSimulation(true)) {
        return false;
    }

    if (!FEDatabase) {
        return false;
    }

    if (!FEDatabase->GetGameplaySettings()->RearviewOn) {
        return false;
    }

    ePlayerSettingsCameras playerCam = FEDatabase->GetPlayerSettings(viewId - 1)->CurCam;
    if (playerCam >= PSC_CLOSE && playerCam <= PSC_PURSUIT) {
        return false;
    }

    return cFEng::Get()->IsPackagePushed("HUD_SingleRace.fng");
}

float FEngHud::ChooseMaxRpmTextureNumber(float rpm) {
    if (rpm < 7000.0f) {
        return 7000.0f;
    }
    if (rpm < 8000.0f) {
        return 8000.0f;
    }
    if (rpm < 9000.0f) {
        return 9000.0f;
    }
    return 10000.0f;
}

FEngHud::FEngHud(ePlayerHudType ht, const char *pkg_name, IPlayer *player, int player_number)
    : UTL::COM::Object(0x14) //
    , IHud(this) //
    , mPlayerHudType(ht) //
    , PlayerNumber(player_number) //
    , mActionQ(true) //
    , mCurrentWidescreenSetting(false)
{
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
    pPackageName = pkg_name;

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.LoadRequiredResources(mPlayerHudType, pkg_name);
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
            if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == 2) {
                pReputation = new Reputation(this, pkg_name, player_number);
            }
            pHeatMeter = new HeatMeter(this, pkg_name, player_number);
            pCostToState = new CostToState(this, pkg_name, player_number);
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

void FEngHud::JoyHandle(IPlayer *player) {
    if (!player || !player->GetSettings()) {
        mActionQ.SetPort(-1);
        mActionQ.SetConfig(0, "FEngHud");
        return;
    }

    {
        bool wheel_connected = false;
        if (player->GetSteeringDevice()) {
            if (player->GetSteeringDevice()->IsConnected()) {
                wheel_connected = true;
            }
        }

        mActionQ.SetPort(player->GetControllerPort());
        mActionQ.SetConfig(player->GetSettings()->GetControllerAttribs(CA_HUD, wheel_connected), "FEngHud");

        if (mActionQ.IsEmpty()) goto drain;
        if (MemoryCard::GetInstance()->IsAutoSaving()) goto drain;
        if (MemoryCard::GetInstance()->AutoSaveRequested()) goto drain;

        {
            ActionRef aRef = mActionQ.GetAction();

            if (!CurrentHudFeatures) goto drain;

            switch (aRef.ID()) {
                case HUDACTION_PAUSEREQUEST:
                    new EPause(player->GetSettingsIndex(), 0, 0);
                    break;

                case HUDACTION_ENGAGE_EVENT:
                    if (!FEDatabase->IsLANMode() && !FEDatabase->IsOnlineMode()) {
                        ISimable *isimable = player->GetSimable();
                        IVehicleAI *vehicleai;
                        IMenuZoneTrigger *izone;
                        ePursuitStatus pursuitStatus;
                        IPursuit *ipursuit;

                        if (isimable->QueryInterface(&vehicleai)) {
                            ipursuit = vehicleai->GetPursuit();
                            if (ipursuit) {
                                pursuitStatus = ipursuit->GetPursuitStatus();
                                if (pursuitStatus == PS_COOL_DOWN) {
                                    if (QueryInterface(&izone)) {
                                        if (izone->IsPlayerInsideTrigger()) {
                                            if (izone->IsType("safehouse")) {
                                                ipursuit->EndPursuitEnteringSafehouse();
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
                            !GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
                            new EShowResults(FERESULTTYPE_RACE, true);
                        } else if (mInPursuit) {
                            new EShowResults(FERESULTTYPE_PURSUIT, true);
                        } else {
                            if (QueryInterface(&izone)) {
                                if (izone->IsPlayerInsideTrigger()) {
                                    izone->ExitTrigger(mActionQ.GetPort());
                                    izone->RequestEventInfoDialog(mActionQ.GetPort());
                                }
                            }
                        }
                    }
                    break;

                case HUDACTION_PAD_LEFT:
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        new EWorldMapOn();
                    }
                    break;

                case HUDACTION_PAD_DOWN:
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        if (!FEDatabase->IsDDay()) {
                            new ERaceSheetOn(0);
                        }
                    }
                    break;

                case HUDACTION_PAD_RIGHT:
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        new EShowSMS(-1);
                    }
                    break;
            }
        }

    drain:
        while (!mActionQ.IsEmpty()) {
            mActionQ.PopAction();
        }
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

unsigned long long FEngHud::DetermineHudFeatures(IPlayer *player) {
    unsigned long long hud_features = 0;

    eView *view = eGetView(player->GetRenderPort(), false);
    CameraMover *cammover = nullptr;
    if (view) {
        cammover = view->GetCameraMover();
    }

    if (!cammover) {
        return 0;
    }
    if (cammover->GetType() != CM_DRIVE_CUBIC) {
        return 0;
    }
    if (cammover->GetLookbackAngle()) {
        return 0;
    }

    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && bIsRestartingRace) {
            return 0;
        }
    }

    if (FEManager::IsPaused()) {
        return 0;
    }
    if (Sim::GetState() != Sim::STATE_ACTIVE) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("Pause_Main.fng")) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("MC_Main_GC.fng")) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("PostRace_Results.fng")) {
        return 0;
    }
    if (!TheHudResourceManager.AreResourcesLoaded(mPlayerHudType)) {
        return 0;
    }
    if (UTL::Collections::Singleton< INIS >::Get()) {
        return 0;
    }
    if (mCurrentWidescreenSetting != FEDatabase->GetVideoSettings()->WideScreen) {
        return 0;
    }
    if (TheICEManager.IsEditorOn()) {
        return 0;
    }
    if (TheGameFlowManager.IsLoading()) {
        return 0;
    }

    bool EnableMinimap = (LoadedTrackInfo->TrackNumber == 2000);
    if (!GRaceStatus::Get().GetRaceParameters()) {
        if (FEDatabase->GetGameplaySettings()->ExploringMiniMapMode == 2) {
            EnableMinimap = false;
        }
    } else {
        if (FEDatabase->GetGameplaySettings()->RacingMiniMapMode == 2) {
            EnableMinimap = false;
        }
    }

    if (EnableMinimap) {
        hud_features |= 0x10000;
        hud_features |= 0x4000;
    }

    if (GRaceStatus::IsDragRace()) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= 0x8000000;
        }
    } else {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
            hud_features |= 0x8000000;
        }
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
        hud_features |= 0x2;

        if (mHasTurbo) {
            hud_features |= 0x20000;
        }

        if (GRaceStatus::IsDragRace()) {
            hud_features |= 0x40;
        }

        hud_features |= 0x800;
        hud_features |= 0x40000;
    }

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters()) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && !pursuitRace) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->LeaderboardOn) {
            hud_features |= 0x8;
        }
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= 0x4000000;
        }
    } else {
        hud_features |= 0x100000;
        hud_features |= 0x400000000ULL;
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->SplitTimeType != 4) {
        hud_features |= 0x10;
    }

    if (GRaceStatus::IsTollboothRace()) {
        hud_features |= 0x2000000;
    }

    if (player->GetSettings()->ScoreOn) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career
            || (GRaceStatus::Get().GetRaceParameters()
                && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
            hud_features |= 0x1000;
        }
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        hud_features |= 0x20;
    }

    IRaceOverMessage *iraceover;
    if (QueryInterface(&iraceover)) {
        if (iraceover->ShouldShowRaceOverMessage()) {
            hud_features |= 0x4;
        }
    }

    hud_features |= 0x1000000;
    hud_features |= 0x10000000;
    hud_features |= 0x100000000ULL;
    hud_features |= 0x80000;
    hud_features |= 0x400;
    hud_features |= 0x20000000;
    hud_features |= 0x400000;
    hud_features |= 0x200000000ULL;
    hud_features |= 0x800000;
    hud_features |= 0x200000;

    return hud_features;
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

void HideEverySingleHud() {
    const UTL::Collections::Listable<IHud, 2>::List &list = UTL::Collections::Listable<IHud, 2>::GetList();
    IHud *const *end = list.end();
    for (IHud *const *iter = list.begin(); iter != end; iter++) {
        (*iter)->HideAll();
    }
}
