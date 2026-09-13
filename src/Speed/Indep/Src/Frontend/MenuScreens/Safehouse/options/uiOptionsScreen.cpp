#include "uiOptionsScreen.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

int UIOptionsScreen::PlayerToEdit = 0;

// STRIPPED
int GetPlayerPort(PlayerNumbers num) {}

int GetPlayerToEditForOptions() {
    return UIOptionsScreen::PlayerToEdit;
}

void SetPlayerToEditForOptions(int player) {
    UIOptionsScreen::PlayerToEdit = player;
}

UIOptionsScreen::UIOptionsScreen(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    mCalledFromPauseMenu = sd->Arg != 0;
    NeedsColorCal = false;
    OriginalAudioSettings = nullptr;
    OriginalVideoSettings = nullptr;
    OriginalGameplaySettings = nullptr;
    OriginalPlayerSettings = nullptr;

    if (mCalledFromPauseMenu) {
        iMaxWidgetsOnScreen = 10;
    } else {
        iMaxWidgetsOnScreen = 9;
    }

    if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_PLAYER && Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        cFEng::Get()->QueuePackageMessage(0x7DB7B6D7, GetPackageName(), nullptr);
        FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);
    }

    Setup();
}

UIOptionsScreen::~UIOptionsScreen() {
    delete OriginalAudioSettings;
    delete OriginalVideoSettings;
    delete OriginalGameplaySettings;
    delete OriginalPlayerSettings;
}

void UIOptionsScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);

    // UNSOLVED: switch jumps
    switch (msg) {
        case __PAD_BACK__:
            if (OptionsDidNotChange()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            } else {
                DialogInterface::ShowTwoButtons(GetPackageName(), mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng", dialog_alert,
                                                LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x775DBA97, dialog_message_no, dialog_message_no,
                                                first_dialog_button2, GetLocalizedString(0xE9CB802F));
            }
            break;
        case 0x775DBA97:
            RestoreOriginals();
            MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case __PAD_BUTTON5__: {
            DialogInterface::ShowTwoButtons(GetPackageName(), mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng", dialog_alert,
                                            LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes, dialog_message_no, dialog_message_no,
                                            first_dialog_button2, GetLocalizedString(0x8AEF5AE8));
            break;
        }
        case __PAD_RTRIGGER__:
        case __PAD_LTRIGGER__:
            if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_PLAYER) {
                cFEng::Get()->QueueSoundMessage(msg == __PAD_LTRIGGER__ ? 0x6B283007 : 0xF4B32D4D, GetPackageName());
                if (!OptionsDidNotChange()) {
                    char buf[128];
                    FEngSNPrintf(buf, 128, GetLocalizedString(0xBA463431), GetPlayerToEditForOptions() + 1);
                    DialogInterface::ShowTwoButtons(GetPackageName(), mCalledFromPauseMenu ? "InGameDialog.fng" : "Dialog.fng", dialog_alert,
                                                    LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x9A5AD46D, 0xA2A07AC4, dialog_message_no,
                                                    first_dialog_button2, buf);
                } else {
                    cFEng::Get()->QueueGameMessage(0x9A5AD46D, nullptr, 0xFF);
                }
            }
            break;
        case 0xA2A07AC4:
            TogglePlayer(true);
            break;
        case 0x9A5AD46D:
            TogglePlayer(false);
            break;
        case dialog_message_yes:
            if (!FEDatabase->GetOptionsSettings()->TheGameplaySettings.AutoSaveOn &&
                FEDatabase->GetOptionsSettings()->CurrentCategory == OC_GAMEPLAY) {
                MemcardEnter(GetPackageName(), GetPackageName(), 0xA1, nullptr, nullptr, 0, 0);
            }

            RestoreDefaults();
            break;
        case FEHASH_EXITCOMPLETE: {
            FEDatabase->SetOptionsDirty(FEDatabase->IsOptionsDirty() || !OptionsDidNotChange());

            if (mCalledFromPauseMenu) {
                cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 1, 0, false);
            } else if (FEDatabase->IsOnlineMode()) {
                cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            }

            if (FEDatabase->GetOptionsSettings()->CurrentCategory == OC_AUDIO) {
                g_pEAXSound->UpdateVolumes(&FEDatabase->GetOptionsSettings()->TheAudioSettings, -1.0f);
            }
            break;
        }
        case __PAD_START__:
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
        case __PAD_UP__:
        case __PAD_ACCEPT__:
            break;
    }
}

