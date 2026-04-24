// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackOptions.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/FEng/cFEng.h"

#include <types.h>

class GRaceCustom;
extern void GRaceCustom_SetCopsEnabled(GRaceCustom *self, bool enabled);
extern void GRaceCustom_SetNumOpponents(GRaceCustom *self, int num);

extern cFrontendDatabase *FEDatabase;
extern GRaceDatabase *GRaceDatabase_mObj;
extern cFEng *cFEng_mInstance;

extern unsigned long FEHashUpper(const char *str);
extern void FEngSetLanguageHash(const char *pkg, unsigned int obj_hash, unsigned int lang_hash);
extern void FEngSetLanguageHash(FEString *text, unsigned int hash);
extern const char *GetLocalizedString(unsigned int hash);

struct NumOpponents : public FEToggleWidget {
    NumOpponents(bool enabled) : FEToggleWidget(enabled) {}
    ~NumOpponents() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

struct AISkill : public FEToggleWidget {
    AISkill(bool enabled) : FEToggleWidget(enabled) {}
    ~AISkill() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

struct CatchUp : public FEToggleWidget {
    CatchUp(bool enabled) : FEToggleWidget(enabled) {}
    ~CatchUp() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

struct TrafficLevel : public FEToggleWidget {
    TrafficLevel(bool enabled) : FEToggleWidget(enabled) {}
    ~TrafficLevel() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

struct NumLaps : public FEToggleWidget {
    NumLaps(bool enabled) : FEToggleWidget(enabled) {}
    ~NumLaps() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

struct TrackDirection : public FEToggleWidget {
    TrackDirection(bool enabled) : FEToggleWidget(enabled) {}
    ~TrackDirection() override;
    void Act(const char *parent_pkg, unsigned int data) override;
    void Draw() override;
};

UIQRTrackOptions::UIQRTrackOptions(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    m_code = 0;
    msgHandle = 0;
    m_boDisconnectPercAvail = false;
    race = GRaceDatabase_mObj->GetRaceFromHash(FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode)->EventHash);
    iMaxWidgetsOnScreen = 9;
    Setup();
}

void UIQRTrackOptions::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
    case 0x406415e3:
        if (!(FEDatabase->IsOnlineMode()) && !(FEDatabase->IsLANMode())) {
            GRaceCustom *custom = GRaceDatabase_mObj->AllocCustomRace(race);
            GRaceCustom_SetCopsEnabled(custom, false);
            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(race->GetRaceType());
            FEDatabase->FillCustomRace(custom, settings);
            bool isSplitScreen = false;
            if (FEDatabase->IsSplitScreenMode()) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen) {
                GRaceCustom_SetNumOpponents(custom, 1);
            }
            GRaceDatabase_mObj->SetStartupRace(custom, kRaceContext_QuickRace);
            GRaceDatabase_mObj->FreeCustomRace(custom);
        }
        {
            bool isSplitScreen = false;
            if (FEDatabase->IsSplitScreenMode()) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen) {
                cFEng_mInstance->QueuePackageSwitch("PressStart.fng", 0, 0, false);
            } else {
                cFEng_mInstance->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
            }
        }
        break;
    case 0x911ab364:
        cFEng_mInstance->QueuePackageSwitch("Track_Select.fng", 0, 0, false);
        break;
    case 0xc519bfc4: {
        const char *locStr = GetLocalizedString(0x8aef5ae8);
        DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, 0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), locStr);
        break;
    }
    case 0xd05fc3a3:
        FEDatabase->DefaultRaceSettings();
        {
            int count = Options.TraversebList(nullptr);
            for (int i = 0; i < count; i++) {
                FEWidget *w = static_cast<FEWidget *>(Options.GetNode(i));
                w->Draw();
            }
        }
        break;
    case 0x34dc1bcf:
        break;
    }
}

