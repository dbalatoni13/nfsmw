#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"

void *LoadingControllerScreen::mLoadingControllerScreenPtr;

LoadingControllerScreen::LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }
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
    int config = FEDatabase->GetPlayerSettings(0)->Config;
    for (int i = 0; i < 17; i++) {
        char sztemp[32];
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
    if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog != 0) {
        FEngSetTextureHash(GetPackageName(), 0x4592229c, 0x148e38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x4592229c), 0xb30961b);
    }
    FEngSetInvisible(GetPackageName(), 0xf274b86);
    FEngSetInvisible(GetPackageName(), 0x673d77bc);
    FEngSetInvisible(GetPackageName(), 0x351ae442);
    FEImage *img0 = FEngFindImage(GetPackageName(), 0x81b57400);
    FEngSetTextureHash(img0, 0x2959349);
    FEImage *img1 = FEngFindImage(GetPackageName(), 0x81b57401);
    FEngSetTextureHash(img1, 0x6851aaf5);
    FEImage *img2 = FEngFindImage(GetPackageName(), 0x81b57402);
    FEngSetTextureHash(img2, 0x3b7f86d);
}

void LoadingControllerScreen::ShowControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, 0x001CA7C0, true);
}

void LoadingControllerScreen::HideControllerConfig() {
    FEngSetScript(GetPackageName(), 0x3248E720, 0x0016A259, true);
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
    unsigned int texHash;
    if (!IsJoystickTypeWheel(static_cast<JoystickPort>(FEDatabase->PlayerJoyports[0]))) {
        texHash = 0xed543bac;
        if (FEDatabase->CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[0].DriveWithAnalog != 0) {
            texHash = 0xed543bab;
        }
    } else {
        FEDatabase->CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[0].Config = CC_CONFIG_1;
        texHash = 0xb511476b;
    }
    WhichControllerTexture = texHash;
    FEImage *img = FEngFindImage(GetPackageName(), 0x922a39c4);
    FEngSetTextureHash(img, texHash);
    eLoadStreamingTexture(WhichControllerTexture, FinishLoadingControllerTextureCallbackBridge, (uint32)this, 0);
}

void LoadingControllerScreen::ClearLoadedControllerTexture() {
    unsigned int tex[1];
    tex[0] = WhichControllerTexture;
    if (tex[0]) {
        eUnloadStreamingTexture(tex, 1);
    }
}

void LoadingControllerScreen::NotificationMessage(u32 msg, FEObject *obj, u32 p1, u32 p2) {}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(0x38, nullptr, 0, 0);
}
