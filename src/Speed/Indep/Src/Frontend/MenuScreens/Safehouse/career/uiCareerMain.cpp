#include "uiCareerMain.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"

extern int iCurrentViewBin;

uiCareerCrib::uiCareerCrib(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
}

void uiCareerCrib::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case 0x1265ECE9:
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            return;
        case 0xE1FDE1D1:
            if (PrevButtonMessage != 0x911AB364) {
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
        case 0xD05FC3A3: {
            bool dday_flow_completed = false;
            if (!SkipDDayRaces) {
                GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(Attrib::StringHash32(GRaceDatabase::Get().GetDDayEndRace()));
                dday_flow_completed = GRaceDatabase::Get().CheckRaceScoreFlags(parms->GetEventHash(), GRaceDatabase::kCompleted_ContextCareer);
            } else {
                dday_flow_completed = true;
            }

            if (dday_flow_completed) {
                RaceStarter::StartCareerFreeRoam();
            } else {
                const char *firstDDayRace;
                if (!SkipDDayRaces) {
                    firstDDayRace = GRaceDatabase::Get().GetNextDDayRace();
                } else {
                    firstDDayRace = GRaceDatabase::Get().GetDDayEndRace();
                }
                GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(Attrib::StringHash32(firstDDayRace));
                GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
                GRaceDatabase::Get().SetStartupRace(race, kRaceContext_Career);
                GRaceDatabase::Get().FreeCustomRace(race);
                RaceStarter::StartCareerFreeRoam();
            }
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
            return;
        }
        case 0x34DC1BCF:
            return;
    }
}

void uiCareerCrib::Setup() {
    CResumeFreeRoam *resumeFreeRoam = new CResumeFreeRoam(0x12BB5EA2, 0x1BD185C, 0);
    resumeFreeRoam->SetReactImmediately(true);
    AddOption(resumeFreeRoam);

    AddOption(new CTop15(0x2C14AC23, 0x80B9FF9B, 0));
    AddOption(new CCarSelect(0xC6A1A6E0, 0xD5F627, 0));

    if (FEDatabase->GetCareerSettings()->HasRapSheet()) {
        AddOption(new CRapSheet(0x2FD8B206, 0xAC22F27E, 0));
    }

    if (IsMemcardEnabled) {
        CSave *save = new CSave(0x228B7E32, 0x1C8ACE, 0);
        save->SetReactImmediately(true);
        AddOption(save);
    }

    int lastButton = FEngGetLastButton(GetPackageName());
    SetInitialOption(lastButton);

    FEngSetLanguageHash(GetPackageName(), 0x3C458C1, 0xE596C4A3);
    FEngSetLanguageHash(GetPackageName(), 0xB5C74226, 0xE596C4A3);

    const char *szPercentUnit = "%";
    eLanguages currLang = static_cast<eLanguages>(GetCurrentLanguage());
    if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN ||
        currLang == eLANGUAGE_SWEDISH) {
        szPercentUnit = "%%";
    }

    unsigned int hash = FEHashUpper("ICON_GROUP");
    unsigned int hash2 = FEHashUpper("CAREER_CRIB");
    FEngSetScript(GetPackageName(), hash, hash2, true);

    GameCompletionStats stats = FEDatabase->GetGameCompletionStats();
    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("GAME_COMPLETE")), "%d%s", stats.m_nCareer, szPercentUnit);

    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("TOTAL_BOUNTY")), "%d", FEDatabase->GetPlayerCarStable(0)->GetTotalBounty());

    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("TOTAL_CASH")), "%d", FEDatabase->GetCareerSettings()->GetCash());

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}

inline void CResumeFreeRoam::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        cFrontendDatabase *db = FEDatabase;
        signed char port = FEngMapJoyParamToJoyport(param1);
        db->SetPlayersJoystickPort(0, port);
        const char *blurb = GetLocalizedString(0xEB694C0C);
        DialogInterface::ShowTwoButtons(pkg_name, "", dialog_alert, 0x70E01038, 0x417B25E4, 0xD05FC3A3, 0x34DC1BCF, 0x34DC1BCF, first_dialog_button2,
                                        blurb);
    }
}

inline void CCarSelect::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        cFEng::Get()->QueuePackageSwitch("IG_CarLot.fng", 0, 0, false);
    }
}

inline void CRapSheet::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        FEDatabase->SetGameMode(eFE_GAME_MODE_RAP_SHEET);
        cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
    }
}

inline void CTop15::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        cFEng::Get()->QueuePackageSwitch("WorldMap_Main.fng", 0, 0, false);
    }
}

inline void CSave::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data == 0x0C407210) {
        MemcardEnter(pkg_name, pkg_name, 0x2251, 0, 0, 0, 0);
    }
}
