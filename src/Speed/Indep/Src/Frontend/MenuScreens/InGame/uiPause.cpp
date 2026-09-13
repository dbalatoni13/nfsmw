#include "uiPause.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitDemo.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERestartRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceAbandoned.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

u32 PauseMenu::mSelectionHash = 0;

PauseMenu::PauseMenu(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    mCalledFromPostRace = sd->Arg != 0;
    Options.SetIdleColor(0xFFFFAE40);
    Options.SetFadeColor(0x00FFAE40);
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_AUDIO;
    Setup();
}

PauseMenu::~PauseMenu() {}

eMenuSoundTriggers PauseMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == FEHASH_SOUND_BACK && mCalledFromPostRace) {
        return UISND_NONE;
    }
    return maybe;
}

// UNSOLVED
void PauseMenu::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg != __PAD_BACK__ || !mCalledFromPostRace) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case __PAD_BACK__:
            if (!mCalledFromPostRace) {
                FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
                StorePrevNotification(__PAD_BACK__, pobj, param1, param2);
            }
            break;
        case __PAD_START__:
            if (!mCalledFromPostRace) {
                SetSelectionHash(0xFDAE152F);
                FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
            }
            break;
        case 0x43DA9FD0:
        case 0x451E768E:
        case 0xC9BFD1C3:
        case 0x30EB8F53:
        case 0x30F32A49:
        case 0xE1A57D51:
            FEngSetScript(GetPackageName(), 0x47FF4E7C, 0xDE6EFF34, true);
            break;
        case 0xB4623F67:
            Options.StartFadeIn();
            cFEng::Get()->QueuePackageMessage(FEHASH_ENABLE_INPUT, GetPackageName(), nullptr);
            break;
        case FEHASH_EXITCOMPLETE:
            if (PrevButtonMessage != __PAD_BACK__) {
                switch (mSelectionHash) {
                    case 0xFBDF2EE3:
                        if (GRaceStatus::Exists() && (GRaceStatus::Get().GetRaceParameters() != nullptr) &&
                            GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
                            MemoryCard::GetInstance()->CancelNextAutoSave();
                        }
                        new ERestartRace();
                        break;
                    case 0xFDAE152F:
                        new EUnPause();
                        break;
                    case 0xCDD2635A: {
                        new EUnPause();
                        if (GRaceStatus::Exists()) {
                            GRaceStatus::Get().RaceAbandoned();
                        }
                        MNotifyRaceAbandoned().Post(0x20d60dbf);
                        break;
                    }
                    case 0x0506202D:
                        new EQuitDemo(DEMO_DISC_ENDREASON_PLAYABLE_QUIT);
                        break;
                    case 0x33195CF0:
                        FEDatabase->SetGameMode(eFE_GAME_MODE_OPTIONS);
                        cFEng::Get()->QueuePackageSwitch("Pause_Main.fng", 1, 0, false);
                        break;
                    case 0x78F1C035:
                        cFEng::Get()->QueuePackageSwitch("Pause_Performance_Tuning.fng", 0, 0, false);
                        break;
                    case 0xE5C9C609: {
                        if (GRaceStatus::Exists()) {
                            GRaceStatus::Get().RaceAbandoned();
                        }
                        new EQuitToFE(GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career ? GARAGETYPE_CAREER_SAFEHOUSE
                                                                                                        : GARAGETYPE_MAIN_FE,
                                      nullptr);
                        break;
                    }
                    case 0x85162CB0:
                        if (GRaceStatus::Exists()) {
                            GRaceStatus::Get().RaceAbandoned();
                        }
                        new EQuitToFE(GARAGETYPE_MAIN_FE, "MainMenu.fng");
                        break;
                }
            }
        case 0x409e9120:
            break;
    }
}

bool PauseMenu::IsTuningAvailable() {
    bool avail = false;
    uint32 player_car;
    if (FEDatabase->IsCareerMode()) {
        player_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    } else {
        player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0);
    }
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *record = stable->GetCarRecordByHandle(player_car);
    FECustomizationRecord *custom = stable->GetCustomizationRecordByHandle(record->Customization);
    if (custom != nullptr) {
        for (int i = 0; i < Physics::Tunings::MAX_TUNINGS; i++) {
            avail = avail | CustomTuningScreen::IsTuningAvailable(stable, record, static_cast<Physics::Tunings::Path>(i));
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
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    RefreshHeader();
}

void PauseMenu::SetupOptions() {
    if (mCalledFromPostRace) {
        FEngSetInvisible(GetPackageName(), 0x812A09D4);
    }

    const u32 FEObj_DEMOBACK = 0; // TODO

    if (mCalledFromPostRace) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            if (FEDatabase->IsDDay() || FEDatabase->IsFinalEpicChase()) {
                AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
            } else {
                AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new ("pm_QuitRaceToFreeRoam", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
            }
        } else {
            AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));

            GRaceParameters *pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                AddOption(new ("pm_QuitMainMenu", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0));
            } else {
                AddOption(new ("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0));
            }
        }
        return;
    }
    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            if (FEDatabase->IsDDay()) {
                AddOption(new ("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase()) {
                AddOption(new ("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (PostRacePursuitScreen::GetPursuitData().mPursuitIsActive) {
                AddOption(new ("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new ("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        } else {
            GRaceParameters *parms = GRaceStatus::Exists() ? GRaceStatus::Get().GetRaceParameters() : nullptr;
            bool isEpicPursuit = parms != nullptr && parms->GetIsEpicPursuitRace();

            if (FEDatabase->IsDDay()) {
                AddOption(new ("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase() || isEpicPursuit) {
                AddOption(new ("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new ("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new ("pm_QuitRaceToFreeRoam", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        }
    } else {
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            AddOption(new ("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            AddOption(new ("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0));
            AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        } else {
            AddOption(new ("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new ("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));

            GRaceParameters *pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                AddOption(new ("pm_QuitMainMenu", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0));
            } else {
                AddOption(new ("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0));
            }

            if (!GRaceStatus::IsTollboothRace() && (pParams == nullptr || !pParams->GetIsChallengeSeriesRace())) {
                AddOption(new ("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
            }

            AddOption(new ("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        }
    }
}

void PauseMenu::SetupOnlineOptions() {
    AddOption(new ("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0xF95320B8, 0));
}
