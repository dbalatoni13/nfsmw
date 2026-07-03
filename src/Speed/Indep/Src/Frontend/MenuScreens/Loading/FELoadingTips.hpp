#ifndef LOADINGTIPS_HPP
#define LOADINGTIPS_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FELoadingScreenSelector.hpp
// Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreenSelector.hpp:15
typedef enum {
    GAME_TIP_LAST_CAR_AND_2_STRIKES = 0,
    GAME_TIP_INTRO_TIP = 1,
    GAME_TIP_MAP_INTRO_TIP = 2,
    GAME_TIP_CUSTOMIZATION_SHOPS = 3,
    GAME_TIP_CARLOTS = 4,
    GAME_TIP_ENGAGEMENT_MECHANIC = 5,
    GAME_TIP_SMS = 6,
    GAME_TIP_UNLOCKS = 7,
    GAME_TIP_SPEEDBREAKER = 8,
    GAME_TIP_ROADBLOCKS = 9,
    GAME_TIP_SUVRAM = 10,
    GAME_TIP_CROSS = 11,
    GAME_TIP_HELICOPTER = 12,
    GAME_TIP_ROLLING_ROADBLOCK = 13,
    GAME_TIP_BOX_FORMATION = 14,
    GAME_TIP_PURSUIT_BREAKERS = 15,
    GAME_TIP_HIDING_SPOTS = 16,
    GAME_TIP_BUSTED = 17,
    GAME_TIP_WORLDMAP = 18,
    GAME_TIP_RIVAL_CHALLENGES = 19,
    GAME_TIP_MARKER_REWARDS = 20,
    GAME_TIP_BLACKLIST = 21,
    GAME_TIP_BOUNTY = 22,
    GAME_TIP_CHALLENGE_SERIES = 23,
    GAME_TIP_RAP_SHEET = 24,
    GAME_TIP_SAFEHOUSE = 25,
    GAME_TIP_STRIKEOUT = 26,
    NUM_GAME_TIPS_THAT_USE_TIPS_SCREEN = 26,
    GAME_TIP_USE_CONTROLLER_CONFIG = 27,
    GAME_TIP_NONE = 28,
    NUM_GAME_TIPS = 29,
} eGameTips;

// total size: 0x10
// Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreenSelector.hpp:143
typedef enum {
    GT_BIN_NONE = 0,
    GT_BIN_1 = 2,
    GT_BIN_2 = 4,
    GT_BIN_3 = 8,
    GT_BIN_4 = 16,
    GT_BIN_5 = 32,
    GT_BIN_6 = 64,
    GT_BIN_7 = 128,
    GT_BIN_8 = 256,
    GT_BIN_9 = 512,
    GT_BIN_10 = 1024,
    GT_BIN_11 = 2048,
    GT_BIN_12 = 4096,
    GT_BIN_13 = 8192,
    GT_BIN_14 = 16384,
    GT_BIN_15 = 32768,
    GT_BIN_16 = 65536,
    GT_BIN_UNIQUE = 131072,
    GT_BIN_ALL = -1,
} eGameTipBins;

typedef enum {
    GT_TYPE_NONE = 0,
    GT_TYPE_GENERAL = 1,
    GT_TYPE_BLACKLIST = 2,
    GT_TYPE_CHALLENGE_SERIES = 4,
    GT_TYPE_QUICK_RACE = 8,
    GT_TYPE_FREE_ROAM = 16,
    GT_TYPE_ALL = 65535,
} eGameTipType;

typedef enum {
    GTF_NONE = 0,
    GFT_RACETYPE_CIRCUIT = 1,
    GFT_RACETYPE_DRAG = 2,
    GFT_RACETYPE_SPRINT = 4,
    GFT_RACETYPE_TOLLBOOTH = 8,
    GFT_RACETYPE_KNOCKOUT = 16,
    GFT_RACETYPE_ALL = 15,
    GFT_RACETYPE_MASK = 15,
    GTF_TRANSITION_TO_FE = 256,
    GTF_TRANSITION_TO_INGAME = 512,
    GFT_WAIT_FOR_BUTTON_PRESS = 1024,
} eGameTipFlags;

struct GameTipInfo {
    char *Name;            // offset 0x0
    eGameTipBins Bin;      // offset 0x4
    eGameTipType Category; // offset 0x8
    eGameTipFlags Flags;   // offset 0xC
};

// File: speed/indep/src/frontend/menuscreens/loading/FELoadingTips.hpp
// total size: 0x3C
// Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingTips.hpp:15

class LoadingTips : public MenuScreen {
  public:
    static inline void *operator new(size_t size) {
        return mLoadingTipsScreenPtr;
    }

    static inline void *operator new(size_t size, char *file, int line) {
        return mLoadingTipsScreenPtr;
    }

    static inline void *operator new(size_t size, char *msg) {
        return mLoadingTipsScreenPtr;
    }

    static inline void operator delete(void *ptr) {} // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingTips.hpp:21

    static inline void operator delete(void *ptr, char *msg) {} // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingTips.hpp:22

    LoadingTips(ScreenConstructorData *sd);
    ~LoadingTips() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void FinishLoadingTexCallback(uint32 p);
    static inline bool IsDoneShowingLoadingTips() {
        return mDoneShowingLoadingTips;
    }
    static void InitLoadingTipsScreen();
    static void CloseLoadingTipsScreen();
    static eGameTips WhatTipScreenShouldIUseToday(LoadingScreen::LoadingScreenTypes loading_direction);

  private:
    void ShowTipInfo();
    void StartLoadingTipImage();
    GameTipInfo *GetGameTip(eGameTips tip);
    void AllowInput();
    static eGameTips GetARandomTipScreen(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestLastCarWithTwoStrikes(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestFirstTimeOutOfSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestFirstTimeIntoSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction);

    static bool mDoneLoading;
    static bool mDoneShowingLoadingTips;
    static void *mLoadingTipsScreenPtr; // size: 0x4, address: 0x8041C188, Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingTips.cpp:21

    uint32 TipTextureHash;         // offset 0x2C
    Timer DisplayTime;             // offset 0x30
    GameTipInfo *CurrentTip;       // offset 0x34
    bool mPressAcceptHasBeenShown; // offset 0x38
};

extern GameTipInfo GameTipInfoTable[];

#endif
