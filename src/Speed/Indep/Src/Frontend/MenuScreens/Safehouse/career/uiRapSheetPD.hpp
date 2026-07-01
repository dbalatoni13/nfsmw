#ifndef __UI_RAP_SHEET_PD_HPP__
#define __UI_RAP_SHEET_PD_HPP__

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class uiRapSheetPD : public MenuScreen {
  public:
    uiRapSheetPD(ScreenConstructorData *sd);
    ~uiRapSheetPD() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup();

    int pursuit_number; // offset 0x2C, size 0x4
};

#endif
