#ifndef FRONTEND_MENUSCREENS_COMMON_FEANYTUTORIALSCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEANYTUTORIALSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x60
struct FEAnyTutorialScreen : public MenuScreen {
    unsigned int LastTime;    // offset 0x2C
    float TimeElapsed;        // offset 0x30
    float TextToggleTiming;   // offset 0x34
    Timer mTimer;             // offset 0x38
    SubTitler mSubtitler;     // offset 0x3C

    FEAnyTutorialScreen(ScreenConstructorData* sd);
    ~FEAnyTutorialScreen() override;
    static MenuScreen* Create(ScreenConstructorData* sd);
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    static void LaunchMovie(const char*, const char*);
    static void DismissMovie(bool);
    static void SetMovieName(const char*);
    static void SetPackageName(const char*);
    static char MovieFilename[64];
    static char PackageFilename[64];
    static bool PackageSet;
};

#endif
