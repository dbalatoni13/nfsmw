#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRMAINMENU_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRMAINMENU_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

#include <types.h>

// total size: 0x16C
struct UIQRMainMenu : public IconScrollerMenu {
    UIQRMainMenu(ScreenConstructorData *sd);
    ~UIQRMainMenu() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    void RefreshHeader() override;
    void Setup() override;
};

#endif