void UIOptionsScreen::Setup() {
    // TODO: verify mapping
    const u32 FEObj_ColorCalLogo = 0xE54C30BE;
    const u32 FEObj_ColorCalBlurb = 0x8E1BEA84;
    const u32 FEObj_ColourCalibrationBacking = 0x608BB8C8;
    const u32 FEObj_ScrollBarArrow1 = 0x444969FD;
    const u32 FEObj_ScrollBarArrow2 = 0x444969FE;

    ClearWidgets();
    NeedsColorCal = false;
    mInitialAutoSaveValue = FEDatabase->GetGameplaySettings()->AutoSaveOn;

    FEngSetInvisible(GetPackageName(), FEObj_ColorCalLogo);
    FEngSetInvisible(GetPackageName(), FEObj_ColorCalBlurb);
    FEngSetInvisible(GetPackageName(), FEObj_ColourCalibrationBacking);
    FEngSetInvisible(GetPackageName(), FEObj_ScrollBarArrow1);
    FEngSetInvisible(GetPackageName(), FEObj_ScrollBarArrow2);

    switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
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
            FEngSetVisible(GetPackageName(), FEObj_ScrollBarArrow1);
            FEngSetVisible(GetPackageName(), FEObj_ScrollBarArrow2);
            break;
        case OC_ONLINE:
            SetupOnline();
            break;
    }

    SetInitialOption(0);
}

void UIOptionsScreen::SetupAudio() {
    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;

    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0xF37AF144);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0xB1426DFA);
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x3932C2E4);
    }

    AddSliderOption(new ("AOSFXMasterVol", 0) AOSFXMasterVol(true), true);
    AddSliderOption(new ("AOCarVol", 0) AOCarVol(true), true);
    AddSliderOption(new ("AOSpeechVol", 0) AOSpeechVol(true), true);
    AddSliderOption(new ("AOFEMusicVol", 0) AOFEMusicVol(true), true);
    AddSliderOption(new ("AOIGMusicVol", 0) AOIGMusicVol(true), true);
    AddToggleOption(new ("AOInteractiveMusicVol", 0) AOInteractiveMusicMode(true), true);
    AddToggleOption(new ("AOEATraxMusicMode", 0) AOEATraxMusicMode(true), true);

    if (TheGameFlowManager.IsInFrontend()) {
        AddToggleOption(new ("AOAudioMode", 0) AOAudioMode(true), true);
    }

    OriginalAudioSettings = new ("AudioSettings", 0) AudioSettings();
    *OriginalAudioSettings = *FEDatabase->GetAudioSettings();
}

void UIOptionsScreen::SetupVideo() {
    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;

    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0x8A006328);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0xD94EA03F);
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x48478029);
    }

    AddToggleOption(new ("VOWideScreen", 0) VOWideScreen(true), true);

    OriginalVideoSettings = new ("VideoSettings", 0) VideoSettings();
    *OriginalVideoSettings = *FEDatabase->GetVideoSettings();

    FEngSetScript(GetPackageName(), 0xAD6B204F, FEHASH_APPEAR, true);
}

