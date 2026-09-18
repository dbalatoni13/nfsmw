#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

struct uiCareerCrib : public IconScrollerMenu {
    uiCareerCrib(ScreenConstructorData* sd);
    ~uiCareerCrib() override {}
    void NotificationMessage(u32 msg, FEObject* pobj, u32 param1, u32 param2) override;
    void Setup() override;
};

#endif
