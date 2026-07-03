#include "uiMain.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

const char *gOnlineMainMenu = "OL_MAIN.fng";

extern int UnlockAllThings;

class MainQuickRace : public IconOption {
  public:
    MainQuickRace(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainQuickRace() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data != 0x0C407210)
            return;
        FEDatabase->SetGameMode(eFE_GAME_MODE_QUICK_RACE);
    }
};

class MainCustomize : public IconOption {
  public:
    MainCustomize(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainCustomize() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data != 0x0C407210)
            return;
        FEDatabase->SetGameMode(eFE_GAME_MODE_CUSTOMIZE);
    };
};

class MainProfileManager : public IconOption {
  public:
    MainProfileManager(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainProfileManager() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data != 0x0C407210)
            return;
        FEDatabase->SetGameMode(eFE_GAME_MODE_PROFILE_MANAGER);
    };
};

class MainOptions : public IconOption {
  public:
    MainOptions(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainOptions() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data != 0x0C407210)
            return;
        FEDatabase->SetGameMode(eFE_GAME_MODE_OPTIONS);
    };
};

class MainCareer : public IconOption {
  public:
    MainCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data != 0x0C407210)
            return;
        if (IsMemcardEnabled) {
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER_MANAGER);
        } else {
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
        }
    };
};

class Challenge : public IconOption {
  public:
    Challenge(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~Challenge() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            if (FEDatabase->bProfileLoaded || !IsMemcardEnabled) {
                FEDatabase->SetGameMode(eFE_GAME_MODE_CHALLENGE);
                SetReactImmediately(false);
                cFEng::Get()->QueuePackageMessage(0x0C407210, pkg_name, obj);
            } else {
                MemcardEnter("MainMenu.fng", "ChallengeSeries.fng", 0x10063, nullptr, nullptr, 0, 0);
            }
        }
    };
};

UIMain::UIMain(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    m_bStatsShowing = false;
    Setup();
}

void UIMain::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case 0x1265ece9:
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            break;
        case 0x35f8620b:
            if (!MemoryCard::GetInstance()->IsAutoLoadDone()) {
                MemoryCard::GetInstance()->SetAutoLoadDone(true);
                MemcardEnter(nullptr, nullptr, 0xF1, nullptr, nullptr, 0, 0);
            }
            break;
        case 0xe1fde1d1:
            if (PrevButtonMessage != 0x0c407210) {
                break;
            }
            if (FEDatabase->IsCareerMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else if (FEDatabase->IsCareerManagerMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else if (FEDatabase->IsQuickRaceMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else if (FEDatabase->IsOptionsMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else if (FEDatabase->IsProfileManagerMode()) {
                cFEng::Get()->QueuePackageSwitch("MC_ProfileManager.fng", 0, 0, false);
            } else if (FEDatabase->IsChallengeMode()) {
                cFEng::Get()->QueuePackageSwitch("ChallengeSeries.fng", 0, 0, false);
            } else if (FEDatabase->IsCustomizeMode()) {
                cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
            }
            break;
        case 0xc519bfc4:
            if (FEDatabase->bProfileLoaded) {
                cFEng::Get()->QueuePackageMessage(FEHashUpper(m_bStatsShowing ? "GAMESATS_LEAVE" : "GAMESTATS_APPEAR"), GetPackageName(), nullptr);
                m_bStatsShowing = !m_bStatsShowing;
            }
            break;
        case 0x7e998e5e:
            UpdateProfileData();
            break;
        case 0x9120409e:
            break;
    }
}

void UIMain::Setup() {
    const uint32 FEObj_TITLEGROUP = 0xb71b576d;

    FEDatabase->ResetGameMode();
    FEDatabase->SetPlayersJoystickPort(0, -1);

    if (GetMikeMannBuild()) {
        AddOption(new ("Challenge", 0) Challenge(0x9a962438, 0xcc8cb746, 0));
        AddOption(new ("MainQuickRace", 0) MainQuickRace(0x4e6fbb02, 0x54020a7a, 0));
        AddOption(new ("MainCustomize", 0) MainCustomize(0xb0c46023, 0x1afd5be6, 0));
        AddOption(new ("MainOptions", 0) MainOptions(0x3058fe37, 0x19a8c0af, 0));
        UnlockAllThings = 1;
    } else {
        AddOption(new ("MainCareer", 0) MainCareer(0x3704f3d, 0x5815a2b5, 0));
        AddOption(new ("Challenge", 0) Challenge(0x9a962438, 0xcc8cb746, 0));
        AddOption(new ("MainQuickRace", 0) MainQuickRace(0x4e6fbb02, 0x54020a7a, 0));
        AddOption(new ("MainCustomize", 0) MainCustomize(0xb0c46023, 0x1afd5be6, 0));
        if (IsMemcardEnabled) {
            AddOption(new ("MainProfileManager", 0) MainProfileManager(0x6b303856, 0xbcb18f38, 0));
        }
        AddOption(new ("MainOptions", 0) MainOptions(0x3058fe37, 0x19a8c0af, 0));
    }

    FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0xb24aae58);
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    RefreshHeader();
    UpdateProfileData();
}

void UIMain::UpdateProfileData() {
    if (FEDatabase->bProfileLoaded) {
        GameCompletionStats stats = FEDatabase->GetGameCompletionStats();
        const char *szPercentUnit = "%";
        eLanguages currLang = GetCurrentLanguage();
        const uint32 FEObj_PLAYERNAMEGROUP = 0xb514e2d8;

        if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN ||
            currLang == eLANGUAGE_SWEDISH) {
            szPercentUnit = " %";
        }

        FEPrintf(GetPackageName(), FEHashUpper("GAME_COMPLETED_DATA"), "%d%s", stats.m_nOverall, szPercentUnit);
        FEPrintf(GetPackageName(), FEHashUpper("CAREER_COMPLETED_DATA"), "%d%s", stats.m_nCareer, szPercentUnit);
        FEngSetInvisible(GetPackageName(), FEHashUpper("RAP_SHEET_ITEMS_DATA_1"));
        FEPrintf(GetPackageName(), FEHashUpper("RAP_SHEET_ITEMS_DATA_2"), "%d%s", stats.m_nRapSheetRankings, szPercentUnit);
        FEPrintf(GetPackageName(), FEHashUpper("CHALLENGE_SERIES_DATA_1"), "%d/%d", stats.m_nCompletedChallengeRaces, stats.m_nTotalChallengeRaces);
        FEPrintf(GetPackageName(), FEHashUpper("CHALLENGE_SERIES_DATA_2"), "%d%s", stats.m_nChallenge, szPercentUnit);
        FEPrintf(GetPackageName(), FEObj_PLAYERNAMEGROUP, "%s", FEDatabase->GetMultiplayerProfile(0)->GetProfileName());
        FEngSetVisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
        FEngSetVisible(GetPackageName(), FEHashUpper("GAME STATS GROUP"));
    } else {
        FEngSetInvisible(GetPackageName(), FEHashUpper("NAME_GROUP"));
        FEngSetInvisible(GetPackageName(), FEHashUpper("GAME STATS GROUP"));
    }
}
