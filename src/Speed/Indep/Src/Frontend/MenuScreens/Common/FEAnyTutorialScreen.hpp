#ifndef FRONTEND_MENUSCREENS_COMMON_FEANYTUTORIALSCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEANYTUTORIALSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x60
class FEAnyTutorialScreen : public MenuScreen {
  public:
    FEAnyTutorialScreen(struct ScreenConstructorData *sd);
    ~FEAnyTutorialScreen() override;
    static void LaunchMovie(const char *filename, const char *packageName);
    static void DismissMovie(bool send_message);
    static void SetMovieName(const char *filename);
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(u32 msg, struct FEObject *obj, u32 param1, u32 param2) override;
    static void SetPackageName(const char *packageName);

  private:
    static char MovieFilename[64];   // size: 0x40, address: 0x8041B9E4
    static char PackageFilename[64]; // size: 0x40, address: 0x8041BA24
    static bool PackageSet;          // size: 0x1, address: 0x8041BA64

    unsigned int LastTime;  // offset 0x2C, size 0x4
    float TimeElapsed;      // offset 0x30, size 0x4
    float TextToggleTiming; // offset 0x34, size 0x4
    Timer mTimer;           // offset 0x38, size 0x4
    SubTitler mSubtitler;   // offset 0x3C, size 0x24
};
#endif
