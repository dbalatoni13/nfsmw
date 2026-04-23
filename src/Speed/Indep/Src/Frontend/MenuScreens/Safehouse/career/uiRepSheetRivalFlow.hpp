#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALFLOW_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

// total size: 0x8
struct uiRepSheetRivalFlow {
    enum Stage {
        CHALLENGE_SCREEN = 0,
        MARKER_SELECTION = 1,
        REGION_UNLOCK = 2,
        SAVE_FLOW = 3,
        BIO_NEW_RIVAL = 4,
        BIO_MOVIE = 5,
        BACK_TO_FREE_ROAM = 6,
        IN_GAME_BLACKLIST = 7,
    };

    int mStage; // offset 0x0, size 0x4
    // vtable at 0x4

    static uiRepSheetRivalFlow* mInstance;

    uiRepSheetRivalFlow();
    virtual ~uiRepSheetRivalFlow() {}

    static void Init();
    static void Destroy();
    static uiRepSheetRivalFlow* Get();

    void StartFlow(int start_stage);
    void Next();

    int GetStage() { return mStage; }
};

#endif
