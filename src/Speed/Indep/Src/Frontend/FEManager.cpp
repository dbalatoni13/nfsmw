#include "FEManager.hpp"

#include "Speed/GameCube/Src/Logitech/LGWheels.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEGameInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"

extern bool DrawFEng;
extern int SummonChyronNow;
extern int DoScreenPrintf;

FEManager *FEManager::mInstance = nullptr;
int FEManager::mPauseRequest = 0;            // size: 0x4, address: 0x8041B974
const char *FEManager::mPauseReason[8] = {}; // size: 0x20, address: 0x8041B978

FEManager::FEManager()
    : bSuppressControllerError(false), bAllowControllerError(false), mFirstScreen(nullptr), mFirstScreenArg(0),
#ifndef EA_BUILD_A124
      mFirstScreenMask(0xFF),
#endif
      mGarageType(GARAGETYPE_NONE), mPreviousGarageType(GARAGETYPE_NONE), mGarageBackground(nullptr), mFirstBoot(true), mEATraxDelay(0),
      mEATraxFirstButton(false) {
    for (int port = 0; port < 8; port++) {
        bWantControllerError[port] = false;
    }
}

void FEManager::Init() {
    if (mInstance == nullptr) {
        mInstance = new ("FEManager", 0) FEManager;
    }
    InitFEngMemoryPool();
    LoadingScreen::InitLoadingScreen();
    LoadingTips::InitLoadingTipsScreen();
    LoadingControllerScreen::InitLoadingControllerScreen();
    InitChyron();
    cFEngGameInterface::pInstance = new ("cFEngGameInterface", 0) cFEngGameInterface;
    ChoppedMiniMapManager::Init();
    cFEng::Init();
    cFEngRender::mInstance = new ("cFEngRender", 0) cFEngRender;
    FEManager::Get()->SetGarageType(GARAGETYPE_MAIN_FE);
    uiRepSheetRivalFlow::Init();
}

void FEManager::InitInput() {
    cFEngJoyInput::mInstance = new ("cFEngJoyInput", 0) cFEngJoyInput();
}

void FEManager::Destroy() {
    // TODO: probably other stuff
    delete mInstance;
}

FEManager *FEManager::Get() {
    return mInstance;
}

eGarageType FEManager::GetGarageType() {
    return mGarageType;
}

void FEManager::SetGarageType(eGarageType pGarageType) {
    mPreviousGarageType = GetGarageType();
    mGarageType = pGarageType;
}

const char *FEManager::GetGarageNameFromType() {
    eGarageType garageTypeToUse = mGarageType;
    switch (garageTypeToUse) {
        case GARAGETYPE_NONE:
            return "";
        case GARAGETYPE_MAIN_FE:
            return "FRONTEND\\PLATFORMS\\PLATFORMCRIB.BIN";
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return "FRONTEND\\PLATFORMS\\CAREER_SAFEHOUSE.BIN";
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return "FRONTEND\\PLATFORMS\\CUSTOMIZATION_SHOP.BIN";
        case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
            return "FRONTEND\\PLATFORMS\\CUSTOMIZATION_SHOP_BACKROOM.BIN";
        case GARAGETYPE_CAR_LOT:
            return "FRONTEND\\PLATFORMS\\CAR_LOT.BIN";
        default:
            return "";
    }
}

const char *FEManager::GetGaragePrefixFromType(eGarageType pGarageType) {
    switch (pGarageType) {
        case GARAGETYPE_NONE:
            return "";
        case GARAGETYPE_MAIN_FE:
            return "QRACE";
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return "CAREER";
        case GARAGETYPE_CUSTOMIZATION_SHOP:
        case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
            return "CSHOP";
        case GARAGETYPE_CAR_LOT:
            return "CARLOT";
        default:
            return "";
    }
}

