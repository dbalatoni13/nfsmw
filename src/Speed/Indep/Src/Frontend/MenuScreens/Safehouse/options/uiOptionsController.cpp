#include "uiOptionsController.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetButtonTexture(FEImage* img, unsigned int tex_hash);
unsigned long FEHashUpper(const char* str);
unsigned int FindButtonNameHashForFEString(int config, int string_number, JoystickPort player);
bool IsJoystickTypeWheel(JoystickPort port);

void eLoadStreamingTexture(unsigned int* textures, int count, void (*callback)(void*),
                           void* param, int pool);
void eUnloadStreamingTexture(unsigned int* textures, int count);

int UIOptionsController::PortToConfigure = 0;
int UIOptionsController::isWheelConfig = 0;

void MyFinishLoadingControllerTextureCallbackBridge(unsigned int p);

UIOptionsController::UIOptionsController(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) //
    , WhichControllerTexture(0) //
    , PrevJoystickType(-1) //
    , mCalledFromPauseMenu(sd->Arg != 0) //
    , NeedSetup(true) {
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        cFEng::Get()->QueuePackageMessage(0x7DB7B6D7, GetPackageName(), 0);
        const char* pkg = GetPackageName();
        int player = GetPlayerToEditForOptions();
        unsigned int lang = 0x7B070985;
        if (player == 0) {
            lang = 0x7B070984;
        }
        FEngSetLanguageHash(pkg, 0x53BF826D, lang);
    }

    oldConfig = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;
    reinterpret_cast<int&>(oldVibration) = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble;
    reinterpret_cast<int&>(oldDriveWithAnalog) = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;

    CalcControllerTextureToLoad();

    if (isWheelConfig) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config =
            static_cast<eControllerConfig>(0);
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = true;
    }

    Setup();
}

UIOptionsController::~UIOptionsController() {
    ClearLoadedControllerTexture();
}

bool UIOptionsController::OptionsDidNotChange() {
    bool result =
        (oldConfig == FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config);
    if (oldVibration != FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble) {
        result = false;
    }
    if (oldDriveWithAnalog !=
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog) {
        result = false;
    }
    return result;
}

void UIOptionsController::NotificationMessage(unsigned long msg, FEObject* pobj,
                                              unsigned long param1, unsigned long param2) {
    if (msg == 0x9120409E || msg == 0xB5971BF1) {
        int joyPort = FEngMapJoyParamToJoyport(param1);
        FEDatabase->SetPlayersJoystickPort(GetPlayerToEditForOptions(), joyPort);
    }

    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
    case 0xE1FDE1D1: {
        bool dirty = false;
        if (FEDatabase->IsOptionsDirty() || !OptionsDidNotChange()) {
            dirty = true;
        }
        FEDatabase->SetOptionsDirty(dirty);

        if (mCalledFromPauseMenu) {
            cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 1, 0, false);
        } else {
            if (FEDatabase->IsOnlineMode()) {
                cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            }
        }
        break;
    }
    case 0x911AB364:
        if (OptionsDidNotChange()) {
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        } else {
            DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1),
                                            0x70E01038, 0x417B25E4, 0x775DBA97, 0x34DC1BCF,
                                            0x34DC1BCF, static_cast<eDialogFirstButtons>(1),
                                            GetLocalizedString(0xE9CB802F));
        }
        break;
    case 0x775DBA97:
        RestoreOriginals();
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        break;
    case 0xD9FEEC59:
    case 0x5073EF13:
        if (!OptionsDidNotChange()) {
            char buf[128];
            FEngSNPrintf(buf, 128, GetLocalizedString(0xBA463431),
                         GetPlayerToEditForOptions() + 1);
            const char* pkgName = GetPackageName();
            const char* dlg_pkg;
            if (mCalledFromPauseMenu) {
                dlg_pkg = "InGameDialog.fng";
            } else {
                dlg_pkg = "Dialog.fng";
            }
            DialogInterface::ShowTwoButtons(pkgName, dlg_pkg,
                                            static_cast<eDialogTitle>(1), 0x70E01038, 0x417B25E4,
                                            0x9A5AD46D, 0xA2A07AC4, 0x34DC1BCF,
                                            static_cast<eDialogFirstButtons>(1), buf);
        } else {
            cFEng::Get()->QueueGameMessage(0x9A5AD46D, 0, 0xFF);
        }
        break;
    case 0xA2A07AC4:
        RestoreOriginals();
        TogglePlayer();
        break;
    case 0x9A5AD46D: {
        bool dirty = false;
        if (FEDatabase->IsOptionsDirty() || !OptionsDidNotChange()) {
            dirty = true;
        }
        FEDatabase->SetOptionsDirty(dirty);
        TogglePlayer();
        break;
    }
    case 0xB5AF2461:
        if (mCalledFromPauseMenu) {
            new EUnPause();
        }
        break;
    case 0x92B703B5:
        SetupControllerConfig();
        break;
    case 0xC98356BA:
        DetectControllers();
        break;
    case 0x34DC1BCF:
        return;
    }
}

