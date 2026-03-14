#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *LoadingScreen::mLoadingScreenPtr;

LoadingScreen::LoadingScreen(ScreenConstructorData *sd) : MenuScreen(sd) {}

void LoadingScreen::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) {}

void LoadingScreen::InitLoadingScreen() {
    mLoadingScreenPtr = bMalloc(0x2C, 0);
}

MenuScreen *CreateLoadingScreen(ScreenConstructorData *sd) {
    return new (LoadingScreen::mLoadingScreenPtr) LoadingScreen(sd);
}