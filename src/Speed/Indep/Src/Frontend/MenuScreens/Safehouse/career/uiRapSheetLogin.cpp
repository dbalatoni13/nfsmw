#include "uiRapSheetLogin.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);

uiRapSheetLogin::uiRapSheetLogin(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , screen(sd->Arg) //
    , returnToMainMenu(false)
{
    Setup();
}

uiRapSheetLogin::~uiRapSheetLogin() {}

void uiRapSheetLogin::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x7EABCA56 || msg == 0x406415E3) {
        if (screen == 0) {
            g_pEAXSound->StopUISoundFX(UISND_RAPSHEET_LOGIN);
        } else if (screen == 2) {
            g_pEAXSound->StopUISoundFX(UISND_RAPSHEET_LOGIN2);
        }
        screen = 3;
    } else if (msg == 0x911AB364) {
        returnToMainMenu = true;
    } else if (msg == 0xE1FDE1D1) {
        if (returnToMainMenu) {
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            FEDatabase->ClearGameMode(eFE_GAME_MODE_RAP_SHEET);
        } else if (screen - 2U < 2) {
            cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("RapSheetLogin2.fng", 2, 0, false);
        }
    }
}

void uiRapSheetLogin::Setup() {
    if (screen == 2) {
        const char* name = FEDatabase->CurrentUserProfiles[0]->GetProfileName();
        FEPrintf(GetPackageName(), 0x3CC94D6, "> %s", name);
    }
}
