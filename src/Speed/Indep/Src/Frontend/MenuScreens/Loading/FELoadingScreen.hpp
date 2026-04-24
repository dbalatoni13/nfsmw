#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class LoadingScreen : public MenuScreen {
  public:
    enum LoadingScreenTypes {
        LS_LOADING_FE = 0,
        LS_LOADING_GAME_FROM_FE = 1,
    };

    LoadingScreen(ScreenConstructorData *sd);
    static void InitLoadingScreen();
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    static void *mLoadingScreenPtr;
};

#endif