// UNSOLVED
bool FEManager::IsOkayToRequestPauseSimulation(int playerIndex, bool useControllerErrors, bool okIfAutoSaveActive) {
    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return false;
    }

    if (cFEng::Get()->IsPackagePushed("InGamePhotoMaster.fng")) {
        return false;
    }

    if (MemoryCard::GetInstance()->IsAutoSaving() && !okIfAutoSaveActive) {
        return false;
    }

    if (FadeScreen::IsFadeScreenOn()) {
        return false;
    }

    if (cFEng::Get()->IsPackagePushed("FadeScreenNoLoadingBar.fng")) {
        return false;
    }

    if (GRaceStatus::Exists()) {
        ISimable *simable = IPlayer::GetList(PLAYER_LOCAL)[playerIndex]->GetSimable();
        GRacerInfo *racerInfo;
        if (simable != nullptr) {
            racerInfo = GRaceStatus::Get().GetRacerInfo(simable);
        } else {
            racerInfo = nullptr;
        }

        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
            if (!GRaceStatus::Get().GetIsTimeLimited() || GRaceStatus::Get().GetRaceTimeRemaining() > 0.0f) {
                if ((racerInfo == nullptr) || (!racerInfo->GetIsEngineBlown() && !racerInfo->GetIsTotalled() && !racerInfo->GetIsKnockedOut() &&
                                               !racerInfo->IsFinishedRacing())) {
                    return !ShouldPauseSimulation(useControllerErrors);
                }

                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    int other_player = static_cast<int>(playerIndex != 1);
                    ISimable *other_simable = IPlayer::GetList(PLAYER_LOCAL)[other_player]->GetSimable();
                    GRacerInfo *other_racerInfo;
                    if (other_simable != nullptr) {
                        other_racerInfo = GRaceStatus::Get().GetRacerInfo(other_simable);
                    } else {
                        other_racerInfo = nullptr;
                    }
                    if ((other_racerInfo == nullptr) || (!other_racerInfo->GetIsEngineBlown() && !other_racerInfo->GetIsTotalled() &&
                                                         !other_racerInfo->GetIsKnockedOut() && !other_racerInfo->IsFinishedRacing())) {
                        return !ShouldPauseSimulation(useControllerErrors);
                    }
                }
            }
            return false;
        }

        if (simable != nullptr) {
            IVehicle *vehicle;
            if (simable->QueryInterface(&vehicle)) {
                IVehicleAI *vehicleai = vehicle->GetAIVehiclePtr();
                if (vehicleai != nullptr) {
                    IPursuit *ipursuit = vehicleai->GetPursuit();
                    if ((ipursuit != nullptr) && ipursuit->IsPerpBusted()) {
                        return false;
                    }
                }
            }
        }
    }
    return !ShouldPauseSimulation(useControllerErrors);
}

bool FEManager::ShouldPauseSimulation(bool useControllerErrors) {
    if (!mInstance->bSuppressControllerError && mInstance->WaitingForControllerError() && useControllerErrors &&
        (UTL::Collections::Singleton<INIS>::Get() == nullptr) && (gMoviePlayer == nullptr)) {
        return true;
    }
    return mPauseRequest != 0;
}

void FEManager::RequestPauseSimulation(const char *reason) {
    mPauseReason[mPauseRequest++] = reason;
}

void FEManager::RequestUnPauseSimulation(const char *reason) {
    mPauseRequest--;
}

void FEManager::WantControllerError(int port) {
    if (port == -1) {
        return;
    }

    if (TheGameFlowManager.IsInGame() && (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode())) {
        ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        GRacerInfo *racerInfo;
        if (simable != nullptr) {
            racerInfo = GRaceStatus::Get().GetRacerInfo(simable);
        } else {
            racerInfo = nullptr;
        }
        if (racerInfo != nullptr) {
            IPlayer *player = racerInfo->GetSimable()->GetPlayer();
            ICountdown *icountdown;
            if (player->GetHud() != nullptr) {
                if (player->GetHud()->QueryInterface(&icountdown) && icountdown->IsActive()) {
                    return;
                }
            }
        }
    }

    bWantControllerError[port] = true;
}

bool FEManager::WaitingForControllerError() {
    for (int port = 0; port < 8; port++) {
        if (bWantControllerError[port]) {
            return true;
        }
    }
    return false;
}

void FEManager::StartFE() {
    if (mFirstBoot) {
        mFirstBoot = false;
        BootFlowManager::Init();
    } else {
        extern EAXSound *g_pEAXSound;
        g_pEAXSound->PlayFEMusic(-1);
        if (!CarViewer::haveLoadedOnce) {
            RideInfo ride;
            CarViewer::ShowCarScreen();
            FEDatabase->BuildCurrentRideForPlayer(0, &ride);
            CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_CATCHALL, eCARVIEWER_PLAYER1_CAR);
            CarViewer::haveLoadedOnce = true;
        }
        CarViewer::ShowAllCars();
    }

    bSuppressControllerError = false;
    bAllowControllerError = false;
    for (int port = 0; port < 8; port++) {
        bWantControllerError[port] = false;
    }
    mPauseRequest = 0;
    cFEng::Get()->QueuePackagePush(mFirstScreen, mFirstScreenArg,
#ifdef EA_BUILD_A124
                                   0,
#else
                                   mFirstScreenMask,
#endif
                                   false);
}

void FEManager::StopFE() {
    cFEngJoyInput::mInstance->JoyDisable(JOYSTICK_PORT_ALL, true);
    FEPackageManager::Get()->CloseAllPackages(0);
    BootFlowManager::Destroy();
    mEATraxDelay = 0;
}

