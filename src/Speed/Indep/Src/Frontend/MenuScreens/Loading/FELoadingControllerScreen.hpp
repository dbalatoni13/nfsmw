#ifndef LOADINGCONTROLLERSCREEN_HPP
#define LOADINGCONTROLLERSCREEN_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FELoadingControllerScreen.hpp
// total size: 0x38
// Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingControllerScreen.hpp:15

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
    static void operator delete(void *ptr) {}            // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingControllerScreen.hpp:30
    static void operator delete(void *ptr, char *msg) {} // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingControllerScreen.hpp:31

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
    static void *mLoadingControllerScreenPtr; // size: 0x4, address: 0x8041C19C, Decl:
                                              // speed/indep/src/frontend/menuscreens/loading/FELoadingControllerScreen.cpp:22
};

#endif
