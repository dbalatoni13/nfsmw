#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackOptions.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRMainMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

#include <types.h>
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

class NumOpponents : public FEToggleWidget {
  public:
    NumOpponents(bool enabled) : FEToggleWidget(enabled) {}
    ~NumOpponents() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        int numPlayers = FEDatabase->iNumPlayers;
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        int val = settings->NumOpponents;
        if (data == 0x9120409e) {
            val = val - 1;
            if (val < 1) {
                val = 4 - numPlayers;
            }
        } else if (data == 0xb5971bf1) {
            val = val + 1;
            if (val > 4 - numPlayers) {
                val = 1;
            }
        }
        settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->NumOpponents = static_cast<uint8>(val);
        if (FEDatabase->RaceMode == static_cast<GRace::Type>(3)) {
            settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            settings->NumLaps = static_cast<uint8>(val);
            cFEng::Get()->QueueGameMessage(0x92b703b5, parent_pkg, 0xff);
        }
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    };
    void Draw() override {
        FEngSetLanguageHash(GetTitleObject(), 0x3384a679);
        FEString *data = GetDataObject();
        const char *fmt = "%d";
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        FEPrintf(data, fmt, settings->NumOpponents);
    };
};

class AISkill : public FEToggleWidget {
  public:
    AISkill(bool enabled) : FEToggleWidget(enabled) {}
    ~AISkill() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        int val = settings->AISkill;
        if (data == 0x9120409e) {
            val = val - 1;
            if (val < 0) {
                val = 2;
            }
        } else if (data == 0xb5971bf1) {
            val = val + 1;
            if (val > 2) {
                val = 0;
            }
        }
        settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->AISkill = static_cast<uint8>(val);
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    }
    void Draw() override {
        unsigned int hash = 0;
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        unsigned char skill = settings->AISkill;
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
    };
};

class CatchUp : public FEToggleWidget {
  public:
    CatchUp(bool enabled) : FEToggleWidget(enabled) {}
    ~CatchUp() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        if (data == 0x9120409e || data == 0xb5971bf1) {
            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            RaceSettings *settings2 = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            settings->CatchUp = !settings2->CatchUp;
        }
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    };
    void Draw() override {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        if (settings->CatchUp) {
            FEngSetLanguageHash(GetDataObject(), LANGUAGE_COMMON_ON);
        } else {
            FEngSetLanguageHash(GetDataObject(), LANGUAGE_COMMON_OFF);
        }
        FEngSetLanguageHash(GetTitleObject(), 0x8b8e913a);
    };
};

class TrafficLevel : public FEToggleWidget {
  public:
    TrafficLevel(bool enabled) : FEToggleWidget(enabled) {}
    ~TrafficLevel() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        int val = settings->TrafficDensity;
        if (data == 0x9120409e) {
            val = val - 1;
            if (val < 0) {
                val = 3;
            }
        } else if (data == 0xb5971bf1) {
            val = val + 1;
            if (val > 3) {
                val = 0;
            }
        }
        settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->TrafficDensity = static_cast<uint8>(val);
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    };
    void Draw() override {
        unsigned int hash = 0;
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        unsigned char level = settings->TrafficDensity;
        switch (level) {
            case 0:
                hash = LANGUAGE_COMMON_NONE;
                break;
            case 1:
                hash = LANGUAGE_COMMON_MINIMUM;
                break;
            case 2:
                hash = 0xa2cca838;
                break;
            case 3:
                hash = LANGUAGE_COMMON_MAXIMUM;
                break;
        }
        FEngSetLanguageHash(GetDataObject(), hash);
        FEngSetLanguageHash(GetTitleObject(), 0xeb9dfc09);
    };
};

