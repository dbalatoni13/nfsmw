#include "uiCareerManager.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiCareerMain.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"

void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
const char* GetLocalizedString(unsigned int hash);
int FEngMapJoyParamToJoyport(int feng_param);
void MemcardEnter(const char* from, const char* to, unsigned int op, void (*pTermFunc)(void*),
                  void* pTermFuncParam, unsigned int msgSuccess, unsigned int msgFailed);

uiCareerManager::uiCareerManager(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
}

uiCareerManager::~uiCareerManager() {}

void uiCareerManager::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                          unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
    case 0x1265ECE9:
        GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
        break;
    case 0xE1FDE1D1:
        if (PrevButtonMessage == 0x911AB364) {
            if (FEDatabase->GetCareerSettings()->IsGameOver()) {
                cFEng::Get()->QueuePackageSwitch(GetPackageName(), 0, 0, false);
            } else {
                FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
                cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            }
        }
        break;
    case 0x7E998E5E:
        FEDatabase->RefreshCurrentRide();
        break;
    }
}

void uiCareerManager::Setup() {
    IconOption* pLoadOption;

    if (FEDatabase->GetCareerSettings()->HasCareerStarted()) {
        if (!FEDatabase->GetCareerSettings()->IsGameOver()) {
            AddOption(new CResumeCareer(0xC1C089CE, 0xE072DB21, 0));
        }

        CStartNewCareer* startNew = new CStartNewCareer(0xE7353BE7, 0x17E18F87, 0);
        startNew->SetReactImmediately(true);
        AddOption(startNew);
    } else {
        CStartNewCareer* startNew = new CStartNewCareer(0xE7353BE7, 0x6005281E, 0);
        startNew->SetReactImmediately(true);
        AddOption(startNew);
    }

    CLoadCareer* loadCareer = new CLoadCareer(0x2287E063, 0x18ECFF, 0);
    loadCareer->SetReactImmediately(true);
    AddOption(loadCareer);
    pLoadOption = loadCareer;

    if (FEDatabase->GetCareerSettings()->IsGameOver()) {
        int index = Options.GetOptionIndex(pLoadOption);
        if (bFadeInIconsImmediately) {
            Options.StartFadeIn();
        }
        Options.SetInitialPos(index);
    } else {
        int lastButton = FEngGetLastButton(GetPackageName());
        if (bFadeInIconsImmediately) {
            Options.StartFadeIn();
        }
        Options.SetInitialPos(lastButton);
    }

    FEngSetLanguageHash(GetPackageName(), 0x3C458C1, 0x8FFF61F2);
    FEngSetLanguageHash(GetPackageName(), 0xB5C74226, 0x8FFF61F2);

    if (FEDatabase->bProfileLoaded) {
        FEngSetScript(GetPackageName(), 0xC87422F7, 0x1CA7C0, true);
        FEPrintf(GetPackageName(), 0xEB406FEC, "%s",
                 FEDatabase->GetUserProfile(0)->GetProfileName());
    }

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

void CResumeCareer::React(const char* pkg_name, unsigned int data, FEObject* obj,
                          unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        bool should_go_into_epic_pursuit = false;
        cFrontendDatabase* db = FEDatabase;
        signed char port = FEngMapJoyParamToJoyport(param1);
        db->SetPlayersJoystickPort(0, port);
        FEDatabase->GetCareerSettings()->ResumeCareer();

        if (!FEDatabase->GetCareerSettings()->HasBeatenCareer()) {
            GRaceDatabase &rdb = GRaceDatabase::Get();
            GRaceParameters* parms = rdb.GetRaceFromName(rdb.GetFinalBossRace());
            if (rdb.IsCareerRaceComplete(parms->GetEventHash()))
                should_go_into_epic_pursuit = true;
        }

        if (FEDatabase->GetCareerSettings()->GetCurrentBin() != 16 &&
            !should_go_into_epic_pursuit) {
            GManager::Get().SetStartingFreeRoamFromSafeHouse();
            cFEng::Get()->QueuePackageSwitch("IG_SafehouseMain.fng", 0, 0, false);
        }
    }
}

void CStartNewCareer::React(const char* pkg_name, unsigned int data, FEObject* obj,
                            unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        cFrontendDatabase* db = FEDatabase;
        signed char port = FEngMapJoyParamToJoyport(param1);
        db->SetPlayersJoystickPort(0, port);

        if (FEDatabase->GetCareerSettings()->HasCareerStarted() ||
            !FEDatabase->bProfileLoaded) {
            MemcardEnter(pkg_name, pkg_name, 0x80063, 0, 0, 0, 0);
        } else {
            FEDatabase->GetCareerSettings()->StartNewCareer(true);
            cFEng::Get()->QueuePackageSwitch(pkg_name, 0, 0, false);
        }
    }
}

void CLoadCareer::React(const char* pkg_name, unsigned int data, FEObject* obj,
                        unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        cFrontendDatabase* db = FEDatabase;
        signed char port = FEngMapJoyParamToJoyport(param1);
        db->SetPlayersJoystickPort(0, port);
        MemcardEnter(pkg_name, pkg_name, 0x413, 0, 0, 0x7E998E5E, 0x8867412D);
    }
}
