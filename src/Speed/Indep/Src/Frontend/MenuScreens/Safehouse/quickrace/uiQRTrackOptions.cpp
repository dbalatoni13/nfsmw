#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackOptions.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

#include <types.h>

class NumOpponents : public FEToggleWidget {
  public:
    NumOpponents(bool enabled) : FEToggleWidget(enabled) {}
    ~NumOpponents() override {}
    void Act(const char *parent_pkg, uint32 data) override {
        int players = FEDatabase->iNumPlayers;
        int opponents = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumOpponents;
        if (data == __PAD_LEFT__) {
            opponents = opponents - 1;
            if (opponents < 1) {
                opponents = 4 - players;
            }
        } else if (data == __PAD_RIGHT__) {
            opponents = opponents + 1;
            if (opponents > 4 - players) {
                opponents = 1;
            }
        }
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumOpponents = opponents;
        if (FEDatabase->RaceMode == GRace::kRaceType_Knockout) {
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumLaps = opponents;
            cFEng::Get()->QueueGameMessage(0x92b703b5, parent_pkg, 0xff);
        }
        Update(data);
        Draw();
    }
    void Draw() override {
        FEngSetLanguageHash(GetTitleObject(), 0x3384a679);
        FEPrintf(GetDataObject(), "%d", FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumOpponents);
    }
};

class AISkill : public FEToggleWidget {
  public:
    AISkill(bool enabled) : FEToggleWidget(enabled) {}
    ~AISkill() override {}
    void Act(const char *parent_pkg, uint32 data) override {
        int skill = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->AISkill;
        if (data == __PAD_LEFT__) {
            skill = skill - 1;
            if (skill < 0) {
                skill = 2;
            }
        } else if (data == __PAD_RIGHT__) {
            skill = skill + 1;
            if (skill > 2) {
                skill = 0;
            }
        }
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->AISkill = skill;
        Update(data);
        Draw();
    }
    void Draw() override {
        uint8 skill = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->AISkill;
        uint32 hash = 0;
        switch (skill) {
            case 0:
                hash = 0x61973e01;
                break;
            case 1:
                hash = 0x3747f6d0;
                break;
            case 2:
                hash = 0x6198e2ee;
                break;
        }
        FEngSetLanguageHash(GetTitleObject(), 0x4d156786);
        FEngSetLanguageHash(GetDataObject(), hash);
    }
};

class CatchUp : public FEToggleWidget {
  public:
    CatchUp(bool enabled) : FEToggleWidget(enabled) {}
    ~CatchUp() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->CatchUp =
                !FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->CatchUp;
        }
        Update(data);
        Draw();
    }
    void Draw() override {
        if (FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->CatchUp) {
            FEngSetLanguageHash(GetDataObject(), 0x417b2604);
        } else {
            FEngSetLanguageHash(GetDataObject(), 0x70dfe5c2);
        }
        FEngSetLanguageHash(GetTitleObject(), 0x8b8e913a);
    }
};

class TrafficLevel : public FEToggleWidget {
  public:
    TrafficLevel(bool enabled) : FEToggleWidget(enabled) {}
    ~TrafficLevel() override {}
    void Act(const char *parent_pkg, uint32 data) override {
        int level = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrafficDensity;
        if (data == __PAD_LEFT__) {
            level = level - 1;
            if (level < 0) {
                level = 3;
            }
        } else if (data == __PAD_RIGHT__) {
            level = level + 1;
            if (level > 3) {
                level = 0;
            }
        }
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrafficDensity = level;
        Update(data);
        Draw();
    }
    void Draw() override {
        int hash = 0;
        switch (FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrafficDensity) {
            case 0:
                hash = 0x8cdc3937;
                break;
            case 1:
                hash = 0x73c615a3;
                break;
            case 2:
                hash = 0xa2cca838;
                break;
            case 3:
                hash = 0x61d1c5a5;
                break;
        }
        FEngSetLanguageHash(GetDataObject(), hash);
        FEngSetLanguageHash(GetTitleObject(), 0xeb9dfc09);
    }
};