class NumLaps : public FEToggleWidget {
  public:
    NumLaps(bool enabled) : FEToggleWidget(enabled) {}
    ~NumLaps() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        int val = settings->NumLaps;
        if (data == 0x9120409e) {
            val = val - 1;
            if (val < 1) {
                val = 8;
            }
        } else if (data == 0xb5971bf1) {
            val = val + 1;
            if (val > 8) {
                val = 1;
            }
        }
        settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->NumLaps = static_cast<uint8>(val);
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    };
    void Draw() override {
        FEString *data = GetDataObject();
        const char *fmt = "%d";
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        FEPrintf(data, fmt, settings->NumLaps);
        FEngSetLanguageHash(GetTitleObject(), 0x48494e83);
    };
};

class TrackDirection : public FEToggleWidget {
  public:
    TrackDirection(bool enabled) : FEToggleWidget(enabled) {}
    ~TrackDirection() override {};
    void Act(const char *parent_pkg, uint32 data) override {
        if (data == 0x9120409e || data == 0xb5971bf1) {
            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            RaceSettings *settings2 = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            settings->TrackDirection = (settings2->TrackDirection == 0);
        }
        bMovedLastUpdate = true;
        BlinkArrows(data);
        Draw();
    };
    void Draw() override {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        if (settings->TrackDirection == 0) {
            FEngSetLanguageHash(GetDataObject(), FEHASH_FORWARD);
        } else {
            FEngSetLanguageHash(GetDataObject(), 0xa1cd823e);
        }
        FEngSetLanguageHash(GetTitleObject(), 0xa88ffeb4);
    };
};

UIQRTrackOptions::UIQRTrackOptions(ScreenConstructorData *sd) : UIWidgetMenu(sd), m_code(0), msgHandle(0), m_boDisconnectPercAvail(false) {
    race = GRaceDatabase::Get().GetRaceFromHash(FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode)->EventHash);
    iMaxWidgetsOnScreen = 9;
    Setup();
}

void UIQRTrackOptions::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0x406415e3:
            if (!(FEDatabase->IsOnlineMode()) && !(FEDatabase->IsLANMode())) {
                GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(race);
                custom->SetCopsEnabled(false);
                FEDatabase->FillCustomRace(custom, FEDatabase->GetQuickRaceSettings(race->GetRaceType()));
                bool isSplitScreen = FEDatabase->IsSplitScreenMode();
                if (isSplitScreen) {
                    custom->SetNumOpponents(1);
                }
                GRaceDatabase::Get().SetStartupRace(custom, GRace::kRaceContext_QuickRace);
                GRaceDatabase::Get().FreeCustomRace(custom);
            }
            {
                bool isSplitScreen = FEDatabase->IsSplitScreenMode();
                if (isSplitScreen) {
                    cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
                }
            }
            break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
            break;
        case 0xc519bfc4:
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
                                            first_dialog_button2, GetLocalizedString(0x8aef5ae8));
            break;
        case 0xd05fc3a3:
            FEDatabase->DefaultRaceSettings();
            for (int i = 0; i < Options.CountElements(); i++) {
                FEWidget *w = static_cast<FEWidget *>(Options.GetNode(i));
                w->Draw();
            }
            break;
        case 0x34dc1bcf:
            break;
        case 0xe1fde1d1:
            break;
    }
}

void UIQRTrackOptions::Setup() {
    GRace::Type raceType = FEDatabase->RaceMode;
    switch (raceType) {
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
    if ((FEDatabase->IsOnlineMode()) || (FEDatabase->IsLANMode())) {
        FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x7dadee33);
        return;
    }
    unsigned int titleHash;
    GRace::Type type = race->GetRaceType();
    switch (type) {
        case GRace::kRaceType_Circuit:
            titleHash = 0xa6ed015d;
            break;
        case GRace::kRaceType_Drag:
            titleHash = 0xec86e188;
            break;
        case GRace::kRaceType_Knockout:
            titleHash = 0xcc959b8;
            break;
        case GRace::kRaceType_P2P:
            titleHash = 0xb80bfc8a;
            break;
        case GRace::kRaceType_Tollbooth:
            titleHash = 0x141edfe1;
            break;
        case GRace::kRaceType_SpeedTrap:
            titleHash = 0xf2745852;
            break;
        default:
            titleHash = 0;
            break;
    }
    FEngSetLanguageHash(GetPackageName(), 0x42adb44c, titleHash);
}

