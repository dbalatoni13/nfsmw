#include "FEManager.hpp"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEngine.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
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
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

struct ChoppedMiniMapManager {
    static void Init();
};

struct FadeScreen {
    static bool IsFadeScreenOn();
};

extern bool DrawFEng;
extern int SummonChyronNow;
extern int DoScreenPrintf;
extern float RealTimeElapsed;

void InitFEngMemoryPool();
void InitChyron();
void SummonChyron(char *, char *, char *);
void UpdateGarageCarLoaders();
unsigned long FEngMapJoyportToJoyParam(int);

struct LGWheels {
    bool IsConnected(int channel);
    void StopConstantForce(int channel);
    void StopSurfaceEffect(int channel);
    void StopDamperForce(int channel);
    void StopCarAirborne(int channel);
    void StopSlipperyRoadEffect(int channel);
    void PlaySpringForce(int channel, char offset, unsigned char saturation, short coefficient);
};

struct SteeringWheelDevice {
    static LGWheels *lgwheels;
};

void SteeringWheels_StopAllForces();

FEManager::FEManager()
    : bSuppressControllerError(false) //
    , bAllowControllerError(false) //
    , mFirstScreen(nullptr) //
    , mFirstScreenArg(0) //
    , mFirstScreenMask(0xFF) //
    , mGarageType(GARAGETYPE_NONE) //
    , mPreviousGarageType(GARAGETYPE_NONE) //
    , mGarageBackground(nullptr) //
    , mFirstBoot(true) //
    , mEATraxDelay(0) //
    , mEATraxFirstButton(false) {
    for (int port = 0; port < 8; port++) {
        bWantControllerError[port] = false;
    }
}

void FEManager::Init() {
    if (!mInstance) {
        mInstance = new FEManager;
    }
    InitFEngMemoryPool();
    LoadingScreen::InitLoadingScreen();
    LoadingTips::InitLoadingTipsScreen();
    LoadingControllerScreen::InitLoadingControllerScreen();
    InitChyron();
    cFEngGameInterface::pInstance = new cFEngGameInterface;
    ChoppedMiniMapManager::Init();
    cFEng::Init();
    cFEngRender::mInstance = new cFEngRender;
    FEManager::Get()->SetGarageType(GARAGETYPE_MAIN_FE);
    uiRepSheetRivalFlow::Init();
}

void FEManager::InitInput() {
    cFEngJoyInput::mInstance = new cFEngJoyInput();
}

void FEManager::Destroy() {
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

bool FEManager::IsOkayToRequestPauseSimulation(int playerIndex, bool useControllerErrors,
                                                bool okIfAutoSaveActive) {
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
        ISimable *simable =
            IPlayer::GetList(PLAYER_LOCAL)[static_cast< unsigned int >(playerIndex)]->GetSimable();
        GRacerInfo *racerInfo;
        if (simable) {
            racerInfo = GRaceStatus::Get().GetRacerInfo(simable);
        } else {
            racerInfo = nullptr;
        }

        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
            if (!GRaceStatus::Get().GetIsTimeLimited() ||
                GRaceStatus::Get().GetRaceTimeRemaining() > 0.0f) {
                if (!racerInfo ||
                    (!racerInfo->GetIsEngineBlown() && !racerInfo->GetIsTotalled() &&
                     !racerInfo->GetIsKnockedOut() && !racerInfo->IsFinishedRacing())) {
                    goto done;
                }

                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    ISimable *other_simable =
                        IPlayer::GetList(PLAYER_LOCAL)[static_cast< unsigned int >(playerIndex != 1)]
                            ->GetSimable();
                    GRacerInfo *other_racerInfo;
                    if (other_simable) {
                        other_racerInfo = GRaceStatus::Get().GetRacerInfo(other_simable);
                    } else {
                        other_racerInfo = nullptr;
                    }
                    if (!other_racerInfo ||
                        (!other_racerInfo->GetIsEngineBlown() &&
                         !other_racerInfo->GetIsTotalled() &&
                         !other_racerInfo->GetIsKnockedOut() &&
                         !other_racerInfo->IsFinishedRacing())) {
                        goto done;
                    }
                }
            }
            return false;
        }

        if (simable) {
            IVehicle *vehicle;
            if (simable->QueryInterface(&vehicle)) {
                IVehicleAI *vehicleai = vehicle->GetAIVehiclePtr();
                if (vehicleai) {
                    IPursuit *ipursuit = vehicleai->GetPursuit();
                    if (ipursuit && ipursuit->ShouldEnd()) {
                        return false;
                    }
                }
            }
        }
    }

done:
    return !ShouldPauseSimulation(useControllerErrors);
}

