#ifndef __UI_RAP_SHEET_MAIN_HPP__
#define __UI_RAP_SHEET_MAIN_HPP__

#include "types.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"

class uiRapSheetMain : public UIWidgetMenu {
  public:
    uiRapSheetMain(ScreenConstructorData *sd);
    ~uiRapSheetMain() override {}
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void RefreshHeader();

    uint32 button_pressed; // offset 0x138, size 0x4
};

#endif
