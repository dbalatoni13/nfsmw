#include "uiOptionsTrailers.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"

UIOptionsTrailers::UIOptionsTrailers(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
}

void UIOptionsTrailers::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg != __BUTTON_PRESSED__) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case __PAD_BACK__:
            StorePrevNotification(__PAD_BACK__, pobj, param1, param2);
            cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            break;
        case __BUTTON_PRESSED__:
            cFEng::Get()->QueuePackageMessage(0x8cb81f09, nullptr, nullptr);
            Options.GetCurrentOption()->React(GetPackageName(), __BUTTON_PRESSED__, pobj, param1, param2);
            break;
        case dialog_message_yes:
            Options.GetCurrentOption()->React(GetPackageName(), dialog_message_yes, pobj, param1, param2);
            break;
        case FEHASH_EXITCOMPLETE:
            if (PrevButtonMessage == __PAD_BACK__) {
                FEDatabase->ClearGameMode(eFE_GAME_TRAILERS);
                FEDatabase->GetOptionsSettings()->CurrentCategory = static_cast<eOptionsCategory>(-1);
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            }
            break;
    }
}

void UIOptionsTrailers::Setup() {
    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;

    SetInitialOption(FEngGetLastButton(GetPackageName()));
    GarageMainScreen::GetInstance()->CancelCameraPush();
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0xb65a46d8);
    RefreshHeader();
}
