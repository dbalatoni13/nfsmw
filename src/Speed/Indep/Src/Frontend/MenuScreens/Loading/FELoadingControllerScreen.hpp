#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGCONTROLLERSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGCONTROLLERSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct GameTipInfo;

struct LoadingControllerScreen : public MenuScreen {
    LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}
    static void InitLoadingControllerScreen();
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;

  private:
    int LoadingFinished;                  // offset 0x2C
    GameTipInfo *GameTipToShow;           // offset 0x30
    unsigned int WhichControllerTexture;  // offset 0x34
};

#endif