void UIQRTrackOptions::BoilerPlateOnline(const bool &boAddLaps) {}

void UIQRTrackOptions::SetupCircuit() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
        AddToggleOption(td, true);
    }
    NumLaps *nl = new ("NumLaps", 0) NumLaps(true);
    AddToggleOption(nl, true);
    bool isSplitScreen = FEDatabase->IsSplitScreenMode();
    if (!isSplitScreen) {
        TrafficLevel *tl = new ("TrafficLevel", 0) TrafficLevel(true);
        AddToggleOption(tl, true);
    }
    isSplitScreen = FEDatabase->IsSplitScreenMode();
    if (!isSplitScreen) {
        NumOpponents *no = new ("NumOpponents", 0) NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new ("AISkill", 0) AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new ("CatchUp", 0) CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupSprint() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
        AddToggleOption(td, true);
    }
    bool isSplitScreen = FEDatabase->IsSplitScreenMode();
    if (!isSplitScreen) {
        TrafficLevel *tl = new ("TrafficLevel", 0) TrafficLevel(true);
        AddToggleOption(tl, true);
    }
    isSplitScreen = FEDatabase->IsSplitScreenMode();
    if (!isSplitScreen) {
        NumOpponents *no = new ("NumOpponents", 0) NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new ("AISkill", 0) AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new ("CatchUp", 0) CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupDrag() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
        AddToggleOption(td, true);
    }
    bool isSplitScreen = FEDatabase->IsSplitScreenMode();
    if (!isSplitScreen) {
        NumOpponents *no = new ("NumOpponents", 0) NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new ("AISkill", 0) AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new ("CatchUp", 0) CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupKnockout() {
    if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
        NumLaps *nl = new ("NumLaps", 0) NumLaps(true);
        AddToggleOption(nl, true);
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
        NumOpponents *no = new ("NumOpponents", 0) NumOpponents(true);
        AddToggleOption(no, true);
        NumLaps *nl = new ("NumLaps", 0) NumLaps(true);
        unsigned int idx = AddToggleOption(nl, true);
        FEToggleWidget *tw = static_cast<FEToggleWidget *>(GetWidget(idx));
        tw->SetDisableScript(FEHashUpper("GREY"));
        tw = static_cast<FEToggleWidget *>(GetWidget(idx));
        tw->Disable();
        bool isSplitScreen = FEDatabase->IsSplitScreenMode();
        if (!isSplitScreen) {
            TrafficLevel *tl = new ("TrafficLevel", 0) TrafficLevel(true);
            AddToggleOption(tl, true);
        }
        AISkill *ai = new ("AISkill", 0) AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new ("CatchUp", 0) CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupSpeedTrap() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
        bool isSplitScreen = FEDatabase->IsSplitScreenMode();
        if (!isSplitScreen) {
            TrafficLevel *tl = new ("TrafficLevel", 0) TrafficLevel(true);
            AddToggleOption(tl, true);
        }
        isSplitScreen = FEDatabase->IsSplitScreenMode();
        if (!isSplitScreen) {
            NumOpponents *no = new ("NumOpponents", 0) NumOpponents(true);
            AddToggleOption(no, true);
            AISkill *ai = new ("AISkill", 0) AISkill(true);
            AddToggleOption(ai, true);
            CatchUp *cu = new ("CatchUp", 0) CatchUp(true);
            AddToggleOption(cu, true);
        }
    }
}

void UIQRTrackOptions::SetupTollbooth() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new ("TrackDirection", 0) TrackDirection(true);
            AddToggleOption(td, true);
        }
        bool isSplitScreen = FEDatabase->IsSplitScreenMode();
        if (!isSplitScreen) {
            TrafficLevel *tl = new ("TrafficLevel", 0) TrafficLevel(true);
            AddToggleOption(tl, true);
        }
    }
}
