#ifndef _PAUSEMENU
#define _PAUSEMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

// total size: 0x170
struct PauseMenu : public IconScrollerMenu {
    static unsigned long mSelectionHash;

    bool mCalledFromPostRace; // offset 0x16C, size 0x1

    PauseMenu(ScreenConstructorData* sd);
    ~PauseMenu() override;

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;

    bool IsTuningAvailable();
    void Setup() override;
    void SetupOptions();
    void SetupOnlineOptions();

    static void SetSelectionHash(unsigned long selectionHash) {
        mSelectionHash = selectionHash;
    }

    static unsigned long GetSelectionHash() {
        return mSelectionHash;
    }
};

struct pm_ResumeRace : public IconOption {
    pm_ResumeRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_ResumeRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_ResumeFreeRoam : public IconOption {
    pm_ResumeFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_ResumeFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_RestartRace : public IconOption {
    pm_RestartRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_RestartRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_SwitchToOptions : public IconOption {
    pm_SwitchToOptions(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_SwitchToOptions() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_SwitchToTuning : public IconOption {
    pm_SwitchToTuning(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_SwitchToTuning() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_QuitMainMenu : public IconOption {
    pm_QuitMainMenu(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitMainMenu() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_QuitQuickRace : public IconOption {
    pm_QuitQuickRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitQuickRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_QuitRaceToFreeRoam : public IconOption {
    pm_QuitRaceToFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitRaceToFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct pm_QuitRaceToFE : public IconOption {
    pm_QuitRaceToFE(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) { SetReactImmediately(true); }
    ~pm_QuitRaceToFE() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

#endif
