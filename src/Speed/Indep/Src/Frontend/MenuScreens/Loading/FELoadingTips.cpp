#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

GameTipInfo GameTipInfoTable[] = {
    {"TIP_STRIKEOUT", GT_BIN_NONE, GT_TYPE_NONE, GFT_WAIT_FOR_BUTTON_PRESS},
    {"TIP_ENGAGE_MECHANIC", GT_BIN_NONE, GT_TYPE_NONE, GFT_WAIT_FOR_BUTTON_PRESS},
    {"TIP_WORLDMAP", GT_BIN_NONE, GT_TYPE_NONE, GFT_WAIT_FOR_BUTTON_PRESS},
    {"TIP_CUSTOMIZATION_SHOPS", static_cast<eGameTipBins>(0xD800), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_CARLOTS", static_cast<eGameTipBins>(0x2400), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_ENGAGE_MECHANIC", static_cast<eGameTipBins>(0xE000), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_SMS", static_cast<eGameTipBins>(0xE200), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_UNLOCKS", static_cast<eGameTipBins>(0x4800), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_SPEEDBREAKER", static_cast<eGameTipBins>(0x1440), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_ROADBLOCKS", static_cast<eGameTipBins>(0x5540), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_SUVRAM", static_cast<eGameTipBins>(0xA80), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_CROSS", static_cast<eGameTipBins>(0x2A), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_HELICOPTER", static_cast<eGameTipBins>(0x150), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_ROLLING_ROADBLOCK", static_cast<eGameTipBins>(0xA800), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_BOX_FORMATION", static_cast<eGameTipBins>(0x5400), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_PURSUIT_BREAKERS", static_cast<eGameTipBins>(0xE490), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_HIDING_SPOTS", static_cast<eGameTipBins>(0xE924), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_BUSTED", static_cast<eGameTipBins>(0x1210), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_WORLDMAP", static_cast<eGameTipBins>(0x5400), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_RIVAL_CHALLENGES", static_cast<eGameTipBins>(0xC000), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_MARKER_REWARDS", static_cast<eGameTipBins>(0x2A00), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_BLACKLIST", static_cast<eGameTipBins>(0x108A), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_BOUNTY", static_cast<eGameTipBins>(0x4A0), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_CHALLENGE_SERIES", static_cast<eGameTipBins>(0x96), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_RAP_SHEET", static_cast<eGameTipBins>(0x328), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_SAFEHOUSE", static_cast<eGameTipBins>(0xE000), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"TIP_STRIKEOUT", static_cast<eGameTipBins>(0x1F80), GT_TYPE_FREE_ROAM, GTF_TRANSITION_TO_INGAME},
    {"GAME_TIP_USE_CONTROLLER_CONFIG", GT_BIN_ALL, GT_TYPE_ALL, GTF_TRANSITION_TO_INGAME},
};
void *LoadingTips::mLoadingTipsScreenPtr = nullptr;
bool LoadingTips::mDoneShowingLoadingTips = true;
bool LoadingTips::mDoneLoading = false;

void LoadingTips_FinishLoadingTexBridge(unsigned int p) {
    LoadingTips *screen = static_cast<LoadingTips *>(FEngFindScreen("Loading_Tips.fng"));
    if (screen != nullptr) {
        screen->FinishLoadingTexCallback(p);
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
    unsigned int tex = TipTextureHash;
    eUnloadStreamingTexture(&tex, 1);
}

void LoadingTips::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xC98356BA:
            if (mDoneLoading && (RealTimer - DisplayTime).GetSeconds() > 5.0f) {

                if (CurrentTip->Flags & GFT_WAIT_FOR_BUTTON_PRESS) {
                    AllowInput();
                }
            }
            break;

        case 0x406415E3:
        case 0x0C407210:
            if (CurrentTip != nullptr && (CurrentTip->Flags & GFT_WAIT_FOR_BUTTON_PRESS)) {
                mDoneShowingLoadingTips = true;
                FEManager::Get()->AllowControllerError(false);
            }
            break;

        default:
            break;
    }
}

void LoadingTips::StartLoadingTipImage() {
    if (CurrentTip != nullptr) {
        TipTextureHash = FEngHashString(CurrentTip->Name);
        unsigned int tex = TipTextureHash;
        eLoadStreamingTexture(&tex, 1, reinterpret_cast<void (*)(void *)>(LoadingTips_FinishLoadingTexBridge), nullptr, 0);
    }
}

