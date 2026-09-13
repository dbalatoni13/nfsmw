#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"

void *LoadingControllerScreen::mLoadingControllerScreenPtr;

LoadingControllerScreen::LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd), LoadingFinished(0) {
    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    TheGameFlowManager.GetState(); // unused

    GameTipToShow = nullptr;
    HideControllerConfig();
    SetupControllerConfig();
    PrepToShowControllerConfig();
}

LoadingControllerScreen::~LoadingControllerScreen() {
    ClearLoadedControllerTexture();
}

void LoadingControllerScreen::SetupControllerConfig() {
    if (!FEDatabase->IsCareerMode()) {
        cFEng::Get()->QueuePackageMessage(0xde511657, GetPackageName(), nullptr);
    }
    JoystickPort port = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
    char sztemp[32];
    int config = FEDatabase->GetPlayerSettings(0)->Config;
    for (int i = 0; i < 17; i++) {
        FEngSNPrintf(sztemp, 0x20, "BUTTON%d", i + 1);
        unsigned int obj_hash = FEHashUpper(sztemp);
        FEngSNPrintf(sztemp, 0x20, "BUTTON%d_I", i + 1);
        unsigned int img_hash = FEHashUpper(sztemp);
        unsigned int button_hash = FindButtonNameHashForFEString(config, i, port);
        if (button_hash != 0) {
            FEngSetVisible(GetPackageName(), obj_hash);
            FEngSetLanguageHash(GetPackageName(), obj_hash, button_hash);
            FEngSetVisible(GetPackageName(), img_hash);
        } else {
            FEngSetInvisible(GetPackageName(), obj_hash);
            FEngSetInvisible(GetPackageName(), img_hash);
        }
    }

    const u32 FEObj_DPadUp = 0x4592229c;
    if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog) {
        FEngSetTextureHash(GetPackageName(), FEObj_DPadUp, 0x148e38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), FEObj_DPadUp), 0xb30961b);
    }

    FEngSetInvisible(GetPackageName(), 0xf274b86);
    FEngSetInvisible(GetPackageName(), 0x673d77bc);
    FEngSetInvisible(GetPackageName(), 0x351ae442);
    const u32 FEObj_BUTTON15 = 0x81b57400;
    const u32 FEObj_BUTTON16 = 0x81b57401;
    const u32 FEObj_BUTTON17 = 0x81b57402;
    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON15, 0x2959349);
    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON16, 0x6851aaf5);
    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON17, 0x3b7f86d);
}

void LoadingControllerScreen::ShowControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, FEHASH_SHOW, true);
}

void LoadingControllerScreen::HideControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, FEHASH_HIDE, true);
    WhichControllerTexture = 0;
}

void LoadingControllerScreen::FinishLoadingControllerTextureCallback(uint32 p) {
    ShowControllerConfig();
}

void FinishLoadingControllerTextureCallbackBridge(uint32 p) {
    LoadingControllerScreen *ls;
    if (p != 0) {
        ls = reinterpret_cast<LoadingControllerScreen *>(p);
        ls->FinishLoadingControllerTextureCallback(0);
    }
}

void LoadingControllerScreen::PrepToShowControllerConfig() {
    JoystickPort port = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
    uint32 texture_hash = 0;
    int config = FEDatabase->GetPlayerSettings(0)->Config;
    if (IsJoystickTypeWheel(port)) {
        FEDatabase->GetPlayerSettings(0)->Config = CC_CONFIG_1;
        texture_hash = 0xb511476b;
    } else {
        texture_hash = 0xed543bac;
        if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog) {
            texture_hash = 0xed543bab;
        }
    }
    WhichControllerTexture = texture_hash;
    FEngSetTextureHash(GetPackageName(), 0x922a39c4, texture_hash);
    eLoadStreamingTexture(WhichControllerTexture, FinishLoadingControllerTextureCallbackBridge, (uint32)this, BMEMORY_MAIN_POOL);
}

void LoadingControllerScreen::ClearLoadedControllerTexture() {
    if (WhichControllerTexture) {
        eUnloadStreamingTexture(WhichControllerTexture);
    }
}

void LoadingControllerScreen::NotificationMessage(u32 msg, FEObject *obj, u32 p1, u32 p2) {}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(sizeof(LoadingControllerScreen), "LoadingControllerScreen", 0, 0);
}
