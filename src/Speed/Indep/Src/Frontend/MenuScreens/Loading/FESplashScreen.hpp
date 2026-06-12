#ifndef FESPLASHSCREEN_H
#define FESPLASHSCREEN_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FESplashScreen.hpp
// total size: 0x38
// Decl: speed/indep/src/frontend/menuscreens/loading/FESplashScreen.hpp:14
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

    bool bAllowContinue; // offset 0x2C, size 0x1, Decl: speed/indep/src/frontend/menuscreens/loading/FESplashScreen.hpp:27

  private:
    Timer CopyrightNotice;
    Timer SplashStartedTimer;
};

#endif
