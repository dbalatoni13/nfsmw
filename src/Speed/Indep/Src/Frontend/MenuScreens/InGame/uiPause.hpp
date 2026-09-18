#ifndef _PAUSEMENU
#define _PAUSEMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// total size: 0x170
struct PauseMenu : public IconScrollerMenu {
    static u32 mSelectionHash;

    bool mCalledFromPostRace; // offset 0x16C, size 0x1

    PauseMenu(ScreenConstructorData* sd);
    ~PauseMenu() override;

    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(u32 msg, FEObject* pobj, u32 param1, u32 param2) override;

    bool IsTuningAvailable();
    void Setup() override;
    void SetupOptions();
    void SetupOnlineOptions();

    static void SetSelectionHash(u32 selectionHash) {
        mSelectionHash = selectionHash;
    }

    static u32 GetSelectionHash() {
        return mSelectionHash;
    }
};

#endif
