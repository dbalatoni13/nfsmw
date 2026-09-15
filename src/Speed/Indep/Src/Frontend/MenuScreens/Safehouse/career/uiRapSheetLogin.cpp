#include "uiRapSheetLogin.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

uiRapSheetLogin::uiRapSheetLogin(ScreenConstructorData *sd)
    : MenuScreen(sd),  //
      screen(sd->Arg), //
      returnToMainMenu(false) {
    Setup();
}

void uiRapSheetLogin::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x7EABCA56:
        case __PAD_ACCEPT__:
            if (screen == 0) {
                g_pEAXSound->StopUISoundFX(UISND_RAPSHEET_LOGIN);
            } else if (screen == 2) {
                g_pEAXSound->StopUISoundFX(UISND_RAPSHEET_LOGIN2);
            }
            screen = 3;
            break;
        case __PAD_BACK__:
            returnToMainMenu = true;
            break;
        case FEHASH_EXITCOMPLETE:
            if (returnToMainMenu) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                FEDatabase->ClearGameMode(eFE_GAME_MODE_RAP_SHEET);
            } else if (screen >= 2 && screen < 4) {
                cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("RapSheetLogin2.fng", 2, 0, false);
            }
            break;
    }
}

void uiRapSheetLogin::Setup() {
    if (screen == 2) {
        UserProfile *prof = FEDatabase->GetUserProfile(0);
        FEPrintf(GetPackageName(), 0x3CC94D6, "> %s", prof->GetProfileName());
    }
}
