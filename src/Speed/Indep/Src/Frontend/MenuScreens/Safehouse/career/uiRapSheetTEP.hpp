#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETTEP_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETTEP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"

class uiRapSheetTEP : public UIWidgetMenu {
  public:
    uiRapSheetTEP(ScreenConstructorData *sd);
    ~uiRapSheetTEP() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;

    uint32 button_pressed; // offset 0x138, size 0x4
    int num_pursuits;      // offset 0x13C, size 0x4
};

#endif
