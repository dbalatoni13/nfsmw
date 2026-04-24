#include "uiPause.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitDemo.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceAbandoned.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

struct FEObject;

void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
unsigned char FEngGetLastButton(const char* pkg_name);
FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
void FEngSetInvisible(FEObject* obj);

unsigned long PauseMenu::mSelectionHash;

PauseMenu::PauseMenu(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) //
{
    mCalledFromPostRace = (sd->Arg != 0);
    Options.SetIdleColor(0xFFFFAE40);
    Options.SetFadeColor(0x00FFAE40);
    FEDatabase->GetOptionsSettings()->CurrentCategory = static_cast<eOptionsCategory>(0);
    Setup();
}

PauseMenu::~PauseMenu() {}

eMenuSoundTriggers PauseMenu::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0x480C9A58 && mCalledFromPostRace) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return maybe;
}

void PauseMenu::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                    unsigned long param2) {
    if (msg != 0x911AB364 || !mCalledFromPostRace) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }
    if (msg == 0x9120409E) {
        return;
    }
    if (msg > 0x9120409E) {
        goto msg_gt_9120409E;
    }
    if (msg == 0x43DA9FD0) {
        goto show_script;
    }
    if (msg > 0x43DA9FD0) {
        goto msg_gt_43DA9FD0;
    }
    if (msg == 0x30EB8F53 || msg == 0x30F32A49) {
        goto show_script;
    }
    return;

msg_gt_43DA9FD0:
    if (msg == 0x451E768E) {
        goto show_script;
    }
    if (msg == 0x911AB364) {
        goto message_911AB364;
    }
    return;

msg_gt_9120409E:
    if (msg == 0xB5AF2461) {
        goto message_B5AF2461;
    }
    if (msg > 0xB5AF2461) {
        goto msg_gt_B5AF2461;
    }
    if (msg == 0xB4623F67) {
        goto message_B4623F67;
    }
    return;

msg_gt_B5AF2461:
    if (msg == 0xE1A57D51) {
        goto show_script;
    }
    if (msg > 0xE1A57D51) {
        goto msg_gt_E1A57D51;
    }
    if (msg == 0xC9BFD1C3) {
        goto show_script;
    }
    return;

msg_gt_E1A57D51:
    if (msg == 0xE1FDE1D1) {
        goto message_E1FDE1D1;
    }
    return;

message_911AB364:
    if (mCalledFromPostRace) {
        return;
    }
    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
    StorePrevNotification(0x911AB364, pobj, param1, param2);
    return;

message_B5AF2461: {
    if (mCalledFromPostRace) {
        return;
    }
    const char* pkg = GetPackageName();
    mSelectionHash = 0xFDAE152F;
    FEngSetScript(pkg, 0x47FF4E7C, 0xDE6EFF34, true);
    return;
}

show_script:
    FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
    return;

message_B4623F67:
    Options.StartFadeIn();
    cFEng::Get()->QueuePackageMessage(0xC6341FF6, GetPackageName(), 0);
    return;

message_E1FDE1D1:
    if (PrevButtonMessage != 0x911AB364) {
        switch (mSelectionHash) {
        case 0xFBDF2EE3:
            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters()
                && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
                MemoryCard::GetInstance()->CancelNextAutoSave();
            }
            new ERestartRace();
            break;
        case 0xFDAE152F:
            break;
        case 0xCDD2635A: {
            new EUnPause();
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().RaceAbandoned();
            }
            MNotifyRaceAbandoned().Post(MNotifyRaceAbandoned::_GetKind());
            return;
        }
        case 0x0506202D:
            new EQuitDemo(DEMO_DISC_ENDREASON_PLAYABLE_QUIT);
            return;
        case 0x33195CF0:
            FEDatabase->SetGameMode(eFE_GAME_MODE_OPTIONS);
            cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 1, 0, false);
            return;
        case 0x78F1C035:
            cFEng::Get()->QueuePackageSwitch("Pause_Performance_Tuning.fng", 0, 0, false);
            return;
        case 0xE5C9C609: {
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().RaceAbandoned();
            }
            new EQuitToFE(GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career
                              ? static_cast<eGarageType>(2)
                              : static_cast<eGarageType>(1),
                          static_cast<const char*>(0));
            return;
        }
        case 0x85162CB0:
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().RaceAbandoned();
            }
            new EQuitToFE(static_cast<eGarageType>(1), "MainMenu.fng");
            return;
        default:
            return;
        }
    }
    new EUnPause();
    return;
}

bool PauseMenu::IsTuningAvailable() {
    bool avail = false;
    unsigned int player_car;
    if (FEDatabase->IsCareerMode()) {
        player_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    } else {
        player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0);
    }
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord* record = stable->GetCarRecordByHandle(player_car);
    FECustomizationRecord* custom = stable->GetCustomizationRecordByHandle(record->Customization);
    if (custom != nullptr) {
        for (int i = 0; i <= 6; i++) {
            avail = avail | CustomTuningScreen::IsTuningAvailable(stable, record, static_cast< Physics::Tunings::Path >(i));
        }
    }
    return avail;
}

void PauseMenu::Setup() {
    if (mCalledFromPostRace) {
        FEngSetLanguageHash(GetPackageName(), 0x863404B5, 0x376EB982);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x863404B5, 0x6C839FBE);
    }
    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_TimeTrial) {
        SetupOnlineOptions();
    } else {
        SetupOptions();
    }
    int lastButton = FEngGetLastButton(GetPackageName());
    if (bFadeInIconsImmediately) {
        Options.StartFadeIn();
    }
    Options.SetInitialPos(lastButton);
    RefreshHeader();
}

