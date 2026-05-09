#ifndef FRONTEND_MENUSCREENS_LOADING_FELANGUAGESELECT_H
#define FRONTEND_MENUSCREENS_LOADING_FELANGUAGESELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x170
class LanguageSelectScreen : public IconScrollerMenu {
  public:
    LanguageSelectScreen(ScreenConstructorData *sd);
    ~LanguageSelectScreen() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    void Setup() override {}

  private:
    static bool bChoiceMade;

    Timer StartedTimer; // offset 0x16C
};

#endif