bool FEManager::ShouldPauseSimulation(bool useControllerErrors) {
    if (!mInstance->bSuppressControllerError && mInstance->WaitingForControllerError() &&
        useControllerErrors && !UTL::Collections::Singleton<INIS>::Get() && !gMoviePlayer) {
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

    if (TheGameFlowManager.IsInGame() &&
        (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode())) {
        ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        GRacerInfo *racerInfo;
        if (simable) {
            racerInfo = GRaceStatus::Get().GetRacerInfo(simable);
        } else {
            racerInfo = nullptr;
        }
        if (racerInfo) {
            IPlayer *player = racerInfo->GetSimable()->GetPlayer();
            if (player->GetHud()) {
                ICountdown *icountdown;
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
    cFEng::Get()->QueuePackagePush(mFirstScreen, mFirstScreenArg, mFirstScreenMask, false);
}

void FEManager::StopFE() {
    cFEngJoyInput::mInstance->JoyDisable(kJP_NumPorts, true);
    FEPackageManager::Get()->CloseAllPackages(0);
    BootFlowManager::Destroy();
    mEATraxDelay = 0;
}

void FEManager::Render() {
    if (DrawFEng) {
        cFEng::Get()->DrawForeground();
    }
}

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
    if (FEDatabase->GetPlayersJoystickPort(0) != -1 &&
        FEDatabase->GetPlayersJoystickPort(0) == port) {
        return 0;
    }
    if (FEDatabase->GetPlayersJoystickPort(1) != -1 &&
        FEDatabase->GetPlayersJoystickPort(1) == port) {
        return 1;
    }
    return -1;
}

void FEManager::UpdateJoyInput() {
    if (cFEngJoyInput::mInstance) {
        cFEngJoyInput::mInstance->CheckUnplugged();
    }
}

void FEManager::Update() {
    if (MemoryCard::GetInstance()) {
        MemoryCard::GetInstance()->Tick(static_cast< int >(RealTimeElapsed * 1000.0f));
    }

    if (!Sim::Exists() || (Sim::Exists() && Sim::GetState() != Sim::STATE_ACTIVE) ||
        UTL::Collections::Singleton<INIS>::Get()) {
        SteeringWheels_StopAllForces();
    }

    if (cFEngJoyInput::mInstance) {
        cFEngJoyInput::mInstance->HandleJoy();
    }

    int port;
    for (port = 0; port < 8; port++) {
        if (bWantControllerError[port]) {
            if ((!UTL::Collections::Singleton<INIS>::Get() && !gMoviePlayer) ||
                bAllowControllerError) {
                if (!bSuppressControllerError) {
                    if (TheGameFlowManager.IsInGame() && FEManager::IsPaused()) {
                        FEManager *feManager = FEManager::Get();
                        JoystickPort player_port1 =
                            static_cast< JoystickPort >(FEDatabase->GetPlayersJoystickPort(0));
                        feManager->ClearControllerError(static_cast< int >(player_port1));
                        JoystickPort player_port2 =
                            static_cast< JoystickPort >(FEDatabase->GetPlayersJoystickPort(1));
                        feManager->ClearControllerError(static_cast< int >(player_port2));
                    }

                    int maxPort = IOModule::GetIOModule().GetNumDevices();
                    for (int p = 0; p < maxPort; p++) {
                        InputDevice *device = IOModule::GetIOModule().GetDevice(p);
                        if (device) {
                            device->PollDevice();
                        }
                    }

                    if (!cFEng::Get()->IsPackagePushed("ControllerUnplugged.fng")) {
                        cFEng *eng = cFEng::Get();
                        unsigned long joyParam = FEngMapJoyportToJoyParam(port);
                        eng->PushErrorPackage("ControllerUnplugged.fng", port, joyParam);
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
            if (pCurrentPkgWithControl) {
                pCurrentPkgWithControl->GetName();
            }
        }

        gEasterEggs.HandleJoy();

        if (gMoviePlayer) {
            gMoviePlayer->Update();
        }

        if (SummonChyronNow) {
            SummonChyron(0, 0, 0);
            SummonChyronNow = 0;
        } else {
            if (mEATraxDelay >= 0) {
                mEATraxDelay--;
                if (mEATraxDelay == 0) {
                    SummonChyron(0, 0, 0);
                }
            }
        }
    }
}

void FEManager::SetEATraxSecondButton() {
    if (gMoviePlayer && static_cast< unsigned int >(gMoviePlayer->GetStatus() - 3) < 3) {
        return;
    }

    if (!cFEng::Get()->IsPackagePushed("EA_Trax_Jukebox.fng") &&
        TheGameFlowManager.IsInFrontend()) {
        MControlPathfinder msg(true, 0xffffffff, 0, 0);
        msg.Send(UCrc32("Pathfinder5"));
    }
}

void FEManager::ExitOnlineGameplayBasedOnConnection() {
}