void UIOptionsController::Setup() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        FEngSetScript(GetPackageName(), 0x8A41F5B9, 0x16A259, true);
    }

    if (!FEDatabase->IsCareerMode()) {
        cFEng::Get()->QueuePackageMessage(0xDE511657, GetPackageName(), 0);
    }

    COConfig* config = new COConfig(true);
    config->SetBackingOffsetX(-295.0f);
    AddToggleOption(config, true);

    COVibration* vibration = new COVibration(GetPlayerToEditForOptions(), true);
    int idx = AddToggleOption(vibration, true);
    Options.GetNode(idx - 1)->SetBackingOffsetX(-295.0f);

    FEngSetLanguageHash(GetPackageName(), 0x53BF826D,
                        GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);

    SetInitialOption(0);
    HideControllerConfig();
    PrepToShowControllerConfig();
}

void UIOptionsController::SetupControllerConfig() {
    unsigned int newTex = CalcControllerTextureToLoad();
    if (WhichControllerTexture != newTex) {
        ClearLoadedControllerTexture();
        PrepToShowControllerConfig();
    }

    JoystickPort port = static_cast<JoystickPort>(GetPlayerToEditForOptions());
    int config = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;
    char sztemp[32];

    for (int i = 0; i < 17; i++) {
        FEngSNPrintf(sztemp, 32, "CButton_%d", i + 1);
        unsigned int obj_hash = FEHashUpper(sztemp);
        FEngSNPrintf(sztemp, 32, "BUTTON_%d", i + 1);
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

    if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog) {
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x4592229C), 0x148E38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x4592229C), 0x0B30961B);
    }

    FEngSetInvisible(GetPackageName(), 0x0F274B86);
    FEngSetInvisible(GetPackageName(), 0x673D77BC);
    FEngSetInvisible(GetPackageName(), 0x351AE442);

    FEngSetTextureHash(GetPackageName(), 0x81B57400, 0x02959349);
    FEngSetTextureHash(GetPackageName(), 0x81B57401, 0x6851AAF5);
    FEngSetTextureHash(GetPackageName(), 0x81B57402, 0x03B7F86D);
}

void UIOptionsController::DetectControllers() {
    unsigned int newTex = CalcControllerTextureToLoad();
    if (WhichControllerTexture != newTex) {
        ClearLoadedControllerTexture();
        PrepToShowControllerConfig();
    }
}

void UIOptionsController::ClearLoadedControllerTexture() {
    if (WhichControllerTexture != 0) {
        unsigned int tex = WhichControllerTexture;
        eUnloadStreamingTexture(&tex, 1);
    }
}

void UIOptionsController::FinishLoadingTexCallback() {
    SetupControllerConfig();
    ShowControllerConfig();
    SetupControllerConfig();
}

unsigned int UIOptionsController::CalcControllerTextureToLoad() {
    unsigned int texture_hash;
    isWheelConfig = 0;

    GetPlayerToEditForOptions();
    if (IsJoystickTypeWheel(static_cast<JoystickPort>(GetPlayerToEditForOptions()))) {
        texture_hash = 0xB511476B;
        isWheelConfig = 1;
    } else {
        if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog) {
            texture_hash = 0xED543BAB;
        } else {
            texture_hash = 0xED543BAC;
        }
    }
    return texture_hash;
}

void UIOptionsController::PrepToShowControllerConfig() {
    if (isWheelConfig) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config =
            static_cast<eControllerConfig>(0);
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = true;
        cFEng::Get()->QueueGameMessage(0x92B703B5, 0, 0xFF);
    }

    HideControllerConfig();
    unsigned int tex = CalcControllerTextureToLoad();
    WhichControllerTexture = tex;
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0x922A39C4), tex);

    unsigned int texArr = WhichControllerTexture;
    eLoadStreamingTexture(&texArr, 1,
                          reinterpret_cast<void (*)(void*)>(MyFinishLoadingControllerTextureCallbackBridge),
                          this, 0);
}

void UIOptionsController::ShowControllerConfig() {
    FEngSetVisible(GetPackageName(), 0x3248E720);
}

void UIOptionsController::HideControllerConfig() {
    FEngSetInvisible(GetPackageName(), 0x3248E720);
    WhichControllerTexture = 0;
}

void UIOptionsController::RestoreOriginals() {
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config = oldConfig;
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = oldDriveWithAnalog;
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble = oldVibration;
}

void UIOptionsController::TogglePlayer() {
    SetPlayerToEditForOptions(GetPlayerToEditForOptions() == 0);

    oldConfig = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;
    reinterpret_cast<int&>(oldVibration) = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble;
    reinterpret_cast<int&>(oldDriveWithAnalog) = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;

    const char* pkg = GetPackageName();
    int player = GetPlayerToEditForOptions();
    unsigned int lang = 0x7B070985;
    if (player == 0) {
        lang = 0x7B070984;
    }
    FEngSetLanguageHash(pkg, 0x53BF826D, lang);

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }

    SetupControllerConfig();
}

void MyFinishLoadingControllerTextureCallbackBridge(unsigned int p) {
    UIOptionsController* ls = reinterpret_cast<UIOptionsController*>(p);
    ls->FinishLoadingTexCallback();
}
