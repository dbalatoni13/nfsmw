#ifndef FRONTEND_DATABASE_UIPROFILEMANAGER_H
#define FRONTEND_DATABASE_UIPROFILEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

struct PMSave;

// total size: 0x170
struct UIProfileManager : public IconScrollerMenu {
    PMSave* mpSave; // offset 0x16C

    UIProfileManager(ScreenConstructorData* sd);
    ~UIProfileManager() override;
    void Refresh();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    void Setup() override;
};

MenuScreen* CreateUIProfileManager(ScreenConstructorData* sd);

#endif
