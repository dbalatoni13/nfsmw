#include "ELoadingScreenOn.hpp"

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"

extern const char *LoadingBootName;
extern int bSawLoadingScreen;


ELoadingScreenOn::ELoadingScreenOn(LoadingScreen::LoadingScreenTypes pLoadingType) : Event(0x10), fLoadingType(pLoadingType) {
    const char *load_screen;
    int screen_arg = pLoadingType;
    eGameTips tipID = LoadingTips::WhatTipScreenShouldIUseToday(pLoadingType);

    if (tipID != GAME_TIP_NONE) {
        if (tipID == GAME_TIP_USE_CONTROLLER_CONFIG) {
            if (eIsWidescreen()) {
                load_screen = "WS_Loading_Controller.fng";
            } else {
                load_screen = "Loading_Controller.fng";
            }
        } else {
            load_screen = "Loading_Tips.fng";
            LoadingTips::SetDoneLoading(false);
            screen_arg = tipID;
        }
    } else {
        if (eIsWidescreen()) {
            load_screen = "WS_Loading.fng";
        } else {
            load_screen = "Loading.fng";
        }
    }

    SetLoadingScreenPackageName(load_screen);

    if (cFEng::Get()->IsPackageInControl(LoadingBootName)) {
        cFEng::Get()->QueuePackagePop(1);
    }

    int JoyPort = FEDatabase->GetPlayersJoystickPort(0);

    FEManager::Get()->AllowControllerError(false);

    if (!cFEng::Get()->IsPackagePushed(load_screen)) {
        cFEng::Get()->QueuePackagePush(load_screen, screen_arg, FEngMapJoyportToJoyParam(JoyPort), false);
    }

    bSawLoadingScreen = 0;

    if (FadeScreen::IsFadeScreenOn()) {
        new EFadeScreenOff(FEHASH_15_IN);
    }
}

ELoadingScreenOn::~ELoadingScreenOn() {
}

const char *ELoadingScreenOn::GetEventName() const {
    return "ELoadingScreenOn";
}

void ELoadingScreenOn_MakeEvent_Callback(const void *staticData) {
    new ELoadingScreenOn(((ELoadingScreenOn::StaticData *) staticData)->fLoadingType);
}
