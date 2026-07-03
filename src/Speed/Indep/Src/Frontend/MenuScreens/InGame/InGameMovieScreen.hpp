#ifndef FRONTEND_MENUSCREENS_INGAME_INGAMEMOVIESCREEN_H
#define FRONTEND_MENUSCREENS_INGAME_INGAMEMOVIESCREEN_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

class InGameAnyMovieScreen : public MenuScreen {
  public:
    InGameAnyMovieScreen(ScreenConstructorData *sd);
    ~InGameAnyMovieScreen() override;
    static void LaunchMovie(const char *filename);
    void DismissMovie();
    static void SetMovieName(const char *filename);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    static bool IsPlaying();
    static const char *GetFEngPackageName();

  private:
    static char MovieFilename[64];
    SubTitler mSubtitler;           // offset 0x2C
    bool bAllowingControllerErrors; // offset 0x50
};

#endif
