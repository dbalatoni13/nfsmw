#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGTIPS_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGTIPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingScreen.hpp"

enum eGameTips {};

struct GameTipInfo {
    char *Name;            // offset 0x0
    unsigned int Bin;      // offset 0x4
    unsigned int Category; // offset 0x8
    unsigned int Flags;    // offset 0xC
};

struct LoadingTips : public MenuScreen {
    LoadingTips(ScreenConstructorData *sd);
    ~LoadingTips() override;
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    void ShowTipInfo();
    GameTipInfo *GetGameTip(eGameTips tip);
    void StartLoadingTipImage();
    static void InitLoadingTipsScreen();
    void FinishLoadingTexCallback(unsigned int p);
    static void CloseLoadingTipsScreen();
    void AllowInput();
    static eGameTips WhatTipScreenShouldIUseToday(LoadingScreen::LoadingScreenTypes loading_direction);
    static eGameTips GetARandomTipScreen(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestLastCarWithTwoStrikes(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestFirstTimeOutOfSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction);
    static bool TipTestFirstTimeIntoSafeHouse(LoadingScreen::LoadingScreenTypes loading_direction);

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    static bool mDoneLoading;
    static bool mDoneShowingLoadingTips;
    static void *mLoadingTipsScreenPtr;

    unsigned int TipTextureHash;   // offset 0x2C
    Timer DisplayTime;             // offset 0x30
    GameTipInfo *CurrentTip;       // offset 0x34
    bool mPressAcceptHasBeenShown; // offset 0x38
};

extern GameTipInfo GameTipInfoTable[];

#endif
