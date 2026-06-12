#ifndef _PAUSEMENU
#define _PAUSEMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"

// total size: 0x170
class PauseMenu : public IconScrollerMenu {
  public:
    PauseMenu(ScreenConstructorData *sd);
    ~PauseMenu() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    static void SetSelectionHash(unsigned long selectionHash) {
        mSelectionHash = selectionHash;
    }

    static u32 GetSelectionHash() {
        return mSelectionHash;
    }

  private:
    bool IsTuningAvailable();
    void Setup() override;
    void SetupOptions();
    void SetupOnlineOptions();

    static u32 mSelectionHash;

    bool mCalledFromPostRace; // offset 0x16C, size 0x1
};

class pm_ResumeRace : public IconOption {
  public:
    pm_ResumeRace(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_ResumeRace() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFDAE152F);
            FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
        }
    };
};

class pm_ResumeFreeRoam : public IconOption {
  public:
    pm_ResumeFreeRoam(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_ResumeFreeRoam() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFDAE152F);
            FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
        }
    };
};

class pm_RestartRace : public IconOption {
  public:
    pm_RestartRace(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_RestartRace() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xFBDF2EE3);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD, 0xE1A57D51,
                                            0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), 0x4D3399A8);
        }
    };
};

class pm_SwitchToOptions : public IconOption {
  public:
    pm_SwitchToOptions(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_SwitchToOptions() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0x33195CF0);
            FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
        }
    };
};

class pm_SwitchToTuning : public IconOption {
  public:
    pm_SwitchToTuning(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_SwitchToTuning() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0x78F1C035);
            if (Locked) {
                DialogInterface::ShowOneButton(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0xB4623F67, 0xB4623F67,
                                               0xA7EE8554);
            } else {
                FEngSetScript(pkg_name, 0x47FF4E7C, 0xDE6EFF34, true);
            }
        }
    };
};

class pm_QuitMainMenu : public IconOption {
  public:
    pm_QuitMainMenu(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_QuitMainMenu() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xE5C9C609);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD, 0xC9BFD1C3,
                                            0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), 0xA2E9B449);
        }
    };
};

class pm_QuitQuickRace : public IconOption {
  public:
    pm_QuitQuickRace(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_QuitQuickRace() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xE5C9C609);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD, 0x30F32A49,
                                            0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), 0x1DB1CDE5);
        }
    }
};

class pm_QuitRaceToFreeRoam : public IconOption {
  public:
    pm_QuitRaceToFreeRoam(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_QuitRaceToFreeRoam() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0x0C407210) {
            PauseMenu::SetSelectionHash(0xCDD2635A);
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD, 0x451E768E,
                                            0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), 0x9887EB98);
        }
    };
};

class pm_QuitRaceToFE : public IconOption {
  public:
    pm_QuitRaceToFE(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {
        SetReactImmediately(true);
    }
    ~pm_QuitRaceToFE() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
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
            DialogInterface::ShowTwoButtons(pkg_name, "InGameDialog.fng", static_cast<eDialogTitle>(1), 0x417B2601, 0x1A294DAD, 0x43DA9FD0,
                                            0xB4623F67, 0xB4623F67, static_cast<eDialogFirstButtons>(1), quitMessageHash);
        }
    }
};

#endif