void PauseMenu::SetupOptions() {
    if (mCalledFromPostRace) {
        FEngSetInvisible(GetPackageName(), 0x812A09D4);
    }
    if (mCalledFromPostRace) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            if (FEDatabase->IsDDay() || FEDatabase->IsFinalEpicChase()) {
                AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
            } else {
                AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new("", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
            }
        } else {
            AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            GRaceParameters* pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                pm_QuitMainMenu* opt = new("", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0);
                AddOption(opt);
            } else {
                pm_QuitQuickRace* opt = new("", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0);
                AddOption(opt);
            }
        }
        return;
    }
    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            if (FEDatabase->IsDDay()) {
                AddOption(new("", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase()) {
                AddOption(new("", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (PostRacePursuitScreen::GetPursuitData().mPursuitIsActive) {
                AddOption(new("", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new("", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        } else {
            GRaceParameters* pParams = nullptr;
            if (GRaceStatus::Exists()) {
                pParams = GRaceStatus::Get().GetRaceParameters();
            }
            bool isEpicPursuit = false;
            if (pParams != nullptr && pParams->GetIsEpicPursuitRace()) {
                isEpicPursuit = true;
            }
            if (FEDatabase->IsDDay()) {
                AddOption(new("", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase() || isEpicPursuit) {
                AddOption(new("", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new("", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new("", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
                AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        }
    } else {
        int userMode = Sim::GetUserMode();
        if (userMode == 1) {
            AddOption(new("", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            AddOption(new("", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0));
            AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        } else {
            AddOption(new("", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new("", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            GRaceParameters* pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                pm_QuitMainMenu* opt = new("", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0);
                AddOption(opt);
            } else {
                pm_QuitQuickRace* opt = new("", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0);
                AddOption(opt);
            }
            if (!GRaceStatus::IsTollboothRace() &&
                (pParams == nullptr || !pParams->GetIsChallengeSeriesRace())) {
                bool tuningAvailable = IsTuningAvailable();
                pm_SwitchToTuning* tuning = new("", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0);
                tuning->Locked = !tuningAvailable;
                AddOption(tuning);
            }
            AddOption(new("", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        }
    }
}

void PauseMenu::SetupOnlineOptions() {
    pm_QuitRaceToFE* opt = new("", 0) pm_QuitRaceToFE(0x4C9E34E6, 0xF95320B8, 0);
    AddOption(opt);
}

void pm_ResumeRace::React(const char* pkg_name, unsigned int data, FEObject* obj,
                          unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xFDAE152F);
        FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
    }
}

void pm_ResumeFreeRoam::React(const char* pkg_name, unsigned int data, FEObject* obj,
                              unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xFDAE152F);
        FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
    }
}

void pm_RestartRace::React(const char* pkg_name, unsigned int data, FEObject* obj,
                           unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xFBDF2EE3);
        DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                        static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                        0xE1A57D51, 0xB4623F67, 0xB4623F67,
                                        static_cast<eDialogFirstButtons>(1), 0x4D3399A8);
    }
}

void pm_SwitchToOptions::React(const char* pkg_name, unsigned int data, FEObject* obj,
                               unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0x33195CF0);
        FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
    }
}

void pm_SwitchToTuning::React(const char* pkg_name, unsigned int data, FEObject* obj,
                              unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0x78F1C035);
        if (Locked) {
            DialogInterface::ShowOneButton(pkg_name, "InGameDialog.fng",
                                           static_cast<eDialogTitle>(1), 0x417B2601, 0xB4623F67,
                                           0xB4623F67, 0xA7EE8554);
        } else {
            FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
        }
    }
}

void pm_QuitMainMenu::React(const char* pkg_name, unsigned int data, FEObject* obj,
                            unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xE5C9C609);
        DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                        static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                        0xC9BFD1C3, 0xB4623F67, 0xB4623F67,
                                        static_cast<eDialogFirstButtons>(1), 0xA2E9B449);
    }
}

void pm_QuitQuickRace::React(const char* pkg_name, unsigned int data, FEObject* obj,
                             unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xE5C9C609);
        DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                        static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                        0x30F32A49, 0xB4623F67, 0xB4623F67,
                                        static_cast<eDialogFirstButtons>(1), 0x1DB1CDE5);
    }
}

void pm_QuitRaceToFreeRoam::React(const char* pkg_name, unsigned int data, FEObject* obj,
                                  unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        PauseMenu::SetSelectionHash(0xCDD2635A);
        DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                        static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                        0x451E768E, 0xB4623F67, 0xB4623F67,
                                        static_cast<eDialogFirstButtons>(1), 0x9887EB98);
    }
}

void pm_QuitRaceToFE::React(const char* pkg_name, unsigned int data, FEObject* obj,
                            unsigned int param1, unsigned int param2) {
    if (data == 0x0C407210) {
        unsigned int quitMessageHash = 0;
        PauseMenu::SetSelectionHash(0xE5C9C609);
        GRace::Context ctx = GRaceStatus::Get().GetRaceContext();
        if (ctx == GRace::kRaceContext_TimeTrial) {
        } else if (ctx == GRace::kRaceContext_QuickRace) {
            quitMessageHash = 0x1DB1CDE5;
        } else {
            if (FEDatabase->IsDDay() || FEDatabase->IsFinalEpicChase()) {
                quitMessageHash = 0xECD92696;
            } else {
                if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
                    quitMessageHash = 0xCDE4CAE8;
                } else {
                    if (PostRacePursuitScreen::GetPursuitData().mPursuitIsActive) {
                        quitMessageHash = 0x15A1B5A9;
                    } else {
                        quitMessageHash = 0x6925D0BE;
                    }
                }
            }
        }
        DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                        static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD,
                                        0x43DA9FD0, 0xB4623F67, 0xB4623F67,
                                        static_cast<eDialogFirstButtons>(1), quitMessageHash);
    }
}
