#ifndef FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H
#define FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class FEGameWonScreen : public MenuScreen {
  public:
    FEGameWonScreen(ScreenConstructorData *sd);
    ~FEGameWonScreen() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    static void Initialize();
    static void QueuePackageSwitchForNextScreen();

    static int mCurrentScreen;
};

#endif