void FEManager::Render() {
    if (DrawFEng) {
        cFEng::Get()->DrawForeground();
    }
}

// STRIPPED
void FEManager::UpdateJoyInput() {}

void SteeringWheels_StopAllForces() {
    if (SteeringWheelDevice::lgwheels != nullptr) {
        for (int mDeviceIndex = 0; mDeviceIndex < 2; mDeviceIndex++) {
            if (SteeringWheelDevice::lgwheels->IsConnected(mDeviceIndex)) {
                SteeringWheelDevice::lgwheels->StopConstantForce(mDeviceIndex);
                SteeringWheelDevice::lgwheels->StopSurfaceEffect(mDeviceIndex);
                SteeringWheelDevice::lgwheels->StopDamperForce(mDeviceIndex);
                SteeringWheelDevice::lgwheels->StopCarAirborne(mDeviceIndex);
                SteeringWheelDevice::lgwheels->StopSlipperyRoadEffect(mDeviceIndex);
                SteeringWheelDevice::lgwheels->PlaySpringForce(mDeviceIndex, 0, 200, 200);
            }
        }
    }
}

int GetPortsPlayer(int port) {
    JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
    if (player_port1 != -1 && player_port1 == port) {
        return 0;
    }
    JoystickPort player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
    if (player_port2 != -1 && player_port2 == port) {
        return 1;
    }
    return -1;
}

void FEManager::Update() {
    if (MemoryCard::GetInstance() != nullptr) {
        MemoryCard::GetInstance()->Tick(static_cast<int>(RealTimeElapsed * 1000.0f));
    }

    if (!Sim::Exists() || (Sim::Exists() && Sim::GetState() != Sim::STATE_ACTIVE) || (UTL::Collections::Singleton<INIS>::Get() != nullptr)) {
        SteeringWheels_StopAllForces();
    }

    if (cFEngJoyInput::mInstance != nullptr) {
        cFEngJoyInput::mInstance->HandleJoy();
    }

    for (int port = 0; port < 8; port++) {
        if (bWantControllerError[port]) {
            if (((UTL::Collections::Singleton<INIS>::Get() == nullptr) && (gMoviePlayer == nullptr)) || bAllowControllerError) {
                if (!bSuppressControllerError) {
                    if (TheGameFlowManager.IsInGame() && FEManager::IsPaused()) {
                        FEManager *feManager = FEManager::Get();
                        JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
                        feManager->ClearControllerError(static_cast<int>(player_port1));
                        JoystickPort player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
                        feManager->ClearControllerError(static_cast<int>(player_port2));
                    }

                    int maxPort = IOModule::GetIOModule().GetNumDevices();
                    for (int p = 0; p < maxPort; p++) {
                        InputDevice *device = IOModule::GetIOModule().GetDevice(p);
                        if (device != nullptr) {
                            device->StopVibration();
                        }
                    }

                    if (!cFEng::Get()->IsPackagePushed("ControllerUnplugged.fng")) {
                        cFEng::Get()->PushErrorPackage("ControllerUnplugged.fng", port, FEngMapJoyportToJoyParam(port));
                    }
                }
            }
            break;
        }
    }

    cFEng::Get()->Service();

    if (cFEng::Get()->IsErrorState()) {
        FEPackageManager::Get()->ErrorTick();
    } else {
        FEPackageManager::Get()->Tick();

        if (TheGameFlowManager.IsInFrontend()) {
            UpdateGarageCarLoaders();
        }

        if (DoScreenPrintf && !TheICEManager.IsEditorOn()) {
            FEPackage *pCurrentPkgWithControl = cFEng::Get()->FindPackageWithControl();
            if (pCurrentPkgWithControl != nullptr) {
                pCurrentPkgWithControl->GetName();
            }
        }

        gEasterEggs.HandleJoy();

        if (gMoviePlayer != nullptr) {
            gMoviePlayer->Update();
        }

        if (SummonChyronNow) {
            SummonChyron(nullptr, nullptr, nullptr);
            SummonChyronNow = 0;
        } else {
            if (mEATraxDelay >= 0) {
                mEATraxDelay--;
                if (mEATraxDelay == 0) {
                    SummonChyron(nullptr, nullptr, nullptr);
                }
            }
        }
    }
}

void FEManager::SetEATraxSecondButton() {
    if ((gMoviePlayer != nullptr) && gMoviePlayer->IsMoviePlaying()) {
        return;
    }

    if (!cFEng::Get()->IsPackagePushed("EA_Trax_Jukebox.fng") && TheGameFlowManager.IsInFrontend()) {
        MControlPathfinder(true, 0xffffffff, 0, 0).Send(UCrc32("Pathfinder5"));
    }
}

void FEManager::ExitOnlineGameplayBasedOnConnection() {}
