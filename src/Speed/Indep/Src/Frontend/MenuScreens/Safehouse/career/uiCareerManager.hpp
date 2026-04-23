#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

struct uiCareerManager : public IconScrollerMenu {
    uiCareerManager(ScreenConstructorData* sd);
    ~uiCareerManager() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
};

#endif
