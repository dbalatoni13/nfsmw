#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRMODESELECT_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRMODESELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

#include <types.h>

// total size: 0x16C
struct UIQRModeSelect : public IconScrollerMenu {
    UIQRModeSelect(ScreenConstructorData *sd);
    ~UIQRModeSelect() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void RefreshHeader() override;
    void Setup() override;
};

#endif
