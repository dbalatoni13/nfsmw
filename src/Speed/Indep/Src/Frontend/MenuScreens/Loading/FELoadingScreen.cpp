#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *LoadingScreen::mLoadingScreenPtr;

void LoadingScreen::NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) {}

void LoadingScreen::InitLoadingScreen() {
    mLoadingScreenPtr = bMalloc(0x2C, 0);
}