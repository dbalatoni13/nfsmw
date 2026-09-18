#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

bool IsJoystickTypeWheel(JoystickPort port);
void FinishLoadingControllerTextureCallbackBridge(unsigned int p);

void *LoadingControllerScreen::mLoadingControllerScreenPtr = nullptr;

LoadingControllerScreen::LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd), LoadingFinished(0) {
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
        cFEng::Get()->QueuePackageMessage(0xDE511657, GetPackageName(), nullptr);
    }

    JoystickPort port = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
    int config = FEDatabase->GetPlayerSettings(0)->Config;
    char sztemp[32];

    for (int i = 0; i <= 16; i++) {
        FEngSNPrintf(sztemp, 0x20, "CButton_%d", i + 1);
        unsigned int obj_hash = FEHashUpper(sztemp);
        FEngSNPrintf(sztemp, 0x20, "BUTTON_%d", i + 1);
        unsigned int img_hash = FEHashUpper(sztemp);
        unsigned int button_hash = FindButtonNameHashForFEString(config, i, port);
        if (button_hash != 0) {
            FEngSetVisible(FEngFindObject(GetPackageName(), obj_hash));
            FEngSetLanguageHash(GetPackageName(), obj_hash, button_hash);
            FEngSetVisible(FEngFindObject(GetPackageName(), img_hash));
        } else {
            FEngSetInvisible(FEngFindObject(GetPackageName(), obj_hash));
            FEngSetInvisible(FEngFindObject(GetPackageName(), img_hash));
        }
    }

    if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog) {
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x4592229C), 0x00148E38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x4592229C), 0x0B30961B);
    }

    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x0F274B86));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x673D77BC));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x351AE442));
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x81B57400), 0x02959349);
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x81B57401), 0x6851AAF5);
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x81B57402), 0x03B7F86D);
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

void FinishLoadingControllerTextureCallbackBridge(unsigned int p) {
    if (p) {
        reinterpret_cast<LoadingControllerScreen *>(p)->FinishLoadingControllerTextureCallback(0);
    }
}

void LoadingControllerScreen::PrepToShowControllerConfig() {
    unsigned int texture_hash = 0;

    if (IsJoystickTypeWheel(static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0)))) {
        FEDatabase->GetPlayerSettings(0)->Config = CC_CONFIG_1;
        texture_hash = 0xB511476B;
    } else {
        texture_hash = 0xED543BAC;
        if (FEDatabase->GetPlayerSettings(0)->DriveWithAnalog) {
            texture_hash = 0xED543BAB;
        }
    }

    WhichControllerTexture = texture_hash;
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x922A39C4), texture_hash);

    unsigned int tex = WhichControllerTexture;
    eLoadStreamingTexture(&tex, 1, reinterpret_cast<void (*)(void *)>(FinishLoadingControllerTextureCallbackBridge), this, 0);
}

void LoadingControllerScreen::ClearLoadedControllerTexture() {
    if (WhichControllerTexture != 0) {
        eUnloadStreamingTexture(WhichControllerTexture);
    }
}

void LoadingControllerScreen::NotificationMessage(u32 msg, FEObject *obj, u32 p1, u32 p2) {}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(sizeof(LoadingControllerScreen), "LoadingControllerScreen", 0, 0);
}
