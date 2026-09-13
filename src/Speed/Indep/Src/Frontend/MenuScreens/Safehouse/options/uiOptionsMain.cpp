#include "uiOptionsMain.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsController.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

extern const char *gOnlineMainMenu;

UIOptionsMain::UIOptionsMain(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), //
      mCalledFromPauseMenu(sd->Arg != 0) {
    if (mCalledFromPauseMenu) {
        Options.SetIdleColor(0xFFFFAE40);
        Options.SetFadeColor(0x00FFAE40);
    } else {
        Options.SetIdleColor(0xFFFFFFFF);
        Options.SetFadeColor(0x00FFFFFF);
    }
    Setup();
}

void UIOptionsMain::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case __PAD_START__:
            FEDatabase->ClearGameMode(eFE_GAME_MODE_OPTIONS);
            StorePrevNotification(__PAD_START__, pobj, param1, param2);
            FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
            break;
        case __PAD_BACK__:
            FEDatabase->ClearGameMode(eFE_GAME_MODE_OPTIONS);
            StorePrevNotification(msg, pobj, param1, param2);
            if (mCalledFromPauseMenu) {
                FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
                break;
            }
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                const u32 FEObj_leavescreen = 0x587C018B;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        case __BUTTON_PRESSED__:
            if (FEngIsScriptRunning(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34)) {
                break;
            }
            StorePrevNotification(__BUTTON_PRESSED__, pobj, param1, param2);
            FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
            break;
        case FEHASH_EXITCOMPLETE:
            if (PrevButtonMessage == __PAD_START__) {
                new EUnPause();
                break;
            }
            if (PrevButtonMessage == __PAD_BACK__) {
                if (mCalledFromPauseMenu) {
                    cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 0, 0, false);
                    break;
                }
                if (FEDatabase->IsLANMode() || FEDatabase->IsOnlineMode()) {
                    ExitOptions(gOnlineMainMenu);
                } else {
                    ExitOptions("MainMenu.fng");
                }
                break;
            }
            if (PrevButtonMessage == __BUTTON_PRESSED__) {
                switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
                    case OC_AUDIO:
                    case OC_VIDEO:
                    case OC_GAMEPLAY:
                    case OC_PLAYER:
                    case OC_ONLINE:
                        if (mCalledFromPauseMenu && !FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
                            cFEng::Get()->QueuePackageSwitch("Pause_Options.fng", 1, 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("Options.fng", 0, 0, false);
                        }
                        break;
                    case OC_CREDITS:
                        cFEng::Get()->QueuePackageSwitch("Credits.fng", 0, 0, false);
                        break;
                    case OC_TRAILERS:
                        FEDatabase->SetGameMode(eFE_GAME_TRAILERS);
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                        break;
                    case OC_CONTROLS:
                        UIOptionsController::PortToConfigure = FEngMapJoyParamToJoyport(PrevParam1);
                        if (mCalledFromPauseMenu) {
                            cFEng::Get()->QueuePackageSwitch("Pause_Controller.fng", static_cast<int>(mCalledFromPauseMenu), 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("UI_OptionsController.fng", static_cast<int>(mCalledFromPauseMenu), 0, false);
                        }
                        break;
                    case OC_EATRAX:
                        cFEng::Get()->QueuePackageSwitch("EA_Trax_Jukebox.fng", 0, 0, false);
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
}

void UIOptionsMain::Setup() {
    AddOption(new ("OMAudio", 0) OMAudio(0xF37AF144, 0xE76CD783, 0));
    AddOption(new ("OMVideo", 0) OMVideo(0x8A006328, 0xE8E24508, 0));
    AddOption(new ("OMGameplay", 0) OMGameplay(0x4DF98FB2, 0xD0CF6EE1, 0));
    AddOption(new ("OMPlayer", 0) OMPlayer(0xD708EFEF, 0xF760EABE, 0));
    AddOption(new ("OMController", 0) OMController(0xA04A7B26, 0x04DC6DB5, 0));

    if (!mCalledFromPauseMenu && !FEDatabase->IsOnlineMode()) {
        AddOption(new ("OMEATrax", 0) OMEATrax(0xC52CCBF6, 0xDCFB6B36, 0));
        AddOption(new ("OMCredits", 0) OMCredits(0x51009E20, 0x0905101F, 0));
    }

    this->SetInitialOption(FEngGetLastButton(GetPackageName()));

    if (!mCalledFromPauseMenu) {
        const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
        FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0x4ECA678C);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x863404B5, 0x1D7BB6C9);
    }

    RefreshHeader();
}

void UIOptionsMain::ExitOptions(const char *nextPackage) {
    if (FEDatabase->IsOptionsDirty() && IsMemcardEnabled) {
        MemcardEnter(GetPackageName(), nextPackage, 0x400B3, nullptr, nullptr, 0, 0);
    } else {
        cFEng::Get()->QueuePackageSwitch(nextPackage, 0, 0, false);
    }
}
