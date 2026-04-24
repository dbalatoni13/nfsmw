#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETLOGIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETLOGIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct uiRapSheetLogin : public MenuScreen {
    int screen; // offset 0x2C, size 0x4
    bool returnToMainMenu; // offset 0x30, size 0x1

    uiRapSheetLogin(ScreenConstructorData* sd);
    ~uiRapSheetLogin() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup();
};

#endif
