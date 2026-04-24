#ifndef FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

// total size: 0x58
struct FEAnyMovieScreen : public MenuScreen {
    SubTitler mSubtitler;           // offset 0x2C
    bool bHidGarage;                // offset 0x50
    bool bAllowingControllerErrors; // offset 0x54

    FEAnyMovieScreen(ScreenConstructorData* sd);
    ~FEAnyMovieScreen() override;
    static MenuScreen* Create(ScreenConstructorData* sd);
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    static void LaunchMovie(const char*, const char*);
    static void PlaySafehouseIntroMovie();
    static void DismissMovie();
    static void SetMovieName(const char*);
    static const char* GetFEngPackageName();
    static char MovieFilename[64];
    static char ReturnToPackageName[64];
};

#endif
