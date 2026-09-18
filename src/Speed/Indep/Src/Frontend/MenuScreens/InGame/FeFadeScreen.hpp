#ifndef FRONTEND_MENUSCREENS_INGAME_FEFADESCREEN_H
#define FRONTEND_MENUSCREENS_INGAME_FEFADESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class FadeScreen : public MenuScreen {
  public:
    FadeScreen(struct ScreenConstructorData *sd);
    ~FadeScreen() override;
    void NotificationMessage(u32 Message, struct FEObject *pObject, u32 Param1, u32 Param2) override;
    static bool IsFadeScreenOn();
};

#endif
