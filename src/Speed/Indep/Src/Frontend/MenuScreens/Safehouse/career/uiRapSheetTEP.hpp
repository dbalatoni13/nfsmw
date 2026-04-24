#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETTEP_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETTEP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

struct uiRapSheetTEP : public UIWidgetMenu {
    unsigned int button_pressed; // offset 0x138, size 0x4
    int num_pursuits; // offset 0x13C, size 0x4

    uiRapSheetTEP(ScreenConstructorData* sd);
    ~uiRapSheetTEP() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
};

#endif
