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
    mLoadingTipsScreenPtr = bMalloc(0x3C, nullptr, 0, 0);
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

void LoadingTips::StartLoadingTipImage() {
    if (CurrentTip) {
        unsigned int hash = FEngHashString(CurrentTip->Name);
        TipTextureHash = hash;
        eLoadStreamingTexture(&hash, 1, LoadingTips_FinishLoadingTexBridge, 0, 0);
    }
}

void LoadingTips::ShowTipInfo() {
    if (!CurrentTip) {
        CurrentTip = &GameTipInfoTable[0];
    }
    unsigned int lang_hash = FEngHashString("%s_DESC", CurrentTip->Name);
    FEngSetLanguageHash(GetPackageName(), 0xC5FBC710, lang_hash);
    lang_hash = FEngHashString("%s_HEADER", CurrentTip->Name);
    FEngSetLanguageHash(GetPackageName(), 0x0D555245, lang_hash);
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xC9D77CB6), TipTextureHash);
    FEngSetScript(GetPackageName(), 0x3248E720, 0x5079C8F8, true);
    DisplayTime = RealTimer;
}

void LoadingTips::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x406415E3) {
        goto shared;
    }
    if (msg <= 0x406415E3) {
        if (msg == 0x0C407210) {
            goto shared;
        }
        return;
    }
    if (msg != 0xC98356BA) {
        return;
    }
    if (!mDoneLoading) {
        return;
    }
    if ((RealTimer - DisplayTime).GetSeconds() <= 5.0f) {
        return;
    }
    if (!(CurrentTip->Flags & 0x400)) {
        return;
    }
    AllowInput();
    return;

shared:
    if (CurrentTip && (CurrentTip->Flags & 0x400)) {
        mDoneShowingLoadingTips = true;
        FEManager::Get()->AllowControllerError(false);
    }
}

void LoadingTips::AllowInput() {
    if (!mPressAcceptHasBeenShown) {
        mPressAcceptHasBeenShown = true;
        cFEng::Get()->QueuePackageMessage(0x9938A38F, nullptr, nullptr);
        FEManager::Get()->AllowControllerError(true);
    }
}

eGameTips LoadingTips::WhatTipScreenShouldIUseToday(LoadingScreen::LoadingScreenTypes loading_direction) {
    if (TipTestLastCarWithTwoStrikes(loading_direction)) {
        return static_cast<eGameTips>(0);
    }
    if (TipTestFirstTimeIntoSafeHouse(loading_direction)) {
        return static_cast<eGameTips>(1);
    }
    if (TipTestFirstTimeOutOfSafeHouse(loading_direction)) {
        return static_cast<eGameTips>(2);
    }
    return GetARandomTipScreen(loading_direction);
}

eGameTips LoadingTips::GetARandomTipScreen(LoadingScreen::LoadingScreenTypes loading_direction) {
    unsigned int bin = 0;
    unsigned int type = 1;
    unsigned int flags = 0;
    CareerSettings *career = FEDatabase->GetCareerSettings();
    if (!career->HasCareerStarted()) {
        if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
            bin = 0x1E000;
        }
    } else {
        bin = 1 << (career->GetCurrentBin() & 0x3F);
    }
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        type |= 0x10;
    }
    if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        flags |= 0x20F;
    } else {
        flags |= 0x100;
    }
    int valid_tips[28];
    int num_tips = 0;
    for (int i = 0; i < 0x1C; i++) {
        GameTipInfo *tip = &GameTipInfoTable[i];
        if ((tip->Bin & bin) && (tip->Category & type) && (tip->Flags & flags)) {
            valid_tips[num_tips] = i;
            num_tips++;
        }
    }
    int result = 0x1C;
    if (num_tips != 0) {
        result = valid_tips[bRandom(num_tips)];
    }
    return static_cast<eGameTips>(result);
}

bool LoadingTips::TipTestLastCarWithTwoStrikes(LoadingScreen::LoadingScreenTypes loading_direction) {
    bool lolley_says_this_means_free_roam = false;
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        lolley_says_this_means_free_roam = true;
    }
    if (!FEDatabase->IsCareerMode() || !lolley_says_this_means_free_roam ||
        loading_direction != LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        return false;
    }
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    if (!stable) {
        return false;
    }
    if (stable->GetNumAvailableCareerCars() != 1) {
        return false;
    }
    UserProfile *prof = FEDatabase->GetUserProfile(0);
    if (!prof) {
        return false;
    }
    CareerSettings *fe_career = prof->GetCareer();
    if (!fe_career) {
        return false;
    }
    FECarRecord *fe_car = stable->GetCarRecordByHandle(fe_career->GetCurrentCar());
    if (!fe_car) {
        return false;
    }
    if (!fe_car->IsValid()) {
        return false;
    }
    FECareerRecord *record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
    if (!record) {
        return false;
    }
    if (record->GetTimesBusted() == record->GetMaxBusted() - 1) {
        return true;
    }
    return false;
}

bool LoadingTips::TipTestFirstTimeOutOfSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction) {
    bool lolley_says_this_means_free_roam = false;
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        lolley_says_this_means_free_roam = true;
    }
    if (!FEDatabase->IsCareerMode() || !lolley_says_this_means_free_roam ||
        loading_direction != LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        return false;
    }
    CareerSettings *career = FEDatabase->GetCareerSettings();
    if (!career) {
        return false;
    }
    if (!career->HasDoneCareerIntro()) {
        return false;
    }
    if (career->HasDoneMapLoadigTip()) {
        return false;
    }
    career->SetHasDoneMapLoadigTip();
    return true;
}

bool LoadingTips::TipTestFirstTimeIntoSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction) {
    if (FEDatabase->IsCareerMode()) {
        if (loading_direction == LoadingScreen::LS_LOADING_FE) {
            if (!FEDatabase->GetCareerSettings()->HasDoneCareerIntro() && FEDatabase->IsPostRivalMode()) {
                return true;
            }
        }
    }
    return false;
}