void UIQRTrackOptions::Setup() {
    GRace::Type raceType = FEDatabase->RaceMode;
    if (raceType == GRace::kRaceType_Drag) {
        SetupDrag();
    } else if (raceType < GRace::kRaceType_Drag) {
        if (raceType == GRace::kRaceType_P2P) {
            SetupSprint();
        } else if (raceType == GRace::kRaceType_Circuit) {
            SetupCircuit();
        } else {
            SetupCircuit();
        }
    } else if (raceType == GRace::kRaceType_Tollbooth) {
        SetupTollbooth();
    } else if (raceType == GRace::kRaceType_Knockout) {
        SetupKnockout();
    } else if (raceType == GRace::kRaceType_SpeedTrap) {
        SetupSpeedTrap();
    } else {
        SetupCircuit();
    }
    SetInitialOption(0);
    if ((FEDatabase->IsOnlineMode()) || (FEDatabase->IsLANMode())) {
        FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x7dadee33);
        return;
    }
    unsigned int titleHash = 0;
    GRace::Type type = race->GetRaceType();
    switch (type) {
        case GRace::kRaceType_P2P: titleHash = 0xb80bfc8a; break;
        case GRace::kRaceType_Circuit: titleHash = 0xa6ed015d; break;
        case GRace::kRaceType_Drag: titleHash = 0xec86e188; break;
        case GRace::kRaceType_Knockout: titleHash = 0xcc959b8; break;
        case GRace::kRaceType_Tollbooth: titleHash = 0x141edfe1; break;
        case GRace::kRaceType_SpeedTrap: titleHash = 0xf2745852; break;
        default: break;
    }
    FEngSetLanguageHash(GetPackageName(), 0x42adb44c, titleHash);
}

void UIQRTrackOptions::BoilerPlateOnline(const bool &boAddLaps) {
}

void UIQRTrackOptions::SetupCircuit() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new TrackDirection(true);
        AddToggleOption(td, true);
    }
    NumLaps *nl = new NumLaps(true);
    AddToggleOption(nl, true);
    bool isSplitScreen = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplitScreen = FEDatabase->iNumPlayers == 2;
    }
    if (!isSplitScreen) {
        TrafficLevel *tl = new TrafficLevel(true);
        AddToggleOption(tl, true);
    }
    isSplitScreen = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplitScreen = FEDatabase->iNumPlayers == 2;
    }
    if (!isSplitScreen) {
        NumOpponents *no = new NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupSprint() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new TrackDirection(true);
        AddToggleOption(td, true);
    }
    bool isSplitScreen = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplitScreen = FEDatabase->iNumPlayers == 2;
    }
    if (!isSplitScreen) {
        TrafficLevel *tl = new TrafficLevel(true);
        AddToggleOption(tl, true);
    }
    isSplitScreen = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplitScreen = FEDatabase->iNumPlayers == 2;
    }
    if (!isSplitScreen) {
        NumOpponents *no = new NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupDrag() {
    if (race->GetCanBeReversed()) {
        TrackDirection *td = new TrackDirection(true);
        AddToggleOption(td, true);
    }
    bool isSplitScreen = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplitScreen = FEDatabase->iNumPlayers == 2;
    }
    if (!isSplitScreen) {
        NumOpponents *no = new NumOpponents(true);
        AddToggleOption(no, true);
        AISkill *ai = new AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupKnockout() {
    if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
        NumLaps *nl = new NumLaps(true);
        AddToggleOption(nl, true);
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
        NumOpponents *no = new NumOpponents(true);
        AddToggleOption(no, true);
        NumLaps *nl = new NumLaps(true);
        unsigned int idx = AddToggleOption(nl, true);
        FEToggleWidget *tw = static_cast<FEToggleWidget *>(GetWidget(idx));
        tw->SetDisableScript(FEHashUpper("KO_ROUNDS"));
        tw = static_cast<FEToggleWidget *>(GetWidget(idx));
        tw->Draw();
        bool isSplitScreen = false;
        if (FEDatabase->IsSplitScreenMode()) {
            isSplitScreen = FEDatabase->iNumPlayers == 2;
        }
        if (!isSplitScreen) {
            TrafficLevel *tl = new TrafficLevel(true);
            AddToggleOption(tl, true);
        }
        AISkill *ai = new AISkill(true);
        AddToggleOption(ai, true);
        CatchUp *cu = new CatchUp(true);
        AddToggleOption(cu, true);
    }
}

void UIQRTrackOptions::SetupSpeedTrap() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
        bool isSplitScreen = false;
        if (FEDatabase->IsSplitScreenMode()) {
            isSplitScreen = FEDatabase->iNumPlayers == 2;
        }
        if (!isSplitScreen) {
            TrafficLevel *tl = new TrafficLevel(true);
            AddToggleOption(tl, true);
        }
        isSplitScreen = false;
        if (FEDatabase->IsSplitScreenMode()) {
            isSplitScreen = FEDatabase->iNumPlayers == 2;
        }
        if (!isSplitScreen) {
            NumOpponents *no = new NumOpponents(true);
            AddToggleOption(no, true);
            AISkill *ai = new AISkill(true);
            AddToggleOption(ai, true);
            CatchUp *cu = new CatchUp(true);
            AddToggleOption(cu, true);
        }
    }
}

