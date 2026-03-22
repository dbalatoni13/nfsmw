#ifndef FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H
#define FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
struct FEGameWonScreen : public MenuScreen {
    static int mCurrentScreen;

    FEGameWonScreen(ScreenConstructorData* sd);
    ~FEGameWonScreen() override;
    static MenuScreen* Create(ScreenConstructorData* sd);
    void Initialize();
    void Setup();
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    static void QueuePackageSwitchForNextScreen();
};

#endif
