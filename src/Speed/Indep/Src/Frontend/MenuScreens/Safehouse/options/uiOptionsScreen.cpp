#include "uiOptionsScreen.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

FEImage* FEngFindImage(const char* pkg_name, int name_hash);
void FEngSetTextureHash(FEImage* image, unsigned int texture_hash);
int FEngSNPrintf(char* buffer, int buf_size, const char* fmt, ...);
const char* GetLocalizedString(unsigned int hash);

extern EAXSound* g_pEAXSound;

#define _DIALOGINTERFACE
enum eDialogTitle {};
enum eDialogFirstButtons {};

struct DialogInterface {
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, ...);
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, const char* fmt, ...);
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, unsigned int lang_hash, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, const char* fmt, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, unsigned int lang_hash, ...);
};

inline void FEngSetTextureHash(const char* pkg_name, unsigned int obj_hash,
                               unsigned int texture_hash) {
    FEngSetTextureHash(FEngFindImage(pkg_name, obj_hash), texture_hash);
}

int UIOptionsScreen::PlayerToEdit = 0;

UIOptionsScreen::UIOptionsScreen(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) //
    , mCalledFromPauseMenu(sd->Arg != 0) //
    , NeedsColorCal(false) //
    , OriginalAudioSettings(nullptr) //
    , OriginalVideoSettings(nullptr) //
    , OriginalGameplaySettings(nullptr) //
    , OriginalPlayerSettings(nullptr) {
    iMaxWidgetsOnScreen = mCalledFromPauseMenu ? 10 : 9;

    if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_PLAYER &&
        Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        cFEng::Get()->QueuePackageMessage(0x7DB7B6D7, GetPackageName(), 0);
        const char* pkg = GetPackageName();
        FEngSetLanguageHash(pkg, 0x53BF826D,
                            GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);
    }

    Setup();
}

UIOptionsScreen::~UIOptionsScreen() {
    delete OriginalAudioSettings;
    delete OriginalVideoSettings;
    delete OriginalGameplaySettings;
    delete OriginalPlayerSettings;
}

void UIOptionsScreen::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                          unsigned long param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
    case 0x911AB364:
        if (OptionsDidNotChange()) {
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        } else {
            const char* pkg_name = GetPackageName();
            const char* prompt = GetLocalizedString(0xE9CB802F);
            DialogInterface::ShowTwoButtons(pkg_name,
                                            mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng",
                                            static_cast<eDialogTitle>(1), 0x70E01038, 0x417B25E4,
                                            0x775DBA97, 0x34DC1BCF, 0x34DC1BCF,
                                            static_cast<eDialogFirstButtons>(1), prompt);
        }
        break;
    case 0x775DBA97:
        RestoreOriginals();
        MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), 0);
        break;
    case 0xC519BFC4: {
        const char* pkg_name = GetPackageName();
        const char* prompt = GetLocalizedString(0x8AEF5AE8);
        DialogInterface::ShowTwoButtons(pkg_name,
                                        mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng",
                                        static_cast<eDialogTitle>(1), 0x70E01038, 0x417B25E4,
                                        0xD05FC3A3, 0x34DC1BCF, 0x34DC1BCF,
                                        static_cast<eDialogFirstButtons>(1), prompt);
        break;
    }
    case 0xD9FEEC59:
    case 0x5073EF13:
        if (FEDatabase->GetOptionsSettings()->CurrentCategory != OC_PLAYER) {
            return;
        }
        {
            cFEng* eng = cFEng::Get();
            unsigned int snd = 0xF4B32D4D;
            if (msg == 0x5073EF13) {
                snd = 0x6B283007;
            }
            eng->QueueSoundMessage(snd, GetPackageName());
            if (!OptionsDidNotChange()) {
                char buf[128];
                const char* fmt = GetLocalizedString(0xBA463431);
                FEngSNPrintf(buf, 128, fmt, GetPlayerToEditForOptions() + 1);
                DialogInterface::ShowTwoButtons(GetPackageName(),
                                                mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng",
                                                static_cast<eDialogTitle>(1), 0x70E01038, 0x417B25E4,
                                                0x9A5AD46D, 0xA2A07AC4, 0x34DC1BCF,
                                                static_cast<eDialogFirstButtons>(1), buf);
            } else {
                eng->QueueGameMessage(0x9A5AD46D, 0, 0xFF);
            }
        }
        break;
    case 0xA2A07AC4:
        TogglePlayer(true);
        break;
    case 0x9A5AD46D:
        TogglePlayer(false);
        break;
    case 0xD05FC3A3:
        {
            OptionsSettings* options_settings = FEDatabase->GetOptionsSettings();
            if (!FEDatabase->GetGameplaySettings()->AutoSaveOn &&
                options_settings->CurrentCategory == OC_GAMEPLAY) {
                MemcardEnter(GetPackageName(), GetPackageName(), 0xA1, 0, 0, 0, 0);
            }
        }
        RestoreDefaults();
        break;
    case 0xE1FDE1D1: {
        bool dirty = false;
        if (FEDatabase->IsOptionsDirty() || !OptionsDidNotChange()) {
            dirty = true;
        }
        FEDatabase->SetOptionsDirty(dirty);

        if (mCalledFromPauseMenu) {
            cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 1, 0, false);
        } else if (FEDatabase->IsOnlineMode()) {
            cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
        }

        OptionsSettings* options_settings = FEDatabase->GetOptionsSettings();
        if (options_settings->CurrentCategory != OC_AUDIO) {
            return;
        }
        g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), -1.0f);
        break;
    }
    case 0xB5AF2461:
        new EUnPause();
        break;
    case 0x7E998E5E:
        if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_GAMEPLAY) {
            ClearWidgets();
            SetupGameplay();
            SetInitialOption(0);
        } else {
            for (int i = 0; i < Options.CountElements(); i++) {
                Options.GetNode(i)->Draw();
            }
        }
        break;
    case 0x72619778:
    case 0x406415E3:
        break;
    }
}

