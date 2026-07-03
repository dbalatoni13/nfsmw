#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"

extern MenuScreen *FEngFindScreen(const char *package_name);
extern void eUnloadStreamingTexture(unsigned int *name_hash, int param);

void *LoadingTips::mLoadingTipsScreenPtr;
bool LoadingTips::mDoneLoading;
bool LoadingTips::mDoneShowingLoadingTips;

static void LoadingTips_FinishLoadingTexBridge(uint32 p) {
    LoadingTips *ls = static_cast<LoadingTips *>(FEngFindScreen("Loading_Tips.fng"));
    if (ls) {
        ls->FinishLoadingTexCallback(p);
    }
}

LoadingTips::LoadingTips(ScreenConstructorData *sd) : MenuScreen(sd) {
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

void LoadingTips::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
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

void LoadingTips::StartLoadingTipImage() {
    if (CurrentTip) {
        TipTextureHash = FEngHashString(CurrentTip->Name);
        eLoadStreamingTexture(TipTextureHash, LoadingTips_FinishLoadingTexBridge, 0, 0);
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
    eGameTipBins bin = GT_BIN_NONE;
    eGameTipType type = GT_TYPE_GENERAL;
    eGameTipFlags flags = GTF_NONE;
    CareerSettings *career = FEDatabase->GetCareerSettings();
    if (career->HasCareerStarted()) {
        bin = static_cast<eGameTipBins>(1 << career->GetCurrentBin());
    } else {
        if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
            bin = static_cast<eGameTipBins>(GT_BIN_13 | GT_BIN_14 | GT_BIN_15 | GT_BIN_16);
        }
    }
    if (GRaceDatabase::Exists() && !GRaceDatabase::Get().GetStartupRace()) {
        type = static_cast<eGameTipType>(type | GT_TYPE_FREE_ROAM);
    }
    if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        flags = static_cast<eGameTipFlags>(flags | GTF_TRANSITION_TO_INGAME | GFT_RACETYPE_ALL);
    } else {
        flags = static_cast<eGameTipFlags>(flags | GTF_TRANSITION_TO_FE);
    }
    int valid_tips[NUM_GAME_TIPS - 1];
    int num_tips = 0;
    for (int i = 0; i < NUM_GAME_TIPS - 1; i++) {
        GameTipInfo *tip = &GameTipInfoTable[i];
        if ((tip->Bin & bin) && (tip->Category & type) && (tip->Flags & flags)) {
            valid_tips[num_tips] = i;
            num_tips++;
        }
    }

    if (!num_tips) {
        return GAME_TIP_NONE;
    }
    return static_cast<eGameTips>(valid_tips[bRandom(num_tips)]);
}

bool LoadingTips::TipTestLastCarWithTwoStrikes(LoadingScreen::LoadingScreenTypes loading_direction) {
    bool lolley_says_this_means_free_roam = false;
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        lolley_says_this_means_free_roam = true;
    }
    if (!FEDatabase->IsCareerMode() || !lolley_says_this_means_free_roam || loading_direction != LoadingScreen::LS_LOADING_GAME_FROM_FE) {
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
    if (record->GetTimesBusted() == record->TheImpoundData.MaxBusted - 1) {
        return true;
    }
    return false;
}

bool LoadingTips::TipTestFirstTimeOutOfSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction) {
    bool lolley_says_this_means_free_roam = false;
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        lolley_says_this_means_free_roam = true;
    }
    if (!FEDatabase->IsCareerMode() || !lolley_says_this_means_free_roam || loading_direction != LoadingScreen::LS_LOADING_GAME_FROM_FE) {
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

void LoadingTips::AllowInput() {
    if (!mPressAcceptHasBeenShown) {
        mPressAcceptHasBeenShown = true;
        cFEng::Get()->QueuePackageMessage(0x9938A38F, nullptr, nullptr);
        FEManager::Get()->AllowControllerError(true);
    }
}

GameTipInfo *LoadingTips::GetGameTip(eGameTips tip) {
    if (0 < tip && tip < NUM_GAME_TIPS) {
        return &GameTipInfoTable[tip];
    }
    return &GameTipInfoTable[0];
}

void LoadingTips::InitLoadingTipsScreen() {
    mLoadingTipsScreenPtr = bMalloc(sizeof(LoadingTips), nullptr, 0, 0);
}

void LoadingTips::FinishLoadingTexCallback(uint32 p) {
    ShowTipInfo();
}
