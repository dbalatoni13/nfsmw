#ifndef FRONTEND_MENUSCREENS_INGAME_INGAMETUTORIALSCREEN_H
#define FRONTEND_MENUSCREENS_INGAME_INGAMETUTORIALSCREEN_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

class InGameAnyTutorialScreen : public MenuScreen {
  public:
    InGameAnyTutorialScreen(ScreenConstructorData *sd);
    ~InGameAnyTutorialScreen() override;
    static void LaunchMovie(const char *filename, const char *packageName);
    void DismissMovie();
    static void SetMovieName(const char *filename);
    static void SetPackageName(const char *packageName);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    static char MovieFilename[64];
    static char PackageFilename[64];
    static bool PackageSet;
    SubTitler mSubtitler; // offset 0x2C
};

#endif
