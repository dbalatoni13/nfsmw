#ifndef FRONTEND_MENUSCREENS_LOADING_FESPLASHSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FESPLASHSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class SplashScreen : public MenuScreen {
  public:
    SplashScreen(ScreenConstructorData *);
    ~SplashScreen() override;
    void NotificationMessage(u32, FEObject *, u32, u32) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override {
        if (bAllowContinue) {
            return maybe;
        }
        return static_cast<eMenuSoundTriggers>(-1);
    }
    Timer CalculateLastJoyEventTime();

    bool bAllowContinue;

  private:
    Timer CopyrightNotice;
    Timer SplashStartedTimer;
};

#endif
