#include "uiCareerMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

// GarageMainScreen already defined in uiMain.cpp (earlier in TU)

unsigned char FEngGetLastButton(const char *pkg_name);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);
// [fe3] const char *GetLocalizedString(unsigned int hash);
int FEngMapJoyParamToJoyport(int feng_param);
void MemcardEnter(const char *from, const char *to, unsigned int op, void (*pTermFunc)(void *), void *pTermFuncParam, unsigned int msgSuccess,
                  unsigned int msgFailed);

// [fe3] RaceStarter ya esta declarada en uiRepSheetRivalFlow.cpp (antes en la TU)

extern unsigned int iCurrentViewBin;

struct CResumeFreeRoam : public IconOption {
    CResumeFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CResumeFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            cFrontendDatabase *db = FEDatabase;
            signed char port = FEngMapJoyParamToJoyport(param1);
            db->SetPlayersJoystickPort(0, port);
            const char *blurb = GetLocalizedString(0xEB694C0C);
            DialogInterface::ShowTwoButtons(pkg_name, "", static_cast<eDialogTitle>(1), LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xD05FC3A3, 0x34DC1BCF, 0x34DC1BCF,
                                            static_cast<eDialogFirstButtons>(1), blurb);
        }
    }

};

struct CCarSelect : public IconOption {
    CCarSelect(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CCarSelect() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
        }
    }

};

struct CRapSheet : public IconOption {
    CRapSheet(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CRapSheet() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            FEDatabase->SetGameMode(eFE_GAME_MODE_RAP_SHEET);
            cFEng::Get()->QueuePackageSwitch("RapSheetLogin.fng", 0, 0, false);
        }
    }

};

struct CTop15 : public IconOption {
    CTop15(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CTop15() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            cFEng::Get()->QueuePackageSwitch("SafehouseReputationOverview.fng", 0, 0, false);
        }
    }

};

struct CSave : public IconOption {
    CSave(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~CSave() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x2251, 0, 0, 0, 0);
        }
    }

};

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
            const char *lastDDayRace = GRaceDatabase::Get().GetDDayEndRace();
            bool dday_flow_completed = SkipDDayRaces ||
                                       GRaceDatabase::Get().IsCareerRaceComplete(
                                           GRaceDatabase::Get().GetRaceFromName(lastDDayRace)->GetEventHash());

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
        case 0x34DC1BCF:
            return;
    }
}

void uiCareerCrib::Setup() {
    u32 hash;
    const u32 FEObj_TITLE = 0x3C458C1;
    const u32 FEObj_TITLESHADOW = 0xB5C74226;
    const char *szPercentUnit;
    eLanguages currLang;

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

    FEngSetLanguageHash(GetPackageName(), FEObj_TITLE, 0xE596C4A3);
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLESHADOW, 0xE596C4A3);

    szPercentUnit = "%";
    currLang = static_cast<eLanguages>(GetCurrentLanguage());
    if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN ||
        currLang == eLANGUAGE_SWEDISH) {
        szPercentUnit = " %";
    }

    FEngSetScript(GetPackageName(), FEHashUpper("PLAYER_MASTER"), FEHashUpper("SHOW"), true);

    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("CAREER_DATA")), "%d%s",
             FEDatabase->GetGameCompletionStats().m_nCareer, szPercentUnit);

    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("BOUNTY_DATA")), "%$d",
             FEDatabase->GetPlayerCarStable(0)->GetTotalBounty());

    FEPrintf(GetPackageName(), static_cast<int>(FEHashUpper("CASH_DATA")), "%$d", FEDatabase->GetCareerSettings()->GetCash());

    RefreshHeader();
    FEDatabase->RefreshCurrentRide();
}