void UIOptionsScreen::Setup() {
    ClearWidgets();
    NeedsColorCal = false;
    mInitialAutoSaveValue = FEDatabase->GetGameplaySettings()->AutoSaveOn;

    FEngSetInvisible(GetPackageName(), 0xE54C30BE);
    FEngSetInvisible(GetPackageName(), 0x8E1BEA84);
    FEngSetInvisible(GetPackageName(), 0x608BB8C8);
    FEngSetInvisible(GetPackageName(), 0x444969FD);
    FEngSetInvisible(GetPackageName(), 0x444969FE);

    eOptionsCategory curCat = FEDatabase->GetOptionsSettings()->CurrentCategory;
    switch (curCat) {
    case OC_AUDIO:
        SetupAudio();
        break;
    case OC_VIDEO:
        SetupVideo();
        break;
    case OC_GAMEPLAY:
        SetupGameplay();
        break;
    case OC_PLAYER:
        SetupPlayer();
        FEngSetVisible(GetPackageName(), 0x444969FD);
        FEngSetVisible(GetPackageName(), 0x444969FE);
        break;
    case OC_ONLINE:
        SetupOnline();
        break;
    }

    SetInitialOption(0);
}

void UIOptionsScreen::SetupAudio() {
    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0xF37AF144);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0xB1426DFA);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x3932C2E4);
    }

    AddSliderOption(new AOSFXMasterVol(true), true);
    AddSliderOption(new AOCarVol(true), true);
    AddSliderOption(new AOSpeechVol(true), true);
    AddSliderOption(new AOFEMusicVol(true), true);
    AddSliderOption(new AOIGMusicVol(true), true);
    AddToggleOption(new AOInteractiveMusicMode(true), true);
    AddToggleOption(new AOEATraxMusicMode(true), true);

    if (TheGameFlowManager.IsInFrontend()) {
        AddToggleOption(new AOAudioMode(true), true);
    }

    AudioSettings* temp = new AudioSettings();
    temp->Default();
    OriginalAudioSettings = temp;
    *OriginalAudioSettings = *FEDatabase->GetAudioSettings();
}

void UIOptionsScreen::SetupVideo() {
    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0x8A006328);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0xD94EA03F);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x48478029);
    }

    AddToggleOption(new VOWideScreen(true), true);

    VideoSettings* temp = new VideoSettings();
    temp->Default();
    OriginalVideoSettings = temp;
    *OriginalVideoSettings = *FEDatabase->GetVideoSettings();

    FEngSetScript(GetPackageName(), 0xAD6B204F, 0x5079C8F8, true);
}

void UIOptionsScreen::SetupGameplay() {
    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0x4DF98FB2);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x3936D9F8);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x01CCE8C2);
    }

    bool split = ShouldShowAutoSave();
    if (split) {
        AddToggleOption(new GOAutoSave(true), true);
    }

    if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN) {
        AddToggleOption(new GOJumpCams(true), true);
    }

    AddToggleOption(new GODamage(true), true);
    AddToggleOption(new GORearview(true), true);
    AddToggleOption(new GOSpeedoUnits(true), true);

    bool is_split = Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;
    if (!mCalledFromPauseMenu) is_split = false;
    if (!is_split) {
        if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
            AddToggleOption(new GOExploringMiniMap(true), true);
        }
        AddToggleOption(new GORacingMiniMap(true), true);
    }

    if (OriginalGameplaySettings == nullptr) {
        GameplaySettings* temp = new GameplaySettings();
        temp->Default();
        OriginalGameplaySettings = temp;
        *OriginalGameplaySettings = *FEDatabase->GetGameplaySettings();
    }
}