void UIQRTrackOptions::SetupTollbooth() {
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        bool boAddLaps = false;
        BoilerPlateOnline(boAddLaps);
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
    } else {
        if (race->GetCanBeReversed()) {
            TrackDirection *td = new TrackDirection(true);
            AddToggleOption(td, true);
        }
        bool isSplitScreen = false;
        if (FEDatabase->IsSplitScreenMode()) {
            isSplitScreen = FEDatabase->iNumPlayers == 2;
        }
        if (!isSplitScreen) {
            TrafficLevel *tl = new TrafficLevel(true);
            AddToggleOption(tl, true);
        }
    }
}

// --- Widget Destructors ---

NumOpponents::~NumOpponents() {}
AISkill::~AISkill() {}
CatchUp::~CatchUp() {}
TrafficLevel::~TrafficLevel() {}
NumLaps::~NumLaps() {}
TrackDirection::~TrackDirection() {}

// --- SplitScreen ---

struct SplitScreen : public IconOption {
    SplitScreen(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~SplitScreen() override;
    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override;
};

SplitScreen::~SplitScreen() {}

void SplitScreen::React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) {
    if (data == 0xc407210) {
        _SetQRMode(2);
    }
}

// --- NumOpponents ---

void NumOpponents::Act(const char *parent_pkg, unsigned int data) {
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
        cFEng_mInstance->QueueGameMessage(0x92b703b5, parent_pkg, 0xff);
    }
    bMovedLastUpdate = true;
    BlinkArrows(data);
    Draw();
}

void NumOpponents::Draw() {
    FEngSetLanguageHash(pTitle, 0x3384a679);
    FEString *data = pData;
    const char *fmt = "%d";
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    FEPrintf(data, fmt, settings->NumOpponents);
}

// --- AISkill ---

void AISkill::Act(const char *parent_pkg, unsigned int data) {
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

void AISkill::Draw() {
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
    FEngSetLanguageHash(pTitle, 0x4d156786);
    FEngSetLanguageHash(pData, hash);
}

// --- CatchUp ---

void CatchUp::Act(const char *parent_pkg, unsigned int data) {
    if (data == 0x9120409e || data == 0xb5971bf1) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        RaceSettings *settings2 = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->CatchUp = !settings2->CatchUp;
    }
    bMovedLastUpdate = true;
    BlinkArrows(data);
    Draw();
}

void CatchUp::Draw() {
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    if (settings->CatchUp) {
        FEngSetLanguageHash(pData, 0x417b2604);
    } else {
        FEngSetLanguageHash(pData, 0x70dfe5c2);
    }
    FEngSetLanguageHash(pTitle, 0x8b8e913a);
}

// --- TrafficLevel ---

void TrafficLevel::Act(const char *parent_pkg, unsigned int data) {
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
}

void TrafficLevel::Draw() {
    unsigned int hash = 0;
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    unsigned char level = settings->TrafficDensity;
    switch (level) {
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
    FEngSetLanguageHash(pData, hash);
    FEngSetLanguageHash(pTitle, 0xeb9dfc09);
}

// --- NumLaps ---

void NumLaps::Act(const char *parent_pkg, unsigned int data) {
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
}

void NumLaps::Draw() {
    FEString *data = pData;
    const char *fmt = "%d";
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    FEPrintf(data, fmt, settings->NumLaps);
    FEngSetLanguageHash(pTitle, 0x48494e83);
}

// --- TrackDirection ---

void TrackDirection::Act(const char *parent_pkg, unsigned int data) {
    if (data == 0x9120409e || data == 0xb5971bf1) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        RaceSettings *settings2 = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->TrackDirection = (settings2->TrackDirection == 0);
    }
    bMovedLastUpdate = true;
    BlinkArrows(data);
    Draw();
}

void TrackDirection::Draw() {
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    if (settings->TrackDirection == 0) {
        FEngSetLanguageHash(pData, 0xde6eff34);
    } else {
        FEngSetLanguageHash(pData, 0xa1cd823e);
    }
    FEngSetLanguageHash(pTitle, 0xa88ffeb4);
}
