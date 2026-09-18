#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
class uiRapSheetRS : public MenuScreen {
  public:
    uiRapSheetRS(ScreenConstructorData *sd);

    inline virtual ~uiRapSheetRS() {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void RefreshHeader();
};

#endif
