#ifndef _UIMAIN
#define _UIMAIN

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

// total size: 0x170
struct UIMain : public IconScrollerMenu {
    bool m_bStatsShowing; // offset 0x16C, size 0x1

    UIMain(ScreenConstructorData* sd);
    ~UIMain() override {}

    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void UpdateProfileData();
};

#endif
