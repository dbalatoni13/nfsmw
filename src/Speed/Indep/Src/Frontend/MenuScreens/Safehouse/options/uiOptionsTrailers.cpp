#include "uiOptionsTrailers.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
unsigned char FEngGetLastButton(const char* pkg_name);

UIOptionsTrailers::UIOptionsTrailers(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
}

void UIOptionsTrailers::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                            unsigned long param2) {
    if (msg != 0x0c407210) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
    case 0x911ab364:
        StorePrevNotification(0x911ab364, pobj, param1, param2);
        cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        break;
    case 0x0c407210:
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, nullptr, nullptr);
        Options.GetCurrentOption()->React(GetPackageName(), 0x0c407210, pobj, param1, param2);
        break;
    case 0xd05fc3a3:
        Options.GetCurrentOption()->React(GetPackageName(), 0xd05fc3a3, pobj, param1, param2);
        break;
    case 0xe1fde1d1:
        if (PrevButtonMessage == 0x911ab364) {
            FEDatabase->ClearGameMode(eFE_GAME_TRAILERS);
            FEDatabase->GetOptionsSettings()->CurrentCategory = static_cast<eOptionsCategory>(-1);
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
        }
        break;
    }
}

void UIOptionsTrailers::Setup() {
    unsigned char lastButton = FEngGetLastButton(GetPackageName());

    if (bFadeInIconsImmediately) {
        Options.StartFadeIn();
    }

    Options.SetInitialPos(lastButton);
    GarageMainScreen::GetInstance()->CancelCameraPush();
    FEngSetLanguageHash(GetPackageName(), 0xb71b576d, 0xb65a46d8);
    RefreshHeader();
}
