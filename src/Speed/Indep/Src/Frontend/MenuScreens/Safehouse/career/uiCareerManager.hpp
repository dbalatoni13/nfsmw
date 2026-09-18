#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

struct uiCareerManager : public IconScrollerMenu {
    uiCareerManager(ScreenConstructorData* sd);
    ~uiCareerManager() override {}
    void NotificationMessage(u32 msg, FEObject* pobj, u32 param1, u32 param2) override;
    void Setup() override;
};

#endif
