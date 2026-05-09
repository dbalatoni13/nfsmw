#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGCONTROLLERSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGCONTROLLERSCREEN_H

#include <cstddef>
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"

class LoadingControllerScreen : public MenuScreen {
  public:
    static void *operator new(size_t size) {
        return mLoadingControllerScreenPtr;
    }

    static void *operator new(size_t size, char *file, int line) {
        return mLoadingControllerScreenPtr;
    }

    static void *operator new(size_t size, char *msg) {
        return mLoadingControllerScreenPtr;
    }

    static void operator delete(void *ptr) {}

    static void operator delete(void *ptr, char *msg) {}

    LoadingControllerScreen(ScreenConstructorData *sd);
    ~LoadingControllerScreen() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 p1, u32 p2) override;

    void NotifyLoadingFinished();
    void ClearLoadedControllerTexture();
    void PrepToShowControllerConfig();
    void FinishLoadingControllerTextureCallback(uint32 p);
    void HideControllerConfig();
    void ShowControllerConfig();
    void SetupControllerConfig();
    static void InitLoadingControllerScreen();
    static void CloseLoadingControllerScreen();

    int LoadingFinished;           // offset 0x2C
    GameTipInfo *GameTipToShow;    // offset 0x30
    uint32 WhichControllerTexture; // offset 0x34

  private:
    static void *mLoadingControllerScreenPtr;
};

#endif
