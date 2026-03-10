#include "FEManager.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct cFEngRender;
struct EasterEggs;

void InitFEngMemoryPool();
void InitLoadingScreen();
void InitLoadingTipsScreen();
void InitLoadingControllerScreen();
void InitChyron();
void DismissChyron();
void SummonChyron(char*, char*, char*);
void ChoppedMiniMapManager_Init();
void PlayFEMusic_EAXSound(EAXSound* sound, int id);
void SteeringWheels_StopAllForces();
void UpdateGarageCarLoaders();
unsigned long FEngMapJoyportToJoyParam(int port);
int FadeScreen_IsOn();
void BootFlowManager_Init();
void BootFlowManager_Destroy();
void EasterEggs_HandleJoy(EasterEggs* eggs);

extern cFEngRender* cFEngRender_mInstance;
extern int DrawFEng;
extern int DoScreenPrintf;
extern int SummonChyronNow;
extern float RealTimeElapsed;
extern bool CarViewer_haveLoadedOnce;
extern EasterEggs gEasterEggs;

void ShowCarScreen_CarViewer();
void BuildCurrentRideForPlayer_cFrontendDatabase(cFrontendDatabase* db, int player, RideInfo* ride);
void SetRideInfo_CarViewer(RideInfo* ride, int reason, int which_car);
void ShowAllCars_CarViewer();

FEManager* FEManager::mInstance = nullptr;
int FEManager::mPauseRequest = 0;
const char* FEManager::mPauseReason[8] = {};

FEManager::FEManager() {
    mFirstScreenMask = 0xff;
    mFirstBoot = true;
    mEATraxFirstButton = false;
    bSuppressControllerError = false;
    bAllowControllerError = false;
    mFirstScreen = nullptr;
    mFirstScreenArg = 0;
    mGarageType = GARAGETYPE_NONE;
    mPreviousGarageType = GARAGETYPE_NONE;
    mGarageBackground = nullptr;
    mEATraxDelay = 0;
    for (int i = 0; i < 8; i++) {
        bWantControllerError[i] = false;
    }
}

void FEManager::Init() {
    if (mInstance == nullptr) {
        mInstance = new FEManager();
    }
    InitFEngMemoryPool();
    InitLoadingScreen();
    InitLoadingTipsScreen();
    InitLoadingControllerScreen();
    InitChyron();
    cFEngGameInterface::pInstance = new cFEngGameInterface();
    ChoppedMiniMapManager_Init();
    cFEng::Init();
    cFEngRender_mInstance = new cFEngRender();
    FEManager::Get()->SetGarageType(GARAGETYPE_MAIN_FE);
    uiRepSheetRivalFlow::Init();
}

void FEManager::InitInput() {
    cFEngJoyInput::mInstance = new cFEngJoyInput();
}

FEManager* FEManager::Get() {
    return mInstance;
}

eGarageType FEManager::GetGarageType() {
    return mGarageType;
}

void FEManager::SetGarageType(eGarageType pGarageType) {
    eGarageType old = GetGarageType();
    mGarageType = pGarageType;
    mPreviousGarageType = old;
}

const char* FEManager::GetGarageNameFromType() {
    switch (mGarageType) {
    case GARAGETYPE_NONE: return "";
    case GARAGETYPE_MAIN_FE: return "FE_Garage";
    case GARAGETYPE_CAREER_SAFEHOUSE: return "Safehouse_Garage";
    case GARAGETYPE_CUSTOMIZATION_SHOP: return "CustomShop_Garage";
    case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM: return "BackRoom_Garage";
    case GARAGETYPE_CAR_LOT: return "CarLot_Garage";
    default: return "";
    }
}

const char* FEManager::GetGaragePrefixFromType(eGarageType pGarageType) {
    switch (pGarageType) {
    case GARAGETYPE_NONE: return "";
    case GARAGETYPE_MAIN_FE: return "FE";
    case GARAGETYPE_CAREER_SAFEHOUSE: return "SH";
    case GARAGETYPE_CUSTOMIZATION_SHOP:
    case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM: return "CS";
    case GARAGETYPE_CAR_LOT: return "CL";
    default: return "";
    }
}

