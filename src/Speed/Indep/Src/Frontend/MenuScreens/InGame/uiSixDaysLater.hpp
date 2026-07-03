#ifndef FRONTEND_MENUSCREENS_INGAME_UISIXDAYSLATER_H
#define FRONTEND_MENUSCREENS_INGAME_UISIXDAYSLATER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"

class SixDaysLater : public MenuScreen {
  public:
    SixDaysLater(ScreenConstructorData *sd);
    ~SixDaysLater() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    FEString *mpDataMainString; // offset 0x2C
    int mStringMode;            // offset 0x30
};

#endif
