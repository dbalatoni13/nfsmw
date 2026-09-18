#ifndef FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H
#define FRONTEND_MENUSCREENS_CAREER_FEGAMEWONSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class FEGameWonScreen : public MenuScreen {
  public:
    enum {
        eYOU_WIN = 0,
        eCREDITS = 1,
        eRAP_SHEET_LOGIN = 2,
        eRAP_SHEET_LOGIN2 = 3,
        eRAP_SHEET_MAIN = 4,
        eDONE_WIN_FLOW_NOW_GO_BACK_TO_THE_SAFE_HOUSE = 5,
    };

    FEGameWonScreen(ScreenConstructorData *sd);

    ~FEGameWonScreen() override;

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    static void QueuePackageSwitchForNextScreen();

    static void Initialize();

    static int mCurrentScreen; // size: 0x4
};

#endif
