#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIOPTIONSMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIOPTIONSMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// total size: 0x170
class UIOptionsMain : public IconScrollerMenu {
  public:
    UIOptionsMain(ScreenConstructorData *sd);

    ~UIOptionsMain() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void Setup() override;

    void ExitOptions(const char *nextPackage);

    bool mCalledFromPauseMenu; // offset 0x16C, size 0x1
};

#endif
