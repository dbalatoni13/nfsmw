#include "uiPause.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
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
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

// [fe3] CustomTuningScreen vive en zFe2 y HEAD no tiene su cabecera; solo hace
// falta la firma de la estatica para que el simbolo salga bien manglado.
class FEPlayerCarDB;
class FECarRecord;
struct FEObject;

void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);
unsigned char FEngGetLastButton(const char* pkg_name);

struct pm_ResumeRace : public IconOption {
    pm_ResumeRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_ResumeRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFDAE152F);
            FEngSetScript(pkg_name, 0x47FF4E7C, FEHASH_FORWARD, true);
        }
    }
};

struct pm_ResumeFreeRoam : public IconOption {
    pm_ResumeFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_ResumeFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFDAE152F);
            FEngSetScript(pkg_name, 0x47FF4E7C, FEHASH_FORWARD, true);
        }
    }
};

struct pm_RestartRace : public IconOption {
    pm_RestartRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_RestartRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFBDF2EE3);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL,
                                            0xE1A57D51, 0xB4623F67, 0xB4623F67,
                                            static_cast<eDialogFirstButtons>(1), 0x4D3399A8);
        }
    }
};

struct pm_SwitchToOptions : public IconOption {
    pm_SwitchToOptions(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_SwitchToOptions() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0x33195CF0);
            FEngSetScript(pkg_name, 0x47FF4E7C, FEHASH_FORWARD, true);
        }
    }
};

struct pm_SwitchToTuning : public IconOption {
    pm_SwitchToTuning(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash, bool unlocked)
        : IconOption(tex_hash, name_hash, desc_hash) {
        Locked = !unlocked;
        SetReactImmediately(true);
    }
    ~pm_SwitchToTuning() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0x78F1C035);
            if (Locked) {
                DialogInterface::ShowOneButton(pkg_name, "InGameDialog.fng",
                                               static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, 0xB4623F67,
                                               0xB4623F67, 0xA7EE8554);
            } else {
                FEngSetScript(pkg_name, 0x47FF4E7C, FEHASH_FORWARD, true);
            }
        }
    }
};

struct pm_QuitMainMenu : public IconOption {
    pm_QuitMainMenu(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitMainMenu() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xE5C9C609);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL,
                                            0xC9BFD1C3, 0xB4623F67, 0xB4623F67,
                                            static_cast<eDialogFirstButtons>(1), 0xA2E9B449);
        }
    }
};

struct pm_QuitQuickRace : public IconOption {
    pm_QuitQuickRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitQuickRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xE5C9C609);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL,
                                            0x30F32A49, 0xB4623F67, 0xB4623F67,
                                            static_cast<eDialogFirstButtons>(1), 0x1DB1CDE5);
        }
    }
};

struct pm_QuitRaceToFreeRoam : public IconOption {
    pm_QuitRaceToFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitRaceToFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xCDD2635A);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL,
                                            0x451E768E, 0xB4623F67, 0xB4623F67,
                                            static_cast<eDialogFirstButtons>(1), 0x9887EB98);
        }
    }
};

struct pm_QuitRaceToFE : public IconOption {
    pm_QuitRaceToFE(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitRaceToFE() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
               unsigned int param2) override {
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
                        if (PostRacePursuitScreen::GetPursuitData().GetPursuitIsActive()) {
                            quitMessageHash = 0x15A1B5A9;
                        } else {
                            quitMessageHash = 0x6925D0BE;
                        }
                    }
                }
            }
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng",
                                            static_cast<eDialogTitle>(1), LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL,
                                            0x43DA9FD0, 0xB4623F67, 0xB4623F67,
                                            static_cast<eDialogFirstButtons>(1), quitMessageHash);
        }
    }
};

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
    if (msg == 0x480C9A58 && mCalledFromPostRace) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return maybe;
}

