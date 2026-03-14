#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern MenuScreen *FEngFindScreen(const char *package_name);

void *LoadingTips::mLoadingTipsScreenPtr;
bool LoadingTips::mDoneLoading;
bool LoadingTips::mDoneShowingLoadingTips;

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

static void LoadingTips_FinishLoadingTexBridge(unsigned int p) {
    LoadingTips *ls = static_cast<LoadingTips *>(FEngFindScreen("Loading_Tips.fng"));
    if (ls) {
        ls->FinishLoadingTexCallback(p);
    }
}
