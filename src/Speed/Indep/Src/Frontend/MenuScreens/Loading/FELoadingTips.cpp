#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *LoadingTips::mLoadingTipsScreenPtr;

GameTipInfo *LoadingTips::GetGameTip(eGameTips tip) {
    if (static_cast<unsigned int>(tip) - 1 > 0x19) {
        return &GameTipInfoTable[0];
    }
    return &GameTipInfoTable[tip];
}

void LoadingTips::InitLoadingTipsScreen() {
    mLoadingTipsScreenPtr = bMalloc(0x3C, 0);
}

void LoadingTips::FinishLoadingTexCallback(unsigned int p) {
    ShowTipInfo();
}

MenuScreen *CreateLoadingTipsScreen(ScreenConstructorData *sd) {
    return new (LoadingTips::mLoadingTipsScreenPtr) LoadingTips(sd);
}
