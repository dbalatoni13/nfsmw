#ifndef FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

void DismissChyron();

// total size: 0x58
class FEAnyMovieScreen : public MenuScreen {
  public:
    enum {
        FEMOVIENAMESIZE = 64,
    };

    FEAnyMovieScreen(ScreenConstructorData *sd);

    ~FEAnyMovieScreen() override;

    static MenuScreen *Create(ScreenConstructorData *sd);

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    static void LaunchMovie(const char *return_to_pkg, const char *filename);

    static void PlaySafehouseIntroMovie();

    static void DismissMovie();

    static void SetMovieName(const char *movie_name);

    static const char *GetFEngPackageName();

    SubTitler mSubtitler;          // offset 0x2C, size 0x24
    bool bHidGarage;               // offset 0x50, size 0x1
    bool bAllowingControllerErrors; // offset 0x54, size 0x1

    static char MovieFilename[64];      // size: 0x40
    static char ReturnToPackageName[64]; // size: 0x40
};

#endif
