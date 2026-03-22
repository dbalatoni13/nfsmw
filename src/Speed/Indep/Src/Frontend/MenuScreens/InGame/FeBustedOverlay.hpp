#ifndef FRONTEND_MENUSCREENS_INGAME_FEBUSTEDOVERLAY_H
#define FRONTEND_MENUSCREENS_INGAME_FEBUSTEDOVERLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
struct BustedOverlayScreen : public MenuScreen {
    BustedOverlayScreen(ScreenConstructorData *sd);
    ~BustedOverlayScreen() override;
    void NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) override {}
};

#endif
