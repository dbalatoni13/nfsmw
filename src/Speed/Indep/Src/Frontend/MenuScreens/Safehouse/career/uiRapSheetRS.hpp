#ifndef __UI_RAP_SHEET_RS_HPP__
#define __UI_RAP_SHEET_RS_HPP__

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class uiRapSheetRS : public MenuScreen {
  public:
    uiRapSheetRS(ScreenConstructorData *sd);
    ~uiRapSheetRS() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader();
};

#endif
