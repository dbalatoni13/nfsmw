#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

struct uiRapSheetMain : public UIWidgetMenu {
    unsigned int button_pressed; // offset 0x138, size 0x4

    uiRapSheetMain(ScreenConstructorData* sd);
    ~uiRapSheetMain() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader();
};

#endif
