#ifndef _ICONSCROLLERMENU
#define _ICONSCROLLERMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScroller.hpp"

struct FEString;

// total size: 0x16C
class IconScrollerMenu : public MenuScreen {
  public:
    IconScrollerMenu(ScreenConstructorData *sd);
    ~IconScrollerMenu() override {}
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

    void DelayFadeIn() {
        bFadeInIconsImmediately = true;
    }

    void SetInitialOption(int index) {
        if (bFadeInIconsImmediately) {
            Options.StartFadeIn();
        }
        Options.SetInitialPos(index);
    }

    void StartInput() {
        Options.SetReactToInput(true);
    }

    void StopInput() {
        Options.SetReactToInput(false);
    }

    void StorePrevNotification(uint32 msg, FEObject *pobj, uint32 param1, uint32 param2);

  protected:
    virtual void RefreshHeader();
    virtual void Setup() {}

    void AddOption(IconOption *option);

    IconScroller Options;         // offset 0x2C, size 0x11C
    bool bWasLeftMouseDown;       // offset 0x148, size 0x1
    bool bFadeInIconsImmediately; // offset 0x14C, size 0x1
    FEString *pOptionName;        // offset 0x150, size 0x4
    FEString *pOptionNameShadow;  // offset 0x154, size 0x4
    FEString *pOptionDesc;        // offset 0x158, size 0x4
    uint32 PrevButtonMessage;     // offset 0x15C, size 0x4
    FEObject *PrevButtonObj;      // offset 0x160, size 0x4
    uint32 PrevParam1;            // offset 0x164, size 0x4
    uint32 PrevParam2;            // offset 0x168, size 0x4
};

#endif
