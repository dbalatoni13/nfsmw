#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"

// total size: 0x13C
class uiRapSheetMain : public UIWidgetMenu {
  public:
    uiRapSheetMain(ScreenConstructorData *sd);

    inline virtual ~uiRapSheetMain() {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void RefreshHeader();

    unsigned int button_pressed; // offset 0x138, size 0x4
};

#endif