void PauseMenu::NotificationMessage(u32 msg, FEObject* pobj, u32 param1,
                                    u32 param2) {
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
    FEngSetScript(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD, true);
    StorePrevNotification(0x911AB364, pobj, param1, param2);
    return;

message_B5AF2461: {
    if (mCalledFromPostRace) {
        return;
    }
    const char* pkg = GetPackageName();
    u32 selection = 0xFDAE152F;
    mSelectionHash = selection;
    FEngSetScript(pkg, 0x47FF4E7C, FEHASH_FORWARD, true);
    return;
}

show_script:
    FEngSetScript(GetPackageName(), 0x47FF4E7C, FEHASH_FORWARD, true);
    return;

message_B4623F67:
    Options.StartFadeIn();
    cFEng::Get()->QueuePackageMessage(FEHASH_ENABLE_INPUT, GetPackageName(), 0);
    return;

message_E1FDE1D1:
    if (PrevButtonMessage == 0x911AB364) {
        new EUnPause();
        return;
    }
    {
        switch (mSelectionHash) {
        case 0xFDAE152F:
            new EUnPause();
            return;
        case 0xFBDF2EE3:
            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters()
                && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
                MemoryCard::GetInstance()->CancelNextAutoSave();
            }
            new ERestartRace();
            new EUnPause();
            return;
        case 0xCDD2635A: {
            new EUnPause();
            if (GRaceStatus::Exists()) {
                GRaceStatus::Get().RaceAbandoned();
            }
            MNotifyRaceAbandoned().Post(UCrc32(0x20d60dbf));
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
    SetInitialOption(FEngGetLastButton(GetPackageName()));
    RefreshHeader();
}

void PauseMenu::SetupOptions() {
    if (mCalledFromPostRace) {
        FEngSetInvisible(GetPackageName(), 0x812A09D4);
    }
    if (mCalledFromPostRace) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            if (FEDatabase->IsDDay() || FEDatabase->IsFinalEpicChase()) {
                AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
            } else {
                AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new("pm_QuitRaceToFreeRoam", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
            }
        } else {
            AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            GRaceParameters* pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                pm_QuitMainMenu* opt = new("pm_QuitMainMenu", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0);
                AddOption(opt);
            } else {
                pm_QuitQuickRace* opt = new("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0);
                AddOption(opt);
            }
        }
        return;
    }
    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            if (FEDatabase->IsDDay()) {
                AddOption(new("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase()) {
                AddOption(new("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (PostRacePursuitScreen::GetPursuitData().GetPursuitIsActive()) {
                AddOption(new("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new("pm_ResumeFreeRoam", 0) pm_ResumeFreeRoam(0x12BB5EA2, 0x01BD185C, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        } else {
            GRaceParameters* pParams;
            if (GRaceStatus::Exists()) {
                pParams = GRaceStatus::Get().GetRaceParameters();
            } else {
                pParams = nullptr;
            }
            bool isEpicPursuit = false;
            if (pParams != nullptr && pParams->GetIsEpicPursuitRace()) {
                isEpicPursuit = true;
            }
            if (FEDatabase->IsDDay()) {
                AddOption(new("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else if (FEDatabase->IsFinalEpicChase() || isEpicPursuit) {
                AddOption(new("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x690E9B7C, 0));
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            } else {
                AddOption(new("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
                AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
                AddOption(new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0x3C14C420, 0));
                AddOption(new("pm_QuitRaceToFreeRoam", 0) pm_QuitRaceToFreeRoam(0x56FFBD2C, 0x9DC599B0, 0));
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
                AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
            }
        }
    } else {
        int userMode = Sim::GetUserMode();
        if (userMode == 1) {
            AddOption(new("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            AddOption(new("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0));
            AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        } else {
            AddOption(new("pm_ResumeRace", 0) pm_ResumeRace(0x12BB5EA2, 0xDED357E7, 0));
            AddOption(new("pm_RestartRace", 0) pm_RestartRace(0xB295A6B6, 0xF893AFA1, 0));
            GRaceParameters* pParams = GRaceStatus::Get().GetRaceParameters();
            if (pParams != nullptr && pParams->GetIsChallengeSeriesRace()) {
                pm_QuitMainMenu* opt = new("pm_QuitMainMenu", 0) pm_QuitMainMenu(0x4C9E34E6, 0xE950B7AF, 0);
                AddOption(opt);
            } else {
                pm_QuitQuickRace* opt = new("pm_QuitQuickRace", 0) pm_QuitQuickRace(0x4C9E34E6, 0x4349998B, 0);
                AddOption(opt);
            }
            if (!GRaceStatus::IsTollboothRace() &&
                (pParams == nullptr || !pParams->GetIsChallengeSeriesRace())) {
                AddOption(new("pm_SwitchToTuning", 0) pm_SwitchToTuning(0x483238FD, 0x6A3672A2, 0, IsTuningAvailable()));
            }
            AddOption(new("pm_SwitchToOptions", 0) pm_SwitchToOptions(0x520DE4E3, 0x2B5A03A8, 0));
        }
    }
}

void PauseMenu::SetupOnlineOptions() {
    pm_QuitRaceToFE* opt = new("pm_QuitRaceToFE", 0) pm_QuitRaceToFE(0x4C9E34E6, 0xF95320B8, 0);
    AddOption(opt);
}