class NumLaps : public FEToggleWidget {
  public:
    NumLaps(bool enabled) : FEToggleWidget(enabled) {}
    ~NumLaps() override {}
    void Act(const char *parent_pkg, uint32 data) override {
        int laps = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumLaps;
        int max = 8;
        if (data == __PAD_LEFT__) {
            laps = laps - 1;
            if (laps < 1) {
                laps = max;
            }
        } else if (data == __PAD_RIGHT__) {
            laps = laps + 1;
            if (laps > max) {
                laps = 1;
            }
        }
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumLaps = laps;
        Update(data);
        Draw();
    }
    void Draw() override {
        FEPrintf(GetDataObject(), "%d", FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->NumLaps);
        FEngSetLanguageHash(GetTitleObject(), 0x48494e83);
    }
};

class TrackDirection : public FEToggleWidget {
  public:
    TrackDirection(bool enabled) : FEToggleWidget(enabled) {}
    ~TrackDirection() override {}
    void Act(const char *parent_pkg, uint32 data) override {
        if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrackDirection =
                static_cast<uint8>(FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrackDirection == 0);
        }
        Update(data);
        Draw();
    }
    void Draw() override {
        uint8 TrackDirection = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->TrackDirection;
        if (TrackDirection == 0) {
            FEngSetLanguageHash(GetDataObject(), 0xde6eff34);
        } else {
            FEngSetLanguageHash(GetDataObject(), 0xa1cd823e);
        }
        FEngSetLanguageHash(GetTitleObject(), 0xa88ffeb4);
    }
};

UIQRTrackOptions::UIQRTrackOptions(ScreenConstructorData *sd) : UIWidgetMenu(sd), m_code(0), msgHandle(0), m_boDisconnectPercAvail(false) {
    race = GRaceDatabase::Get().GetRaceFromHash(FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode)->EventHash);
    iMaxWidgetsOnScreen = 9;
    Setup();
}

void UIQRTrackOptions::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case __PAD_ACCEPT__:
            if (!(FEDatabase->IsOnlineMode()) && !(FEDatabase->IsLANMode())) {
                GRaceCustom *custom_race = GRaceDatabase::Get().AllocCustomRace(race);
                custom_race->SetCopsEnabled(false);
                FEDatabase->FillCustomRace(custom_race, FEDatabase->GetQuickRaceSettings(race->GetRaceType()));
                if (FEDatabase->IsSplitScreenMode()) {
                    custom_race->SetNumOpponents(1);
                }
                GRaceDatabase::Get().SetStartupRace(custom_race, GRace::kRaceContext_QuickRace);
                GRaceDatabase::Get().FreeCustomRace(custom_race);
            }

            if (FEDatabase->IsSplitScreenMode()) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
            }

            break;
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
            break;
        case __PAD_BUTTON5__:
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                            dialog_message_no, dialog_message_no, first_dialog_button2, GetLocalizedString(0x8aef5ae8));
            break;
        case dialog_message_yes:
            FEDatabase->DefaultRaceSettings();

            for (int i = 0; i < Options.CountElements(); i++) {
                Options.GetNode(i)->Draw();
            }

            break;
        case dialog_message_no:
        case FEMSG_ERROR_STATE: // TODO: not real, just need a value in range to solve the switch
            break;
    }
}

void UIQRTrackOptions::Setup() {
    GRace::Type raceMode = FEDatabase->RaceMode;

    switch (raceMode) {
        case GRace::kRaceType_Circuit:
            SetupCircuit();
            break;
        case GRace::kRaceType_P2P:
            SetupSprint();
            break;
        case GRace::kRaceType_Drag:
            SetupDrag();
            break;
        case GRace::kRaceType_Knockout:
            SetupKnockout();
            break;
        case GRace::kRaceType_SpeedTrap:
            SetupSpeedTrap();
            break;
        case GRace::kRaceType_Tollbooth:
            SetupTollbooth();
            break;
        default:
            SetupCircuit();
            break;
    }

    SetInitialOption(0);

    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;
    if ((FEDatabase->IsOnlineMode()) || (FEDatabase->IsLANMode())) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x7dadee33);
    } else {
        unsigned int hash;
        switch (race->GetRaceType()) {
            case GRace::kRaceType_Circuit:
                hash = 0xa6ed015d;
                break;
            case GRace::kRaceType_Drag:
                hash = 0xec86e188;
                break;
            case GRace::kRaceType_Knockout:
                hash = 0xcc959b8;
                break;
            case GRace::kRaceType_P2P:
                hash = 0xb80bfc8a;
                break;
            case GRace::kRaceType_Tollbooth:
                hash = 0x141edfe1;
                break;
            case GRace::kRaceType_SpeedTrap:
                hash = 0xf2745852;
                break;
            default:
                hash = 0;
                break;
        }
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, hash);
    }
}

