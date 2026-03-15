#include "uiOptionsMain.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsController.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
bool FEngIsScriptRunning(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash);
void MemcardEnter(const char* from, const char* to, unsigned int op, void (*pTermFunc)(void*),
                  void* pTermFuncParam, unsigned int msgSuccess, unsigned int msgFailed);
int FEngMapJoyParamToJoyport(int feng_param);

extern const char* gOnlineMainMenu;

UIOptionsMain::UIOptionsMain(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) //
    , mCalledFromPauseMenu(sd->Arg != 0) {
    if (mCalledFromPauseMenu) {
        Options.SetIdleColor(0xFFFFAE40);
        Options.SetFadeColor(0x00FFAE40);
    } else {
        Options.SetIdleColor(0xFFFFFFFF);
        Options.SetFadeColor(0x00FFFFFF);
    }
    Setup();
}

void UIOptionsMain::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                        unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
    case 0xB5AF2461:
        FEDatabase->ClearGameMode(eFE_GAME_MODE_OPTIONS);
        StorePrevNotification(0xB5AF2461, pobj, param1, param2);
        goto SetScript;
    case 0x911AB364:
        FEDatabase->ClearGameMode(eFE_GAME_MODE_OPTIONS);
        StorePrevNotification(msg, pobj, param1, param2);
        if (!mCalledFromPauseMenu) {
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
            }
            return;
        }
        goto SetScript;
    case 0x0C407210:
        if (FEngIsScriptRunning(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34)) {
            return;
        }
        StorePrevNotification(0x0C407210, pobj, param1, param2);
SetScript:
        FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
        return;
    case 0xE1FDE1D1:
        if (PrevButtonMessage == 0xB5AF2461) {
            new EUnPause();
            return;
        }
        if (PrevButtonMessage == 0x911AB364) {
            if (mCalledFromPauseMenu) {
                cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 0, 0, false);
                return;
            }
            if (FEDatabase->IsLANMode() || FEDatabase->IsOnlineMode()) {
                ExitOptions(gOnlineMainMenu);
            } else {
                ExitOptions("MainMenu.fng");
            }
            return;
        }
        if (PrevButtonMessage == 0x0C407210) {
            eOptionsCategory curCat = FEDatabase->GetOptionsSettings()->CurrentCategory;
            switch (curCat) {
            case OC_AUDIO:
            case OC_VIDEO:
            case OC_GAMEPLAY:
            case OC_PLAYER:
            case OC_ONLINE:
                if (mCalledFromPauseMenu && !FEDatabase->IsOnlineMode() &&
                    !FEDatabase->IsLANMode()) {
                    cFEng::Get()->QueuePackageSwitch("Pause_Options.fng", 1, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Options.fng", 0, 0, false);
                }
                return;
            case OC_CREDITS:
                cFEng::Get()->QueuePackageSwitch("Credits.fng", 0, 0, false);
                return;
            case OC_TRAILERS:
                FEDatabase->SetGameMode(eFE_GAME_TRAILERS);
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                return;
            case OC_CONTROLS:
                UIOptionsController::PortToConfigure = FEngMapJoyParamToJoyport(PrevParam1);
                if (mCalledFromPauseMenu) {
                    cFEng::Get()->QueuePackageSwitch("Pause_Controller.fng", 0, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("UI_OptionsController.fng", 0, 0, false);
                }
                return;
            case OC_EATRAX:
                cFEng::Get()->QueuePackageSwitch("EA_Trax_Jukebox.fng", 0, 0, false);
                return;
            default:
                return;
            }
        }
        return;
    default:
        return;
    }
}

void UIOptionsMain::Setup() {
    AddOption(new OMAudio(0xF37AF144, 0xE76CD783, 0));
    AddOption(new OMVideo(0x8A006328, 0xE8E24508, 0));
    AddOption(new OMGameplay(0x4DF98FB2, 0xD0CF6EE1, 0));
    AddOption(new OMPlayer(0xD708EFEF, 0xF760EABE, 0));
    AddOption(new OMController(0xA04A7B26, 0x04DC6DB5, 0));

    if (!mCalledFromPauseMenu && !FEDatabase->IsOnlineMode()) {
        AddOption(new OMEATrax(0xC52CCBF6, 0xDCFB6B36, 0));
        AddOption(new OMCredits(0x51009E20, 0x0905101F, 0));
    }

    int lastButton = FEngGetLastButton(GetPackageName());
    if (bFadeInIconsImmediately) {
        Options.bDelayUpdate = false;
        Options.bFadingOut = false;
        Options.StartFadeIn();
    }

    Options.SetInitialPos(lastButton);

    const unsigned long FEObj_TITLEGROUP = 0xB71B576D;
    if (!mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0x4ECA678C);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x863404B5, 0x1D7BB6C9);
    }

    RefreshHeader();
}

void UIOptionsMain::ExitOptions(const char* nextPackage) {
    if (FEDatabase->IsOptionsDirty() && IsMemcardEnabled) {
        MemcardEnter(GetPackageName(), nextPackage, 0x400B3, 0, 0, 0, 0);
    } else {
        cFEng::Get()->QueuePackageSwitch(nextPackage, 0, 0, false);
    }
}

int GetPlayerToEditForOptions() {
    return UIOptionsScreen::PlayerToEdit;
}

void SetPlayerToEditForOptions(int player) {
    UIOptionsScreen::PlayerToEdit = player;
}
