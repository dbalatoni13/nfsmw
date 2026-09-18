#include "uiOptionsTrailers.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"

UIOptionsTrailers::UIOptionsTrailers(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
}

void UIOptionsTrailers::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg != 0x0C407210) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
    case 0x911AB364:
        StorePrevNotification(0x911AB364, pobj, param1, param2);
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        break;
    case 0x0C407210:
        cFEng::Get()->QueuePackageMessage(__ENABLE_INPUTS__, 0, 0);
        Options.GetCurrentOption()->React(GetPackageName(), 0x0C407210, pobj, param1, param2);
        break;
    case 0xD05FC3A3:
        Options.GetCurrentOption()->React(GetPackageName(), 0xD05FC3A3, pobj, param1, param2);
        break;
    case 0xE1FDE1D1:
        if (PrevButtonMessage == 0x911AB364) {
            FEDatabase->ClearGameMode(eFE_GAME_TRAILERS);
            FEDatabase->GetUserProfile(0)->GetOptions()->CurrentCategory = (eOptionsCategory)-1;
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, 0);
        }
        break;
    }
}

void UIOptionsTrailers::Setup() {
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    GarageMainScreen::GetInstance()->CancelCameraPush();
    FEngSetLanguageHash(GetPackageName(), 0xB71B576D, 0xB65A46D8);
    RefreshHeader();
}
