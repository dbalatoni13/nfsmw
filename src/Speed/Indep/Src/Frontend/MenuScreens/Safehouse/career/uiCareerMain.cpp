#include "uiCareerMain.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"

uiCareerCrib::uiCareerCrib(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
}

void uiCareerCrib::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case 0x1265ECE9:
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            return;
        case FEHASH_EXITCOMPLETE: {
            extern int IsMemcardEnabled;

            if (PrevButtonMessage != __PAD_BACK__) {
                return;
            }
            FEManager::Get()->SetGarageType(GARAGETYPE_MAIN_FE);
            FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER);
            if (IsMemcardEnabled) {
                FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER_MANAGER);
                cFEng::Get()->QueuePackageSwitch(GetPackageName(), 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            }
            return;
        }
        case dialog_message_yes: {
            const char *lastDDayRace;
            bool dday_flow_completed = false;

            lastDDayRace = GRaceDatabase::Get().GetDDayEndRace();

            dday_flow_completed =
                (SkipDDayRaces != 0) || GRaceDatabase::Get().IsCareerRaceComplete(GRaceDatabase::Get().GetRaceFromName(lastDDayRace)->GetEventHash());

            if (dday_flow_completed) {
                RaceStarter::StartCareerFreeRoam();
            } else {
                const char *firstDDayRace;
                if (!SkipDDayRaces) {
                    firstDDayRace = GRaceDatabase::Get().GetNextDDayRace();
                } else {
                    firstDDayRace = GRaceDatabase::Get().GetDDayEndRace();
                }
                GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(firstDDayRace);
                GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
                GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
                GRaceDatabase::Get().FreeCustomRace(race);
                RaceStarter::StartCareerFreeRoam();
            }
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
            return;
        }
        case dialog_message_no:
            return;
    }
}

void uiCareerCrib::Setup() {
    AddOption(new ("CResumeFreeRoam", 0) CResumeFreeRoam(0x12BB5EA2, 0x1BD185C, 0));

    AddOption(new ("CTop15", 0) CTop15(0x2C14AC23, 0x80B9FF9B, 0));
    AddOption(new ("CCarSelect", 0) CCarSelect(0xC6A1A6E0, 0xD5F627, 0));

    if (FEDatabase->GetCareerSettings()->HasRapSheet()) {
        AddOption(new ("CRapSheet", 0) CRapSheet(0x2FD8B206, 0xAC22F27E, 0));
    }

    if (IsMemcardEnabled) {
        AddOption(new ("CSave", 0) CSave(0x228B7E32, 0x1C8ACE, 0));
    }

    SetInitialOption(FEngGetLastButton(GetPackageName()));

    u32 hash = 0xE596C4A3;
    const u32 FEObj_TITLE = 0x3C458C1;
    const u32 FEObj_TITLESHADOW = 0xB5C74226;
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLE, hash);
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLESHADOW, hash);

    const char *szPercentUnit = "%";
    eLanguages currLang = GetCurrentLanguage();
    if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN ||
        currLang == eLANGUAGE_SWEDISH) {
        szPercentUnit = " %";
    }

    FEngSetScript(GetPackageName(), FEHashUpper("PLAYER_MASTER"), FEHashUpper("SHOW"), true);

    FEPrintf(GetPackageName(), FEHashUpper("CAREER_DATA"), "%d%%", FEDatabase->GetGameCompletionStats().m_nCareer, szPercentUnit);
    FEPrintf(GetPackageName(), FEHashUpper("BOUNTY_DATA"), "%$d", FEDatabase->GetPlayerCarStable(0)->GetTotalBounty());
    FEPrintf(GetPackageName(), FEHashUpper("CASH_DATA"), "%$d", FEDatabase->GetCareerSettings()->GetCash());

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}
