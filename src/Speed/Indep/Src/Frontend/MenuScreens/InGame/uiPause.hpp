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

#endif