void UIQRTrackOptions::BoilerPlateOnline(const bool &boAddLaps) {}

void UIQRTrackOptions::SetupCircuit() {
    if (race->GetCanBeReversed()) {
        AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
    }

    AddToggleOption(new ("NumLaps", 0) NumLaps(true), true);
    if (!FEDatabase->IsSplitScreenMode()) {
        AddToggleOption(new ("TrafficLevel", 0) TrafficLevel(true), true);
    }

    if (!FEDatabase->IsSplitScreenMode()) {
        AddToggleOption(new ("NumOpponents", 0) NumOpponents(true), true);
        AddToggleOption(new ("AISkill", 0) AISkill(true), true);
        AddToggleOption(new ("CatchUp", 0) CatchUp(true), true);
    }
}

void UIQRTrackOptions::SetupSprint() {
    if (race->GetCanBeReversed()) {
        AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
    }
    if (!FEDatabase->IsSplitScreenMode()) {
        AddToggleOption(new ("TrafficLevel", 0) TrafficLevel(true), true);
    }
    if (!FEDatabase->IsSplitScreenMode()) {
        AddToggleOption(new ("NumOpponents", 0) NumOpponents(true), true);
        AddToggleOption(new ("AISkill", 0) AISkill(true), true);
        AddToggleOption(new ("CatchUp", 0) CatchUp(true), true);
    }
}

void UIQRTrackOptions::SetupDrag() {
    if (race->GetCanBeReversed()) {
        AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
    }
    if (!FEDatabase->IsSplitScreenMode()) {
        AddToggleOption(new ("NumOpponents", 0) NumOpponents(true), true);
        AddToggleOption(new ("AISkill", 0) AISkill(true), true);
        AddToggleOption(new ("CatchUp", 0) CatchUp(true), true);
    }
}

void UIQRTrackOptions::SetupKnockout() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        BoilerPlateOnline(false);
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
        AddToggleOption(new ("NumLaps", 0) NumLaps(true), true);

    } else {
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
        AddToggleOption(new ("NumOpponents", 0) NumOpponents(true), true);

        uint32 id = AddToggleOption(new ("NumLaps", 0) NumLaps(true), true);

        static_cast<FEToggleWidget *>(GetWidget(id))->SetDisableScript(FEHashUpper("GREY"));
        GetWidget(id)->Disable();

        if (!FEDatabase->IsSplitScreenMode()) {
            AddToggleOption(new ("TrafficLevel", 0) TrafficLevel(true), true);
        }
        AddToggleOption(new ("AISkill", 0) AISkill(true), true);
        AddToggleOption(new ("CatchUp", 0) CatchUp(true), true);
    }
}

void UIQRTrackOptions::SetupSpeedTrap() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        BoilerPlateOnline(false);
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
        if (!FEDatabase->IsSplitScreenMode()) {
            AddToggleOption(new ("TrafficLevel", 0) TrafficLevel(true), true);
        }
        if (!FEDatabase->IsSplitScreenMode()) {
            AddToggleOption(new ("NumOpponents", 0) NumOpponents(true), true);
            AddToggleOption(new ("AISkill", 0) AISkill(true), true);
            AddToggleOption(new ("CatchUp", 0) CatchUp(true), true);
        }
    }
}

void UIQRTrackOptions::SetupTollbooth() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        BoilerPlateOnline(false);
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            AddToggleOption(new ("TrackDirection", 0) TrackDirection(true), true);
        }
        if (!FEDatabase->IsSplitScreenMode()) {
            AddToggleOption(new ("TrafficLevel", 0) TrafficLevel(true), true);
        }
    }
}
