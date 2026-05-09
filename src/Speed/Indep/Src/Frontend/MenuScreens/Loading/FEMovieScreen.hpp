#ifndef FRONTEND_MENUSCREENS_LOADING_FEMOVIESCREEN_H
#define FRONTEND_MENUSCREENS_LOADING_FEMOVIESCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"

class MovieScreen : public MenuScreen {
  public:
    MovieScreen(ScreenConstructorData *);
    ~MovieScreen() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    bool bByPassable;     // offset 0x2C
    SubTitler mSubtitler; // offset 0x30
};

#endif
