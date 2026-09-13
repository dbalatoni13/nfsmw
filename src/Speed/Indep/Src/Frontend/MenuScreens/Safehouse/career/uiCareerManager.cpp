#include "uiCareerManager.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

uiCareerManager::uiCareerManager(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
}

void uiCareerManager::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case FEMSG_INIT_STARTED:
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            break;
        case FEHASH_EXITCOMPLETE:
            if (PrevButtonMessage == __PAD_BACK__) {
                if (FEDatabase->GetCareerSettings()->IsGameOver()) {
                    cFEng::Get()->QueuePackageSwitch(GetPackageName(), 0, 0, false);
                } else {
                    FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
                    cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
                }
                if (FEDatabase->GetCareerSettings()->HasCareerStarted()) {
                    // Unsure where this goes, but its empty so it doesn't matter
                }
            }
            break;
        case 0x7E998E5E:
            FEDatabase->RefreshCurrentRide();
            break;
    }
}

void uiCareerManager::Setup() {
    IconOption *pLoadOption;

    if (FEDatabase->GetCareerSettings()->HasCareerStarted()) {
        if (!FEDatabase->GetCareerSettings()->IsGameOver()) {
            AddOption(new ("CResumeCareer", 0) CResumeCareer(0xC1C089CE, 0xE072DB21, 0));
        }
        AddOption(new ("CStartNewCareer", 0) CStartNewCareer(0xE7353BE7, 0x17E18F87, 0));
    } else {
        AddOption(new ("CStartNewCareer", 0) CStartNewCareer(0xE7353BE7, 0x6005281E, 0));
    }

    pLoadOption = new ("CLoadCareer", 0) CLoadCareer(0x2287E063, 0x18ECFF, 0);
    AddOption(pLoadOption);

    if (FEDatabase->GetCareerSettings()->IsGameOver()) {
        SetInitialOption(Options.GetOptionIndex(pLoadOption));
    } else {
        SetInitialOption(FEngGetLastButton(GetPackageName()));
    }

    u32 hash = 0x8FFF61F2;
    const u32 FEObj_TITLE = 0x3C458C1;
    const u32 FEObj_TITLESHADOW = 0xB5C74226;
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLE, hash);
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLESHADOW, hash);

    if (FEDatabase->bProfileLoaded) {
        FEngSetScript(GetPackageName(), 0xC87422F7, FEHASH_SHOW, true);
        FEPrintf(GetPackageName(), 0xEB406FEC, "%s", FEDatabase->GetMultiplayerProfile(0)->GetProfileName());
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}
