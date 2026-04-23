#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UICREDITS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UICREDITS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/UnicodeFile.hpp"

struct FEString;

// total size: 0x48
struct uiCredits : public MenuScreen {
    bool initComplete_;       // offset 0x2C
    FEString* prototypeStr_;  // offset 0x30
    short* creditLine_;       // offset 0x34
    FEObject* pendingDelete_; // offset 0x38
    UnicodeFile uf_;          // offset 0x3C

    uiCredits(ScreenConstructorData* sd);
    ~uiCredits() override;
    static MenuScreen* Create(ScreenConstructorData* sd);
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
};

#endif
