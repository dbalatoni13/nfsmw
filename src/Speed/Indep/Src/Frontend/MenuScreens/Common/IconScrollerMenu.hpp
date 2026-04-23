#ifndef _ICONSCROLLERMENU
#define _ICONSCROLLERMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScroller.hpp"

struct FEString;

// total size: 0x16C
struct IconScrollerMenu : public MenuScreen {
protected:
    IconScroller Options;                   // offset 0x2C, size 0x11C
    bool bWasLeftMouseDown;                 // offset 0x148, size 0x1
    bool bFadeInIconsImmediately;           // offset 0x14C, size 0x1
    FEString* pOptionName;                  // offset 0x150, size 0x4
    FEString* pOptionNameShadow;            // offset 0x154, size 0x4
    FEString* pOptionDesc;                  // offset 0x158, size 0x4
    unsigned int PrevButtonMessage;         // offset 0x15C, size 0x4
    FEObject* PrevButtonObj;                // offset 0x160, size 0x4
    unsigned int PrevParam1;                // offset 0x164, size 0x4
    unsigned int PrevParam2;                // offset 0x168, size 0x4

public:
    IconScrollerMenu(ScreenConstructorData* sd);
    ~IconScrollerMenu() override {}

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void StorePrevNotification(unsigned int msg, FEObject* pobj, unsigned int param1, unsigned int param2);
    virtual void RefreshHeader();
    virtual void Setup() {}
    void AddOption(IconOption* option);

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
};

#endif
