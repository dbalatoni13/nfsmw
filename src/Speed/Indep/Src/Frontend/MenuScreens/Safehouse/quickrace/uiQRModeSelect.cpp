#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRModeSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"

class MSOption : public IconOption {
  public:
    MSOption(uint32 tex_hash, uint32 name_hash, GRace::Type race_type) : IconOption(tex_hash, name_hash, 0), raceType(race_type) {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == __BUTTON_PRESSED__) {
            FEDatabase->RaceMode = raceType;
        }
    };

    GRace::Type raceType; // offset 0x5C, size 0x4
};

UIQRModeSelect::UIQRModeSelect(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    Setup();
    RefreshHeader();
}

void UIQRModeSelect::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    u32 hash = 0x1f203817;
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        hash = 0x6703b807;
    } else {
        if (FEDatabase->IsSplitScreenMode()) {
            hash = 0x43c825ed;
        }
    }
    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, hash);
}

void UIQRModeSelect::Setup() {
    if (GetMikeMannBuild()) {
        AddOption(new ("MSOption", 0) MSOption(0xe9638d3e, 0x34fa2c1, GRace::kRaceType_Circuit));
        if (GetMikeMannBuild() == 1) {
            AddOption(new ("MSOption", 0) MSOption(0x2521e5eb, 0xb94fd70e, GRace::kRaceType_P2P));
            AddOption(new ("MSOption", 0) MSOption(0xaaab31e9, 0x6f547e4c, GRace::kRaceType_Drag));
            AddOption(new ("MSOption", 0) MSOption(0x1a091045, 0xa15e4505, GRace::kRaceType_Tollbooth));
        }
    } else {
        AddOption(new ("MSOption", 0) MSOption(0xe9638d3e, 0x34fa2c1, GRace::kRaceType_Circuit));
        AddOption(new ("MSOption", 0) MSOption(0x2521e5eb, 0xb94fd70e, GRace::kRaceType_P2P));
        AddOption(new ("MSOption", 0) MSOption(0xaaab31e9, 0x6f547e4c, GRace::kRaceType_Drag));
        if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
            if (!FEDatabase->IsSplitScreenMode()) {
                AddOption(new ("MSOption", 0) MSOption(0x3a015595, 0x4930f5fc, GRace::kRaceType_Knockout));
            }
            AddOption(new ("MSOption", 0) MSOption(0x66c9a7b6, 0xee1edc76, GRace::kRaceType_SpeedTrap));
        }
    }
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    const u32 FEObj_ModeSelect = 0x21828323;
    cFEng::Get()->QueuePackageMessage(FEObj_ModeSelect, GetPackageName(), nullptr);
}

void UIQRModeSelect::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case __PAD_BACK__:
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                const u32 FEObj_leavescreen = 0x587c018b;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        case FEHASH_EXITCOMPLETE:
            switch (PrevButtonMessage) {
                case __BUTTON_PRESSED__:
                    cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
                    break;
                case __PAD_BACK__:
                    FEDatabase->ClearGameMode(eFE_GAME_MODE_MODE_SELECT);
                    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                        extern const char *gOnlineMainMenu;
                        cFEng::Get()->QueuePackageSwitch(gOnlineMainMenu, 0, 0, false);
                    } else {
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                    }
                    break;
            }
            break;
    }
}
