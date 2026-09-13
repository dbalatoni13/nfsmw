#include "uiOptionsController.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

int UIOptionsController::PortToConfigure = 0;
int UIOptionsController::isWheelConfig = 0;

UIOptionsController::UIOptionsController(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    WhichControllerTexture = 0;
    PrevJoystickType = -1;
    mCalledFromPauseMenu = sd->Arg != 0;
    NeedSetup = true;

    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        cFEng::Get()->QueuePackageMessage(0x7DB7B6D7, GetPackageName(), nullptr);
        FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);
    }

    oldConfig = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;
    oldVibration = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble;
    oldDriveWithAnalog = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;

    CalcControllerTextureToLoad();

    if (isWheelConfig) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config = CC_CONFIG_1;
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = true;
    }

    Setup();
}

UIOptionsController::~UIOptionsController() {
    ClearLoadedControllerTexture();
}

bool UIOptionsController::OptionsDidNotChange() {
    bool result = (oldConfig == FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config);

    result &= oldVibration == FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble;

    result &= oldDriveWithAnalog == FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;

    return result;
}

void UIOptionsController::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == __PAD_LEFT__ || msg == __PAD_RIGHT__) {
        int joyPort = FEngMapJoyParamToJoyport(param1);
        FEDatabase->SetPlayersJoystickPort(GetPlayerToEditForOptions(), joyPort);
    }

    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case FEHASH_EXITCOMPLETE: {
            FEDatabase->SetOptionsDirty(FEDatabase->IsOptionsDirty() || !OptionsDidNotChange());

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
        case __PAD_BACK__:
            if (OptionsDidNotChange()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            } else {
                DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x775DBA97,
                                                dialog_message_no, dialog_message_no, first_dialog_button2, GetLocalizedString(0xE9CB802F));
            }
            break;
        case 0x775DBA97:
            RestoreOriginals();
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case __PAD_RTRIGGER__:
        case __PAD_LTRIGGER__:
            if (!OptionsDidNotChange()) {
                char buf[128];
                FEngSNPrintf(buf, 128, GetLocalizedString(0xBA463431), GetPlayerToEditForOptions() + 1);
                DialogInterface::ShowTwoButtons(GetPackageName(), mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng", dialog_alert,
                                                LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x9A5AD46D, 0xA2A07AC4, dialog_message_no,
                                                first_dialog_button2, buf);
            } else {
                cFEng::Get()->QueueGameMessage(0x9A5AD46D, nullptr, 0xFF);
            }
            break;
        case 0xA2A07AC4:
            RestoreOriginals();
            TogglePlayer();
            break;
        case 0x9A5AD46D: {
            FEDatabase->SetOptionsDirty(FEDatabase->IsOptionsDirty() || !OptionsDidNotChange());
            TogglePlayer();
            break;
        }
        case __PAD_START__:
            if (mCalledFromPauseMenu) {
                new EUnPause();
            }
            break;
        case 0x92B703B5:
            SetupControllerConfig();
            break;
        case FEMSG_SCREEN_TICK:
            DetectControllers();
            break;
        case dialog_message_no:
            return;
    }
}

void UIOptionsController::Setup() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        FEngSetScript(GetPackageName(), 0x8A41F5B9, FEHASH_HIDE, true);
    }

    if (!FEDatabase->IsCareerMode()) {
        cFEng::Get()->QueuePackageMessage(0xDE511657, GetPackageName(), 0);
    }

    AddToggleOption(new ("COConfig", 0) COConfig(true), true);

    uint32 index = AddToggleOption(new ("COVibration", 0) COVibration(GetPlayerToEditForOptions(), true), true);
    Options.GetNode(index - 1)->SetBackingOffsetX(-295.0f);

    FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);

    SetInitialOption(0);
    HideControllerConfig();
    PrepToShowControllerConfig();
}

void UIOptionsController::SetupControllerConfig() {
    if (WhichControllerTexture != CalcControllerTextureToLoad()) {
        ClearLoadedControllerTexture();
        PrepToShowControllerConfig();
    }

    JoystickPort port = static_cast<JoystickPort>(GetPlayerToEditForOptions());
    char sztemp[32];
    int config = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;

    for (int i = 0; i < 17; i++) {
        FEngSNPrintf(sztemp, 32, "CButton_%d", i + 1);
        uint32 obj_hash = FEHashUpper(sztemp);
        FEngSNPrintf(sztemp, 32, "BUTTON_%d", i + 1);
        uint32 img_hash = FEHashUpper(sztemp);

        uint32 button_hash = FindButtonNameHashForFEString(config, i, port);
        if (button_hash != 0) {
            FEngSetVisible(GetPackageName(), obj_hash);
            FEngSetLanguageHash(GetPackageName(), obj_hash, button_hash);
            FEngSetVisible(GetPackageName(), img_hash);
        } else {
            FEngSetInvisible(GetPackageName(), obj_hash);
            FEngSetInvisible(GetPackageName(), img_hash);
        }
    }

    const u32 FEObj_DPadUp = 0x4592229C;

    if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog) {
        FEngSetTextureHash(GetPackageName(), FEObj_DPadUp, 0x148E38);
    } else {
        FEngSetButtonTexture(FEngFindImage(GetPackageName(), FEObj_DPadUp), 0x0B30961B);
    }

    const u32 FEObj_BUTTON15 = 0x81B57400;
    const u32 FEObj_BUTTON16 = 0x81B57401;
    const u32 FEObj_BUTTON17 = 0x81B57402;

    FEngSetInvisible(GetPackageName(), 0x0F274B86);
    FEngSetInvisible(GetPackageName(), 0x673D77BC);
    FEngSetInvisible(GetPackageName(), 0x351AE442);

    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON15, 0x02959349);
    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON16, 0x6851AAF5);
    FEngSetTextureHash(GetPackageName(), FEObj_BUTTON17, 0x03B7F86D);
}

void UIOptionsController::DetectControllers() {
    if (WhichControllerTexture != CalcControllerTextureToLoad()) {
        ClearLoadedControllerTexture();
        PrepToShowControllerConfig();
    }
}

void UIOptionsController::ClearLoadedControllerTexture() {
    if (WhichControllerTexture != 0) {
        eUnloadStreamingTexture(WhichControllerTexture);
    }
}

void UIOptionsController::FinishLoadingTexCallback() {
    SetupControllerConfig();
    ShowControllerConfig();
    SetupControllerConfig();
}

void MyFinishLoadingControllerTextureCallbackBridge(uint32 p) {
    UIOptionsController *ls = reinterpret_cast<UIOptionsController *>(p);
    ls->FinishLoadingTexCallback();
}

uint32 UIOptionsController::CalcControllerTextureToLoad() {
    uint32 texture_hash;
    isWheelConfig = 0;

    int config = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config;

    JoystickPort port = static_cast<JoystickPort>(GetPlayerToEditForOptions());

    if (IsJoystickTypeWheel(port)) {
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
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config = CC_CONFIG_1;
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = true;
        cFEng::Get()->QueueGameMessage(0x92B703B5, nullptr, 0xFF);
    }

    HideControllerConfig();
    WhichControllerTexture = CalcControllerTextureToLoad();
    FEngSetTextureHash(GetPackageName(), 0x922A39C4, WhichControllerTexture);

    eLoadStreamingTexture(WhichControllerTexture, MyFinishLoadingControllerTextureCallbackBridge, reinterpret_cast<uint32>(this),
                          BMEMORY_DEFAULT_POOL);
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
    oldVibration = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble;
    oldDriveWithAnalog = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;

    FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }

    SetupControllerConfig();
}
