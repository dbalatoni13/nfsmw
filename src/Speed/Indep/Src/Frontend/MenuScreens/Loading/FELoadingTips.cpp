#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"

void *LoadingTips::mLoadingTipsScreenPtr;
bool LoadingTips::mDoneLoading;
bool LoadingTips::mDoneShowingLoadingTips;

static void LoadingTips_FinishLoadingTexBridge(uint32 p) {
    LoadingTips *ls = static_cast<LoadingTips *>(FEngFindScreen("Loading_Tips.fng"));
    if (ls != nullptr) {
        ls->FinishLoadingTexCallback(p);
    }
}

LoadingTips::LoadingTips(ScreenConstructorData *sd) : MenuScreen(sd) {
    DisplayTime.ResetHigh();
    CurrentTip = nullptr;
    CurrentTip = GetGameTip(static_cast<eGameTips>(sd->Arg));
    if (CurrentTip->Flags & GFT_WAIT_FOR_BUTTON_PRESS) {
        mDoneShowingLoadingTips = false;
    } else {
        mDoneShowingLoadingTips = true;
    }
    mDoneLoading = false;
    mPressAcceptHasBeenShown = false;
    StartLoadingTipImage();
}

LoadingTips::~LoadingTips() {
    eUnloadStreamingTexture(TipTextureHash);
}

void LoadingTips::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_ACCEPT__:
        case __BUTTON_PRESSED__:
            break;

        case FEMSG_SCREEN_TICK:
            if (!mDoneLoading) {
                return;
            }
            if ((RealTimer - DisplayTime).GetSeconds() <= 5.0f) {
                return;
            }
            if (!(CurrentTip->Flags & GFT_WAIT_FOR_BUTTON_PRESS)) {
                return;
            }
            AllowInput();
            return;

        default:
            return;
    }

    if ((CurrentTip != nullptr) && (CurrentTip->Flags & GFT_WAIT_FOR_BUTTON_PRESS)) {
        mDoneShowingLoadingTips = true;
        FEManager::Get()->AllowControllerError(false);
    }
}

void LoadingTips::StartLoadingTipImage() {
    if (CurrentTip != nullptr) {
        TipTextureHash = FEngHashString(CurrentTip->Name);
        eLoadStreamingTexture(TipTextureHash, LoadingTips_FinishLoadingTexBridge, 0, 0);
    }
}

void LoadingTips::ShowTipInfo() {
    if (CurrentTip == nullptr) {
        CurrentTip = &GameTipInfoTable[16];
    }

    if (CurrentTip != nullptr) {
        uint32 lang_hash = FEngHashString("%s_DESC", CurrentTip->Name);
        FEngSetLanguageHash(GetPackageName(), 0xC5FBC710, lang_hash);
        lang_hash = FEngHashString("%s_HEADER", CurrentTip->Name);
        FEngSetLanguageHash(GetPackageName(), 0x0D555245, lang_hash);
        FEngSetTextureHash(GetPackageName(), 0xC9D77CB6, TipTextureHash);
        FEngSetScript(GetPackageName(), 0x3248E720, FEHASH_APPEAR, true);
        DisplayTime = RealTimer;
    }
}

eGameTips LoadingTips::WhatTipScreenShouldIUseToday(LoadingScreen::LoadingScreenTypes loading_direction) {
    if (GRaceDatabase::Exists()) {
        // unused, probably similar to GetARandomTipScreen
        GRaceDatabase::Get();
    }

    if (TipTestLastCarWithTwoStrikes(loading_direction)) {
        return GAME_TIP_LAST_CAR_AND_2_STRIKES;
    }
    if (TipTestFirstTimeIntoSafeHouse(loading_direction)) {
        return GAME_TIP_INTRO_TIP;
    }
    if (TipTestFirstTimeOutOfSafeHouse(loading_direction)) {
        return GAME_TIP_MAP_INTRO_TIP;
    }
    return GetARandomTipScreen(loading_direction);
}

eGameTips LoadingTips::GetARandomTipScreen(LoadingScreen::LoadingScreenTypes loading_direction) {
    uint32 bin = GT_BIN_NONE;
    uint32 type = GT_TYPE_GENERAL;
    uint32 flags = GTF_NONE;
    if (FEDatabase->GetCareerSettings()->HasCareerStarted()) {
        bin = (1 << FEDatabase->GetCareerSettings()->GetCurrentBin());
    } else {
        if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
            bin = (GT_BIN_13 | GT_BIN_14 | GT_BIN_15 | GT_BIN_16);
        }
    }
    if (GRaceDatabase::Exists() && (GRaceDatabase::Get().GetStartupRace() == nullptr)) {
        type = (type | GT_TYPE_FREE_ROAM);
    }
    if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        flags = (flags | GTF_TRANSITION_TO_INGAME | GFT_RACETYPE_ALL);
    } else {
        flags = (flags | GTF_TRANSITION_TO_FE);
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

    if (FEDatabase->IsCareerMode() && lolley_says_this_means_free_roam && loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        if (stable != nullptr) {
            int num_cars = stable->GetNumAvailableCareerCars();
            if (num_cars == 1) {
                UserProfile *prof = FEDatabase->GetUserProfile(0);
                if (prof != nullptr) {
                    CareerSettings *fe_career = prof->GetCareer();
                    if (fe_career != nullptr) {
                        FECarRecord *fe_car = stable->GetCarRecordByHandle(fe_career->GetCurrentCar());
                        if ((fe_car != nullptr) && fe_car->IsValid()) {
                            FECareerRecord *record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                            if (record != nullptr) {
                                if (record->GetTimesBusted() == record->TheImpoundData.MaxBusted - 1) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool LoadingTips::TipTestFirstTimeOutOfSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction) {
    bool lolley_says_this_means_free_roam = false;
    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        lolley_says_this_means_free_roam = true;
    }

    if (FEDatabase->IsCareerMode() && lolley_says_this_means_free_roam && loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        CareerSettings *career = FEDatabase->GetCareerSettings();
        if (career != nullptr && career->HasDoneCareerIntro()) {
#ifdef EA_BUILD_A124
            return true;
#else
            if (!career->HasDoneMapLoadigTip()) {
                career->SetHasDoneMapLoadigTip();
                return true;
            }
#endif
        }
    }
    return false;
}

bool LoadingTips::TipTestFirstTimeIntoSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction) {
    if (FEDatabase->IsCareerMode() && loading_direction == LoadingScreen::LS_LOADING_FE && !FEDatabase->GetCareerSettings()->HasDoneCareerIntro() &&
        FEDatabase->IsPostRivalMode()) {
        return true;
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
    if (0 < tip && tip <= NUM_GAME_TIPS_THAT_USE_TIPS_SCREEN) {
        return &GameTipInfoTable[tip];
    }
    return &GameTipInfoTable[16];
}

void LoadingTips::InitLoadingTipsScreen() {
    mLoadingTipsScreenPtr = bMalloc(sizeof(LoadingTips), "LoadingTips", 0, 0);
}

void LoadingTips::FinishLoadingTexCallback(uint32 p) {
    ShowTipInfo();
}
