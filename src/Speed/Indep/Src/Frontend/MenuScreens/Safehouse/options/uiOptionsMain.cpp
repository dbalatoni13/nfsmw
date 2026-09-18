#include "uiOptionsMain.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Event.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsController.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

// Defined in another unit.
extern int IsMemcardEnabled;
extern const char *gOnlineMainMenu;

UIOptionsMain::UIOptionsMain(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    bool fromPauseMenu = sd->Arg != 0;
    mCalledFromPauseMenu = fromPauseMenu;
    if (fromPauseMenu) {
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
    case 0xB5AF2461:
        FEDatabase->ClearGameMode((eFEGameModes)0x10);
        StorePrevNotification(0xB5AF2461, pobj, param1, param2);
        FEngSetScript(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD, true);
        break;
    case 0x911AB364:
        FEDatabase->ClearGameMode((eFEGameModes)0x10);
        StorePrevNotification(0x911AB364, pobj, param1, param2);
        if (mCalledFromPauseMenu != 0) {
            FEngSetScript(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD, true);
        } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLanMode()) {
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        }
        break;
    case 0x0C407210:
        if (FEngIsScriptRunning(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD)) {
            break;
        }
        StorePrevNotification(0x0C407210, pobj, param1, param2);
        FEngSetScript(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD, true);
        break;
    case 0xE1FDE1D1:
        if (PrevButtonMessage == 0xB5AF2461) {
            new EUnPause;
        } else if (PrevButtonMessage == 0x911AB364) {
            if (mCalledFromPauseMenu != 0) {
                cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 0, 0, 0);
            } else if (FEDatabase->IsLanMode() || FEDatabase->IsOnlineMode()) {
                ExitOptions(gOnlineMainMenu);
            } else {
                ExitOptions("MainMenu.fng");
            }
        } else if (PrevButtonMessage == 0x0C407210) {
            switch (FEDatabase->GetUserProfile(0)->GetOptions()->CurrentCategory) {
            case OC_AUDIO:
            case OC_VIDEO:
            case OC_GAMEPLAY:
            case OC_PLAYER:
            case OC_ONLINE:
                if (mCalledFromPauseMenu != 0 && !FEDatabase->IsOnlineMode() && !FEDatabase->IsLanMode()) {
                    cFEng::Get()->QueuePackageSwitch("Pause_Options.fng", 1, 0, 0);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Options.fng", 0, 0, 0);
                }
                break;
            case OC_CREDITS:
                cFEng::Get()->QueuePackageSwitch("Credits.fng", 0, 0, 0);
                break;
            case OC_TRAILERS:
                FEDatabase->SetGameMode(eFE_GAME_TRAILERS);
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, 0);
                break;
            case OC_CONTROLS:
                UIOptionsController::PortToConfigure = FEngMapJoyParamToJoyport(PrevParam1);
                if (mCalledFromPauseMenu != 0) {
                    cFEng::Get()->QueuePackageSwitch("Pause_Controller.fng", mCalledFromPauseMenu, 0, 0);
                } else {
                    cFEng::Get()->QueuePackageSwitch("UI_OptionsController.fng", 0, 0, 0);
                }
                break;
            case OC_EATRAX:
                cFEng::Get()->QueuePackageSwitch("EA_Trax_Jukebox.fng", 0, 0, 0);
                break;
            }
        }
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
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    if (!mCalledFromPauseMenu) {
        const u32 FEObj_TITLEGROUP = 0xB71B576D;
        FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0x4ECA678C);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x863404B5, 0x1D7BB6C9);
    }
    RefreshHeader();
}

void UIOptionsMain::ExitOptions(const char *nextPackage) {
    if (FEDatabase->IsOptionsDirty() && IsMemcardEnabled != 0) {
        MemcardEnter(GetPackageName(), nextPackage, 0x000400B3, 0, 0, 0, 0);
    } else {
        cFEng::Get()->QueuePackageSwitch(nextPackage, 0, 0, false);
    }
}
