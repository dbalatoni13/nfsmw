// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRPRESSSTART_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRPRESSSTART_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

#include <types.h>

// total size: 0x34
struct uiQRPressStart : public MenuScreen {
    uiQRPressStart(ScreenConstructorData *sd);
    ~uiQRPressStart() override;

    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;

    void Setup();

    int iPlayerNum; // offset 0x2C, size 0x4
    int param;      // offset 0x30, size 0x4
};

#endif
