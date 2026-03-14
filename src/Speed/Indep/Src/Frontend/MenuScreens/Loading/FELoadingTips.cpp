#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern MenuScreen *FEngFindScreen(const char *package_name);
extern void eUnloadStreamingTexture(unsigned int *name_hash, int param);

void *LoadingTips::mLoadingTipsScreenPtr;
bool LoadingTips::mDoneLoading;
bool LoadingTips::mDoneShowingLoadingTips;

LoadingTips::LoadingTips(ScreenConstructorData *sd)
    : MenuScreen(sd) //
{
    DisplayTime.ResetHigh();
    CurrentTip = nullptr;
    GameTipInfo *tip = GetGameTip(static_cast<eGameTips>(sd->Arg));
    CurrentTip = tip;
    if (tip->Flags & 0x400) {
        mDoneShowingLoadingTips = false;
    } else {
        mDoneShowingLoadingTips = true;
    }
    mDoneLoading = false;
    mPressAcceptHasBeenShown = false;
    StartLoadingTipImage();
}

LoadingTips::~LoadingTips() {
    unsigned int hash = TipTextureHash;
    eUnloadStreamingTexture(&hash, 1);
}

GameTipInfo *LoadingTips::GetGameTip(eGameTips tip) {
    if (static_cast<unsigned int>(tip) - 1 <= 0x19) {
        return &GameTipInfoTable[tip];
    }
    return &GameTipInfoTable[0];
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
