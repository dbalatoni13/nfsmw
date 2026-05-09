#ifndef FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEANYMOVIESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

// total size: 0x58
class FEAnyMovieScreen : public MenuScreen {
  public:
    FEAnyMovieScreen(ScreenConstructorData *sd);
    ~FEAnyMovieScreen() override;
    static void LaunchMovie(const char *filename);
    static void LaunchMovie(const char *return_to_pkg, const char *filename);
    static void DismissMovie();
    static void SetMovieName(const char *filename);
    static MenuScreen *Create(ScreenConstructorData *sd);
    static void PlaySafehouseIntroMovie();
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    static const char *GetFEngPackageName();

  private:
    static char MovieFilename[64];       // size: 0x40, address: 0x8041B9A4
    struct SubTitler mSubtitler;         // offset 0x2C, size 0x24
    static char ReturnToPackageName[64]; // size: 0x40, address: 0x804FE740
    bool bHidGarage;                     // offset 0x50, size 0x1
    bool bAllowingControllerErrors;      // offset 0x54, size 0x1
};
#endif
