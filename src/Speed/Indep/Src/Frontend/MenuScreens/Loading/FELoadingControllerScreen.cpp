#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingControllerScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *LoadingControllerScreen::mLoadingControllerScreenPtr;

LoadingControllerScreen::LoadingControllerScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

void LoadingControllerScreen::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) {}

void LoadingControllerScreen::FinishLoadingControllerTextureCallback(unsigned int p) {
    ShowControllerConfig();
}

void FinishLoadingControllerTextureCallbackBridge(unsigned int p) {
    if (p != 0) {
        reinterpret_cast<LoadingControllerScreen *>(p)->FinishLoadingControllerTextureCallback(0);
    }
}

void LoadingControllerScreen::InitLoadingControllerScreen() {
    mLoadingControllerScreenPtr = bMalloc(0x38, 0);
}

MenuScreen *CreateLoadingControllerScreen(ScreenConstructorData *sd) {
    return new (LoadingControllerScreen::mLoadingControllerScreenPtr) LoadingControllerScreen(sd);
}