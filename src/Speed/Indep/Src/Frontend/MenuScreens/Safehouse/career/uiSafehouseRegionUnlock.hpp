#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UISAFEHOUSEREGIONUNLOCK_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UISAFEHOUSEREGIONUNLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

class uiSafehouseRegionUnlock : public MenuScreen {
  public:
    uiSafehouseRegionUnlock(ScreenConstructorData *sd);
    ~uiSafehouseRegionUnlock() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup();

    FEImage *pRivalImg;
    FEImage *pTagImg;
    FEImage *pBGImg;
    uiRepSheetRivalStreamer RivalStreamer;
};

#endif