void UIOptionsScreen::SetupPlayer() {
    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0xD708EFEF);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0xD9DC2F12);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0xC055165F);
    }

    FEngSetScript(GetPackageName(), 0x8A41F5B9, 0x5079C8F8, true);

    FEngSetLanguageHash(GetPackageName(), 0x53BF826D,
                        GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);

    if (!GRaceStatus::Exists() || GRaceStatus::Get().GetRaceType() != GRace::kRaceType_Drag) {
        AddToggleOption(new POTransmission(true), true);
    }

    AddToggleOption(new PODriveCam(true), true);
    AddToggleOption(new POGauges(true), true);
    AddToggleOption(new POPosition(true), true);
    AddToggleOption(new POSplitTime(true), true);
    AddToggleOption(new POScore(true), true);

    if (!GRaceStatus::Exists() || (!GRaceStatus::IsDragRace() && !GRaceStatus::IsTollboothRace())) {
        AddToggleOption(new POLeaderBoard(true), true);
    }

    PlayerSettings* temp = new PlayerSettings();
    temp->Default();
    OriginalPlayerSettings = temp;
    *OriginalPlayerSettings =
        *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions());
}

void UIOptionsScreen::SetupOnline() {
    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x966C856D);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0xE463B5F7);
    }
}

void UIOptionsScreen::RestoreDefaults() {
    bool bOldAutoSaveVal;

    eOptionsCategory curCat = FEDatabase->GetOptionsSettings()->CurrentCategory;
    switch (curCat) {
    case OC_AUDIO:
        FEDatabase->GetAudioSettings()->Default();
        break;
    case OC_VIDEO:
        FEDatabase->GetVideoSettings()->Default();
        break;
    case OC_GAMEPLAY:
        bOldAutoSaveVal = FEDatabase->GetGameplaySettings()->AutoSaveOn;
        FEDatabase->GetGameplaySettings()->Default();
        if (!ShouldShowAutoSave()) {
            FEDatabase->GetGameplaySettings()->AutoSaveOn = bOldAutoSaveVal;
        }
        break;
    case OC_PLAYER:
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DefaultFromOptionsScreen();
        break;
    }

    FEDatabase->GetOptionsSettings()->CurrentCategory = curCat;

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }
}

bool UIOptionsScreen::OptionsDidNotChange() {
    eOptionsCategory curCat = FEDatabase->GetOptionsSettings()->CurrentCategory;
    switch (curCat) {
    case OC_AUDIO:
        return *FEDatabase->GetAudioSettings() == *OriginalAudioSettings;
    case OC_VIDEO:
        return *FEDatabase->GetVideoSettings() == *OriginalVideoSettings;
    case OC_GAMEPLAY:
        return *FEDatabase->GetGameplaySettings() == *OriginalGameplaySettings;
    case OC_PLAYER:
        return *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions()) ==
               *OriginalPlayerSettings;
    default:
        return false;
    }
}

void UIOptionsScreen::RestoreOriginals() {
    eOptionsCategory curCat = FEDatabase->GetOptionsSettings()->CurrentCategory;
    switch (curCat) {
    case OC_AUDIO:
        *FEDatabase->GetAudioSettings() = *OriginalAudioSettings;
        break;
    case OC_VIDEO:
        *FEDatabase->GetVideoSettings() = *OriginalVideoSettings;
        break;
    case OC_GAMEPLAY:
        *FEDatabase->GetGameplaySettings() = *OriginalGameplaySettings;
        break;
    case OC_PLAYER:
        *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions()) = *OriginalPlayerSettings;
        break;
    }
}

void UIOptionsScreen::TogglePlayer(bool revert_changes) {
    if (revert_changes) {
        RestoreOriginals();
    }

    SetPlayerToEditForOptions(GetPlayerToEditForOptions() == 0);

    if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_PLAYER) {
        *OriginalPlayerSettings =
            *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions());

        const char* pkg = GetPackageName();
        unsigned int lang = 0x7B070985;
        if (GetPlayerToEditForOptions() == 0) {
            lang = 0x7B070984;
        }
        FEngSetLanguageHash(pkg, 0x53BF826D, lang);
    }

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }
}

bool UIOptionsScreen::ShouldShowAutoSave() {
    bool result = false;
    if (!GRaceStatus::Exists() ||
        GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career ||
        (GRaceStatus::Get().GetRaceParameters() &&
         GRaceStatus::Get().GetRaceParameters()->GetIsChallengeSeriesRace())) {
        if (IsMemcardEnabled && IsAutoSaveEnabled && FEDatabase->bProfileLoaded &&
            !FEDatabase->IsOnlineMode()) {
            result = true;
        }
    }
    return result;
}
