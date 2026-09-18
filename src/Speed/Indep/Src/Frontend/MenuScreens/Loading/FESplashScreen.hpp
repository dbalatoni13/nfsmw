#ifndef FRONTEND_MENUSCREENS_LOADING_FESPLASHSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FESPLASHSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FESplashScreen.hpp
// total size: 0x38
class SplashScreen : public MenuScreen {
  public:
    SplashScreen(ScreenConstructorData *sd);
    ~SplashScreen() override;

    Timer CalculateLastJoyEventTime();

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override {
        if (bAllowContinue) {
            return maybe;
        }
        return static_cast<eMenuSoundTriggers>(-1);
    }

    bool bAllowContinue;      // offset 0x2C, size 0x1
    Timer CopyrightNotice;    // offset 0x30, size 0x4
    Timer SplashStartedTimer; // offset 0x34, size 0x4
};

#endif