bool FEManager::IsOkayToRequestPauseSimulation(int playerIndex, bool useControllerErrors, bool okIfAutoSaveActive) {
    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) return false;
    if (cFEng::Get()->IsPackagePushed("ScreenPrintf")) return false;
    if (FadeScreen_IsOn()) return false;
    if (cFEng::Get()->IsPackagePushed("FePlayerPaused.fng")) return false;
}

bool FEManager::ShouldPauseSimulation(bool useControllerErrors) {
        && useControllerErrors && UTL::Collections::Singleton<INIS>::Get() == nullptr
        && gMoviePlayer == nullptr) return true;
    return mPauseRequest != 0;
}

void FEManager::RequestPauseSimulation(const char* reason) {
    mPauseReason[mPauseRequest] = reason;
    mPauseRequest++;
}

void FEManager::RequestUnPauseSimulation(const char* reason) {
    mPauseRequest--;
}

void FEManager::WantControllerError(int port) {
    if (port == -1) return;
    bWantControllerError[port] = true;
}

bool FEManager::WaitingForControllerError() {
    for (int i = 0; i < 8; i++) {
        if (bWantControllerError[i]) return true;
    }
    return false;
}

void FEManager::StartFE() {
        PlayFEMusic_EAXSound(g_pEAXSound, -1);
            RideInfo ride;
            ShowCarScreen_CarViewer();
            BuildCurrentRideForPlayer_cFrontendDatabase(FEDatabase, 0, &ride);
            SetRideInfo_CarViewer(&ride, 2, 0);
            CarViewer_haveLoadedOnce = true;
        }
        ShowAllCars_CarViewer();
    } else {
        mFirstBoot = false;
        BootFlowManager_Init();
    }
    bAllowControllerError = false;
    bSuppressControllerError = false;
    for (int i = 0; i < 8; i++) bWantControllerError[i] = false;
    mPauseRequest = 0;
    cFEng::Get()->QueuePackagePush(mFirstScreen, mFirstScreenArg, mFirstScreenMask, false);
}

void FEManager::StopFE() {
    cFEngJoyInput::mInstance->JoyDisable();
    FEPackageManager::Get()->CloseAllPackages(0);
    BootFlowManager_Destroy();
    mEATraxDelay = 0;
}

void FEManager::Render() {
    if (DrawFEng) cFEng::Get()->DrawForeground();
}

void FEManager::Update() {
    if (MemoryCard::GetInstance() != nullptr)
        MemoryCard::GetInstance()->Tick(static_cast<int>(RealTimeElapsed * 1000.0f));
        SteeringWheels_StopAllForces();
    if (cFEngJoyInput::mInstance != nullptr) cFEngJoyInput::mInstance->HandleJoy();
    int port;
    for (port = 0; port < 8; port++) {
        if (bWantControllerError[port]) {
                if (TheGameFlowManager.IsInGame() && mPauseRequest > 0) {
                    int p1 = FEDatabase->GetPlayersJoystickPort(0);
                    ClearControllerError(p1);
                    int p2 = FEDatabase->GetPlayersJoystickPort(1);
                    ClearControllerError(p2);
                }
                IOModule& iomod = IOModule::GetIOModule();
                for (int d = 0; d < iomod.fNumDevices; d++) {
                    InputDevice* dev = iomod.GetDevice(d);
                    if (dev != nullptr) dev->PollDevices();
                }
                    unsigned long jp = FEngMapJoyportToJoyParam(port);
                    cFEng::Get()->PushErrorPackage("ControllerUnplugged.fng", port, jp);
                }
            }
            break;
        }
    }
    cFEng::Get()->Service();
        FEPackageManager::Get()->Tick();
        if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) UpdateGarageCarLoaders();
        EasterEggs_HandleJoy(&gEasterEggs);
        if (gMoviePlayer != nullptr) gMoviePlayer->Update();
        if (SummonChyronNow) { SummonChyron(nullptr, nullptr, nullptr); SummonChyronNow = 0; }
        else if (mEATraxDelay > -1) { mEATraxDelay--; if (mEATraxDelay == 0) SummonChyron(nullptr, nullptr, nullptr); }
    } else {
        FEPackageManager::Get()->ErrorTick();
    }
}

void FEManager::SetEATraxSecondButton() {
    if (gMoviePlayer != nullptr) return;
        DismissChyron();
}
