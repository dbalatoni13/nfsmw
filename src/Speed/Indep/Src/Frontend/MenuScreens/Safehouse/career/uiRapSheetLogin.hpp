#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETLOGIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETLOGIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

class uiRapSheetLogin : public MenuScreen {
  public:
    uiRapSheetLogin(ScreenConstructorData *sd);
    ~uiRapSheetLogin() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup();

    int screen;            // offset 0x2C, size 0x4
    bool returnToMainMenu; // offset 0x30, size 0x1
};

#endif
