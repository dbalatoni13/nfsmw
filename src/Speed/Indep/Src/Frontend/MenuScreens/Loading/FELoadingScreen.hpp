#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H

#include <cstddef>
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

    static void *operator new(size_t size) {
        return mLoadingScreenPtr;
    }

    static void *operator new(size_t size, char *file, int line) {
        return mLoadingScreenPtr;
    }

    static void *operator new(size_t size, char *msg) {
        return mLoadingScreenPtr;
    }

    static void operator delete(void *ptr) {}

    static void operator delete(void *ptr, char *msg) {}

    LoadingScreen(ScreenConstructorData *sd);
    ~LoadingScreen() override;
    void NotificationMessage(u32, FEObject *, u32, u32) override {};
    static void InitLoadingScreen();
    static void CloseLoadingScreen();

  private:
    static void *mLoadingScreenPtr;
};

#endif
