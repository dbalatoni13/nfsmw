#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRModeSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"

extern const char *gOnlineMainMenu;

class MSOption : public IconOption {
  public:
    MSOption(uint32 tex_hash, uint32 name_hash, GRace::Type race_type) : IconOption(tex_hash, name_hash, 0), raceType(race_type) {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0xc407210) {
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
    uint32 hash = 0x1f203817;
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        hash = 0x6703b807;
    } else {
        bool isSplitQR = false;
        if (FEDatabase->IsQuickRaceMode()) {
            isSplitQR = FEDatabase->iNumPlayers == 2;
        }
        if (isSplitQR) {
            hash = 0x43c825ed;
        }
    }
    FEngSetLanguageHash(GetPackageName(), 0xb71b576d, hash);
}

void UIQRModeSelect::Setup() {
    if (GetMikeMannBuild()) {
        MSOption *opt;
        opt = new MSOption(0xe9638d3e, 0x34fa2c1, GRace::kRaceType_Circuit);
        AddOption(opt);
        if (GetMikeMannBuild() == 1) {
            opt = new MSOption(0x2521e5eb, 0xb94fd70e, GRace::kRaceType_P2P);
            AddOption(opt);
            opt = new MSOption(0xaaab31e9, 0x6f547e4c, GRace::kRaceType_Drag);
            AddOption(opt);
            opt = new MSOption(0x1a091045, 0xa15e4505, GRace::kRaceType_Tollbooth);
            AddOption(opt);
        }
    } else {
        MSOption *opt;
        opt = new MSOption(0xe9638d3e, 0x34fa2c1, GRace::kRaceType_Circuit);
        AddOption(opt);
        opt = new MSOption(0x2521e5eb, 0xb94fd70e, GRace::kRaceType_P2P);
        AddOption(opt);
        opt = new MSOption(0xaaab31e9, 0x6f547e4c, GRace::kRaceType_Drag);
        AddOption(opt);
        if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
            bool isSplitQR = false;
            if (FEDatabase->IsQuickRaceMode()) {
                isSplitQR = FEDatabase->iNumPlayers == 2;
            }
            if (!isSplitQR) {
                opt = new MSOption(0x3a015595, 0x4930f5fc, GRace::kRaceType_Knockout);
                AddOption(opt);
            }
            opt = new MSOption(0x66c9a7b6, 0xee1edc76, GRace::kRaceType_SpeedTrap);
            AddOption(opt);
        }
    }
    int lastBtn = FEngGetLastButton(GetPackageName());
    SetInitialOption(lastBtn);
    cFEng::Get()->QueuePackageMessage(0x21828323, GetPackageName(), nullptr);
}

void UIQRModeSelect::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0x911ab364:
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            }
            break;
        case 0xe1fde1d1:
            switch (PrevButtonMessage) {
                case 0xc407210:
                    cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
                    break;
                case 0x911ab364:
                    FEDatabase->ClearGameMode(static_cast<eFEGameModes>(0x400));
                    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                        cFEng::Get()->QueuePackageSwitch(gOnlineMainMenu, 0, 0, false);
                    } else {
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                    }
                    break;
            }
            break;
    }
}
