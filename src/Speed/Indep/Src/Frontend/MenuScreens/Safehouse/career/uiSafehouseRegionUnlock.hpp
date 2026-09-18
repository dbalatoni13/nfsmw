#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UISAFEHOUSEREGIONUNLOCK_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UISAFEHOUSEREGIONUNLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "uiRepSheetRivalStreamer.hpp"

// total size: 0x74
class uiSafehouseRegionUnlock : public MenuScreen {
  public:
    uiSafehouseRegionUnlock(ScreenConstructorData *sd);

    ~uiSafehouseRegionUnlock() override;

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    void Setup();

    FEImage *pRivalImg;                          // offset 0x2C, size 0x4
    FEImage *pTagImg;                            // offset 0x30, size 0x4
    FEImage *pBGImg;                             // offset 0x34, size 0x4
    uiRepSheetRivalStreamer RivalStreamer;       // offset 0x38, size 0x3C
};

#endif