void LoadingTips::ShowTipInfo() {
    if (CurrentTip == nullptr) {
        CurrentTip = &GameTipInfoTable[GAME_TIP_HIDING_SPOTS];
        if (CurrentTip == nullptr) {
            return;
        }
    }

    unsigned int lang_hash;
    lang_hash = FEngHashString("%s_DESC", CurrentTip->Name);
    FEngSetLanguageHash(GetPackageName(), 0xC5FBC710, lang_hash);
    lang_hash = FEngHashString("%s_HEADER", CurrentTip->Name);
    FEngSetLanguageHash(GetPackageName(), 0x0D555245, lang_hash);
    unsigned int tex = TipTextureHash;
    FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xC9D77CB6), tex);
    FEngSetScript(GetPackageName(), 0x3248E720, FEHASH_APPEAR, true);
    DisplayTime = RealTimer;
}

eGameTips LoadingTips::WhatTipScreenShouldIUseToday(LoadingScreen::LoadingScreenTypes loading_direction) {
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
    unsigned int bin = 0;
    unsigned int type = GT_TYPE_GENERAL;
    unsigned int flags = 0;
    int valid_tips[28];
    int num_tips;

    CareerSettings *career = FEDatabase->GetCareerSettings();
    if (career->HasCareerStarted()) {
        bin = 1u << career->GetCurrentBin();
    } else if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        bin = 0x1E000;
    }

    if (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr) {
        type |= GT_TYPE_FREE_ROAM;
    }

    if (loading_direction == LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        flags |= 0x20F;
    } else {
        flags |= GTF_TRANSITION_TO_FE;
    }

    num_tips = 0;
    for (int i = 0; i <= GAME_TIP_USE_CONTROLLER_CONFIG; i++) {
        GameTipInfo *tip = &GameTipInfoTable[i];
        if ((tip->Bin & bin) && (tip->Category & type) && (tip->Flags & flags)) {
            valid_tips[num_tips] = i;
            num_tips++;
        }
    }

    if (num_tips == 0) {
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
    if (stable != nullptr) {
        int num_cars = stable->GetNumAvailableCareerCars();
        if (num_cars == 1) {
            UserProfile *prof = FEDatabase->GetUserProfile(0);
            if (prof != nullptr) {
                CareerSettings *fe_career = prof->GetCareer();
                if (fe_career != nullptr) {
                    FECarRecord *fe_car = stable->GetCarRecordByHandle(fe_career->GetCurrentCar());
                    if (fe_car != nullptr && fe_car->IsValid()) {
                        FECareerRecord *record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                        if (record != nullptr && record->GetTimesBusted() == (record->TheImpoundData.MaxBusted - 1)) {
                            return true;
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

    if (!FEDatabase->IsCareerMode() || !lolley_says_this_means_free_roam || loading_direction != LoadingScreen::LS_LOADING_GAME_FROM_FE) {
        return false;
    }

    CareerSettings *career = FEDatabase->GetCareerSettings();
    if (career == nullptr) {
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
    if (FEDatabase->IsCareerMode() && loading_direction == LoadingScreen::LS_LOADING_FE) {
        if (!FEDatabase->GetCareerSettings()->HasDoneCareerIntro()) {
            if (FEDatabase->IsPostRivalMode()) {
                return true;
            }
        }
    }

    return false;
}

void LoadingTips::AllowInput() {
    if (mPressAcceptHasBeenShown) {
        return;
    }
    mPressAcceptHasBeenShown = true;
    cFEng::Get()->QueuePackageMessage(0x9938A38F, nullptr, nullptr);
    FEManager::Get()->AllowControllerError(true);
}

GameTipInfo *LoadingTips::GetGameTip(eGameTips tip) {
    unsigned int idx = static_cast<unsigned int>(tip) - 1;
    if (idx <= 0x19) {
        return &GameTipInfoTable[tip];
    }
    return &GameTipInfoTable[GAME_TIP_HIDING_SPOTS];
}

void LoadingTips::InitLoadingTipsScreen() {
    mLoadingTipsScreenPtr = bMalloc(0x3C, 0);
}

void LoadingTips::FinishLoadingTexCallback(uint32 p) {
    ShowTipInfo();
}
