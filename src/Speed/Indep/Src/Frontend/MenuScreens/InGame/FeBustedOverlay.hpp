#ifndef FRONTEND_MENUSCREENS_INGAME_FEBUSTEDOVERLAY_H
#define FRONTEND_MENUSCREENS_INGAME_FEBUSTEDOVERLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class BustedOverlayScreen : public MenuScreen {
  public:
    BustedOverlayScreen(ScreenConstructorData *sd);
    ~BustedOverlayScreen() override;
    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override {}
};

#endif
