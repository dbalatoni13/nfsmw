#include "uiOptionsTrailers.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct GarageMainScreen {
    static GarageMainScreen *GetInstance();
    void CancelCameraPush();
};

void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int language);
int FEngGetLastButton(const char *pkg_name);

UIOptionsTrailers::UIOptionsTrailers(ScreenConstructorData *sd)
    : IconScrollerMenu(sd) {
    Setup();
}

void UIOptionsTrailers::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1,
                                            unsigned long param2) {
    if (msg != 0xC407210) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    if (msg == 0x911AB364) {
        StorePrevNotification(msg, pobj, param1, param2);
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
    } else if (msg == 0xC407210) {
        cFEng::Get()->QueuePackageMessage(0x8CB81F09, 0, 0);
        Options.GetCurrentOption()->React(GetPackageName(), 0xC407210, pobj, param1, param2);
    } else if (msg == 0xD05FC3A3) {
        Options.GetCurrentOption()->React(GetPackageName(), 0xD05FC3A3, pobj, param1, param2);
    } else if (msg == 0xE1FDE1D1) {
        if (PrevButtonMessage == 0x911AB364) {
            FEDatabase->ClearGameMode(eFE_GAME_TRAILERS);
            FEDatabase->GetOptionsSettings()->CurrentCategory = static_cast<eOptionsCategory>(-1);
            cFEng::Get()->QueuePackageSwitch("OptionsMain.fng", 0, 0, 0);
        }
    }
}

void UIOptionsTrailers::Setup() {
    int lastButton = FEngGetLastButton(GetPackageName());
    if (bFadeInIconsImmediately) {
        SetInitialOption(lastButton);
    }

    GarageMainScreen::GetInstance()->CancelCameraPush();

    FEngSetLanguageHash(GetPackageName(), 0xB71B576D, 0xB65A46D8);
    RefreshHeader();
}
