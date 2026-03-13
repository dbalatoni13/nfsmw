#ifndef FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FELOADINGSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct LoadingScreen : public MenuScreen {
    static void InitLoadingScreen();
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
};

#endif
