#ifndef FRONTEND_MENUSCREENS_LOADING_FELANGUAGESELECT_H
#define FRONTEND_MENUSCREENS_LOADING_FELANGUAGESELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x170
struct LanguageSelectScreen : public IconScrollerMenu {
    LanguageSelectScreen(ScreenConstructorData *sd);
    ~LanguageSelectScreen() override;
    void Setup() override {}
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;

    static bool bChoiceMade;

    Timer StartedTimer; // offset 0x16C
};

#endif