void UIOptionsScreen::SetupGameplay() {
    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;

    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0x4DF98FB2);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x3936D9F8);
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x01CCE8C2);
    }

    if (ShouldShowAutoSave()) {
        AddToggleOption(new ("GOAutoSave", 0) GOAutoSave(true), true);
    }

    if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN) {
        AddToggleOption(new ("GOJumpCams", 0) GOJumpCams(true), true);
    }

    AddToggleOption(new ("GODamage", 0) GODamage(true), true);
    AddToggleOption(new ("GORearview", 0) GORearview(true), true);
    AddToggleOption(new ("GOSpeedoUnits", 0) GOSpeedoUnits(true), true);

    bool split = Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;
    if (!mCalledFromPauseMenu) {
        split = false;
    }
    if (!split) {
        if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
            AddToggleOption(new ("GOExploringMiniMap", 0) GOExploringMiniMap(true), true);
        }
        AddToggleOption(new ("GORacingMiniMap", 0) GORacingMiniMap(true), true);
    }

    if (OriginalGameplaySettings == nullptr) {
        OriginalGameplaySettings = new ("GameplaySettings", 0) GameplaySettings();
        *OriginalGameplaySettings = *FEDatabase->GetGameplaySettings();
    }
}

void UIOptionsScreen::SetupPlayer() {
    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;

    FEngSetTextureHash(GetPackageName(), 0x8007B4C, 0xD708EFEF);

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0xD9DC2F12);
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0xC055165F);
    }

    FEngSetScript(GetPackageName(), 0x8A41F5B9, FEHASH_APPEAR, true);

    FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);

    if (!GRaceStatus::Exists() || GRaceStatus::Get().GetRaceType() != GRace::kRaceType_Drag) {
        AddToggleOption(new ("POTransmission", 0) POTransmission(true), true);
    }

    AddToggleOption(new ("PODriveCam", 0) PODriveCam(true), true);
    AddToggleOption(new ("POGauges", 0) POGauges(true), true);
    AddToggleOption(new ("POPosition", 0) POPosition(true), true);
    AddToggleOption(new ("POSplitTime", 0) POSplitTime(true), true);
    AddToggleOption(new ("POScore", 0) POScore(true), true);

    if (!GRaceStatus::Exists() || (!GRaceStatus::IsDragRace() && !GRaceStatus::IsTollboothRace())) {
        AddToggleOption(new ("POLeaderBoard", 0) POLeaderBoard(true), true);
    }

    OriginalPlayerSettings = new ("PlayerSettings", 0) PlayerSettings();
    *OriginalPlayerSettings = *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions());
}

void UIOptionsScreen::SetupOnline() {
    const u32 FEObj_Headertext = FEHASH_HEADERTEXT;

    if (mCalledFromPauseMenu) {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0x966C856D);
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_Headertext, 0xE463B5F7);
    }
}

void UIOptionsScreen::RestoreDefaults() {
    bool bOldAutoSaveVal;

    switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
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

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }
}

bool UIOptionsScreen::OptionsDidNotChange() {
    switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
        case OC_AUDIO:
            return *FEDatabase->GetAudioSettings() == *OriginalAudioSettings;
        case OC_VIDEO:
            return *FEDatabase->GetVideoSettings() == *OriginalVideoSettings;
        case OC_GAMEPLAY:
            return *FEDatabase->GetGameplaySettings() == *OriginalGameplaySettings;
        case OC_PLAYER:
            return *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions()) == *OriginalPlayerSettings;
        default:
            return false;
    }
}

void UIOptionsScreen::RestoreOriginals() {
    switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
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

    switch (FEDatabase->GetOptionsSettings()->CurrentCategory) {
        case OC_PLAYER:
            *OriginalPlayerSettings = *FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions());
            FEngSetLanguageHash(GetPackageName(), 0x53BF826D, GetPlayerToEditForOptions() == 0 ? 0x7B070984 : 0x7B070985);
            break;
        default:
            break;
    }

    for (int i = 0; i < Options.CountElements(); i++) {
        Options.GetNode(i)->Draw();
    }
}

bool UIOptionsScreen::ShouldShowAutoSave() {
    return (!GRaceStatus::Exists() || GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career ||
            ((GRaceStatus::Get().GetRaceParameters() != nullptr) && GRaceStatus::Get().GetRaceParameters()->GetIsChallengeSeriesRace())) &&
           (IsMemcardEnabled && IsAutoSaveEnabled && FEDatabase->bProfileLoaded && !FEDatabase->IsOnlineMode());
}
